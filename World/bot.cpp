#include "bot.h"
#include "world.h"
#include "rules.h"

CBot::CBot() : CEntity()
{
    Reset();

    playerId.binaryAddress = random( 10000 );
    playerId.port = random( 10000 );

    player = WORLD.GetPlayers()->AddPlayer( playerId );
    WORLD.GetRules()->OnNewPlayerJoin( player );
    player->getControls()->MapKeys( false );
}

CBot::~CBot()
{
    WORLD.GetPlayers()->RemovePlayer( playerId );
}

void CBot::Reset()
{
    state = BOT_DEFAULT;
    seekType = NODECLASS_CHARACTER;
    myActor = target = NULL;
}

void CBot::Think()
{
    targetDistance = getTargetDistance();

    if ( myActor )
    {
      ChangeState();

      player->getControls()->ClearKeys();

      if ( target )
      {
        switch ( state )
        {
          case (BOT_DEFAULT):
            break;

          case (BOT_SEEK_TARGET):
            if ( target->getPosition().X > myActor->getPosition().X )
            {
              player->getControls()->SetActionKey( AK_MOVE_RIGHT, true );
            }
            else
            {
              player->getControls()->SetActionKey( AK_MOVE_LEFT, true );
            }

            if ( targetDistance < 5.0f )
            {
              player->getControls()->SetActionKey( AK_MOUNT, true );
            }

            player->getControls()->SetActionKey( AK_THRUST, false );
            break;

          case (BOT_IN_MACHINE):
            player->getControls()->SetActionKey( AK_THRUST, true );
            if ( targetDistance > 10.0f )
            {
              matrix4 mat;
              NewtonBodyGetMatrix( myActor->getParentAttachment()->body, &mat.M[0] ); 
              vector3df vNMainAxis = vector3df( -1.0f, 0.0f, 0.0f );
              vector3df vNUpAxis = vector3df( 0.0f, 1.0f, 0.0f );
              mat.rotateVect( vNMainAxis );
              mat.rotateVect( vNUpAxis );

              headingPos = vNMainAxis * targetDistance;

              if ( ( ( target->getPosition().X > myActor->getPosition().X ) && ( vNMainAxis.X < 0.0f ) ) || ( ( target->getPosition().X < myActor->getPosition().X ) && ( vNMainAxis.X > 0.0f ) ) )
              {
                player->getControls()->SetActionKey( AK_MOVE_DOWN, true );
                break;
              }

              if ( vNUpAxis.Y < 0.0f )
              {
                player->getControls()->SetActionKey( AK_FLIP, true );
              }

              vTargetDiff = target->getPosition() - ( myActor->getPosition() + headingPos );

              if ( vTargetDiff.Y > targetDistance / 16.0f )
              {
                player->getControls()->SetActionKey( AK_MOVE_UP, true );
              }
              else if ( vTargetDiff.Y < -targetDistance / 16.0f )
              {
                player->getControls()->SetActionKey( AK_MOVE_DOWN, true );
              }
              else if ( vTargetDiff.getLength() < abs( targetDistance / 22.0f ) )
              {
                player->getControls()->SetActionKey( AK_FIRE, true );
              }
            }

            break;

          default:
            break;
        } //switch (state)

        player->getControls()->mousePosWorld = target->getPosition();
      } //if (target)

      if ( myActor->isZombie() )
      {
        myActor = NULL;
      }
    }
    else
    {
      myActor = FindMyActor();
    }

    if ( target )
    {
      if ( target->isZombie() )
      {
        target = NULL;
      }
    }
}

void CBot::Render()
{
    if ( APP.DebugMode )
    {
      if ( myActor )
      {
        IRR.video->setTransform( ETS_WORLD, matrix4() );

        SMaterial m; 
        m.Lighting = false; 
        m.BackfaceCulling = false;
        IRR.video->setMaterial( m );

        vector3df vP1, vP2;
        vP1 = myActor->getPosition();
        vP2 = vP1 + headingPos;
        if ( player->getControls()->ActionKeyPressed( AK_MOVE_DOWN ) )
        {
          IRR.video->draw3DLine( vP1, vP2, SColor( 0, 125, 255, 1 ) );
        }
        if ( player->getControls()->ActionKeyPressed( AK_MOVE_UP ) )
        {
          IRR.video->draw3DLine( vP1, vP2, SColor( 0, 245, 125, 1 ) );
        }
      }

      if ( target )
      {
        position2d<s32> screenPos = target->getDebugPos();

        IRR.gui->getBuiltInFont()->draw( L"<TARGET>", core::rect<s32>( screenPos.X, screenPos.Y - 25, screenPos.X + 100, screenPos.Y + 50 ), irr::video::SColor( 255, 245, 225, 55 ), false, true );
      }

      WideString sVel = "state: ";
      sVel += f2wchar( state );
      IRR.gui->getBuiltInFont()->draw( sVel.c_str(), core::rect<s32>( 10, IRR.getScreenHeight() - 592, IRR.getScreenWidth(), IRR.getScreenHeight() ), irr::video::SColor( 255, 165, 15, 65 ), false, true );
    }
}

void CBot::ChangeState()
{
    // myActor true here

    switch ( state )
    {
      case (BOT_DEFAULT):
        //CONSOLE.add("ChangeState");
        target = FindTarget( seekType );        
        if ( target )
        {
          state = BOT_SEEK_TARGET;
        }
        break;

      case (BOT_SEEK_TARGET):
        if ( myActor->isAttached() ) // if in machine
        {
          target = FindTarget( seekType );
          if ( target )
          {
            state = BOT_IN_MACHINE;
          }
        }

        if ( !target )
        {
          state = BOT_DEFAULT;
        }
        break;

      case (BOT_IN_MACHINE):
        if ( !myActor->isAttached() )
        {
          target = NULL;
        }
        if ( !target )
        {
          state = BOT_DEFAULT;
        }
        break;

      default:
        break;
    }
}

CActor* CBot::FindTarget( int seeknodetype )
{
    array<CActor*> actors;

    for ( int i = 0; i < CActor::actorsList.size(); i++ )
    {
      if ( CActor::actorsList[i]->getControls() != myActor->getControls() )
      {
        if ( CActor::actorsList[i]->getType() == seeknodetype )
        {
          if ( APP.DebugMode )
          {
            CONSOLE.add( "FindTarget: bot found new target" );
          }
          actors.push_back( CActor::actorsList[i] );
        }
      }
    }

    // pick closest
    if ( actors.size() )
    {
      f32 dist, maxdist;
      int pick = 0;
      maxdist = 9999999.9f;
      for ( int i = 0; i < actors.size(); i++ )
      {
        if ( !actors[i]->getAttachedChild( NODECLASS_CHARACTER ) )
        {
          dist = ( actors[i]->getPosition() - myActor->getPosition() ).getLength();
          if ( dist < maxdist )
          {
            pick = i;
            maxdist = dist;
          }
        }
      }

      return actors[pick];
    }

    return NULL;
}

CActor* CBot::FindMyActor()
{
    for ( int i = 0; i < CActor::actorsList.size(); i++ )
    {
      if ( CActor::actorsList[i]->getControls() == player->getControls() )
      {
        if ( APP.DebugMode )
        {
          CONSOLE.add( "FindMyActor: bot found his self" );
        }
        return CActor::actorsList[i];
      }
    }
    return NULL;
}

f32 CBot::getTargetDistance()
{
    if ( ( target ) && ( myActor ) )
    {
      return ( target->getPosition() - myActor->getPosition() ).getLength();
    }   
    return 999999.9f;
}
