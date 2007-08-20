#include "editor.h"

#include "../Game/SingletonIncludes.h"
#include "actor.h"
#include "controls.h"
#include "map.h"
#include "../Engine/misc.h"
#include "../Newton/newton_node.h"

#define EDITOR_SCRIPT "Scripts/editor.gm"
#define EDITOR_EVENT "Scripts/editorevent.gm"
#define ACTOR_MENUS_NUM 2

////////////////////////////////////////////
// CEditor 
////////////////////////////////////////////

CEditor::CEditor()
{
    Reset();

    SCRIPT.RunFile( EDITOR_SCRIPT );

    // make context menu
    SCRIPT.gmTableToStringArray( "editor_ContextMenu", cmenu );
    actorConfigFile = "Scripts/hydroplane.gm";

    GMVARTOVALUE( "editor_pickRadius", f32, pickRadius );
}

CEditor::~CEditor()
{
}

void CEditor::Reset()
{
    actorPick = respawnPick = -1;
}

void CEditor::Think()
{
    SelectAtCursor( pickRadius );

    // context menu handling
    s32 menusel = myControls.popContextMenuSelection();
    s32 actorsSize = FACTORY->Actors.indexArray.size();

    // little explaining, this code is really wierd:
    // the first context menus are managed here, the rest is in the script
    // the first context menus have submenus, each submenu has actorsSize items
    // the script items ids start at the end of the first contetx menus and their submenus
    // Irrlicht doesn't use ID for submenu items, they are command ID's
    // however I made command ID's the same as ID in the script

    if ( menusel > -1 )
    {
      s32 start = ACTOR_MENUS_NUM* ACTOR_MENUS_NUM + ACTOR_MENUS_NUM* FACTORY->Actors.indexArray.size();

      if ( menusel < start ) // do here
      {
        //CONSOLE.addx( "MN %i   start %i actorsSize %i", menusel, start, actorsSize );
        // add actor
        if ( menusel - ACTOR_MENUS_NUM * ACTOR_MENUS_NUM < actorsSize )
        {
          //                CONSOLE.add( myControls.getContextMenuLastSelectionText() );
          //actorConfigFile = "Scripts/hydroplane.gm";
          if ( wordInString( actorConfigFile, myControls.getContextMenuLastSelectionText().c_str() ) )
          {
            String text = myControls.getContextMenuLastSelectionText().c_str();
            CActor::CreateActor( text.c_str(), actorConfigFile.c_str(), 0, 0, menuStartMousePosWorld, text.c_str() );
            //WORLD.GetMap()->GetRespawn()->AddToQueue( text, actorConfigFile, 5, 0 );
          }
          else
          {
            if ( APP.DebugMode )
            {
              CONSOLE.add( "Editor: Invalid config file loaded", COLOR_ERROR );
            }
            IRR.gui->addMessageBox( L"Editor", L"Invalid config file loaded or not loaded at all. Config filename must start with the actors name", true, EMBF_OK, 0, -1 );
          }
        }
        // add respawn
        if ( menusel - ACTOR_MENUS_NUM * ACTOR_MENUS_NUM >= actorsSize )
        {
          if ( WORLD.GetMap() )
          {
            WORLD.GetMap()->GetRespawn()->AddPoint( menuStartMousePosWorld, myControls.getContextMenuLastSelectionText().c_str(), 0 );
          }
        }
      }
      else // do in script
      {
        WideString wstr = "global editor_menu_selection = "; wstr += menusel; wstr += ";";
        wstr += "global editor_mouse_x = "; wstr += f2wchar( menuStartMousePosWorld.X ); wstr += ";";
        wstr += "global editor_mouse_y = "; wstr += f2wchar( menuStartMousePosWorld.Y ); wstr += ";";
        wstr += "global editor_id_start = "; wstr += start; wstr += ";";
        SCRIPT.RunString( wstr );
        SCRIPT.RunFile( EDITOR_EVENT );
      }
    }

    if ( !myControls.isContextMenuActive() )
    {
      menuStartMousePosWorld = myControls.mousePosWorld;
    }

    if ( myControls.ActionKeyPressed( AK_MOUNT ) )
    {
      if ( !myControls.isContextMenuActive() )
      {
        int i;
        myControls.NewContextMenu( myControls.mousePos );
        //menuStartMousePosWorld = myControls.mousePosWorld;

        // create add actors submenus
        s32 id1 = myControls.AddContextMenuItem( "Add Actor", 0, NULL, true );
        s32 id2 = myControls.AddContextMenuItem( "Add Respawn", 1, NULL, true );
        IGUIContextMenu* menu = myControls.getContextMenuSubMenu( 0 );
        IGUIContextMenu* menu2 = myControls.getContextMenuSubMenu( 1 );
        menu->setID( MENUITEM_ID + 0 + 1 );
        menu2->setID( MENUITEM_ID + 1 + 1 );
        if ( ( menu ) && ( menu2 ) )
        {
          // add the registered actors list
          for ( i = 0; i < actorsSize; i++ )
          {
            myControls.AddContextMenuItem( FACTORY->Actors.indexArray[i].c_str(), ACTOR_MENUS_NUM * ACTOR_MENUS_NUM + i, menu, false );
            myControls.AddContextMenuItem( FACTORY->Actors.indexArray[i].c_str(), ACTOR_MENUS_NUM * ACTOR_MENUS_NUM + actorsSize + i, menu2, false );
          }
        }

        // add menu items from script
        if ( cmenu.size() )
        {
          for ( i = 0; i < cmenu.size(); i++ )
          {
            myControls.AddContextMenuItem( cmenu[i].c_str(), ACTOR_MENUS_NUM * ACTOR_MENUS_NUM + ACTOR_MENUS_NUM * actorsSize + i );
          }
        }
      }
    }
}

void CEditor::Render()
{
    WideString wstr = "EDITOR ON";
    IRR.gui->getBuiltInFont()->draw( wstr.c_str(), core::rect<s32>( 10, 11, 100, 100 ), irr::video::SColor( 255, 245, 245, 240 ), false, true );

    wstr = "actor config: '";
    wstr += actorConfigFile.c_str(); wstr += "'"; 
    IRR.gui->getBuiltInFont()->draw( wstr.c_str(), core::rect<s32>( 10, 21, 100, 120 ), irr::video::SColor( 255, 245, 245, 240 ), false, true );

    // draw 3d stuff
    IRR.video->setTransform( ETS_WORLD, matrix4() );

    SMaterial m; 
    m.Lighting = false; 
    m.BackfaceCulling = false;
    IRR.video->setMaterial( m );

    if ( actorPick > -1 )
    {
      vector3df vP = CActor::actorsList[actorPick]->getPosition();
      //IRR.video->draw3DBox( CActor::actorsList[actorPick]->getBoundingBox(), SColor( 255, 55, 252, 40 ) );
      IRR.video->draw3DBox( aabbox3df( vP - vector3df( pickRadius, pickRadius, pickRadius ), vP + vector3df( pickRadius, pickRadius, pickRadius ) ), SColor( 255, 255, 252, 40 ) );
    }

    if ( !WORLD.GetMap() )
    {
      return;
    }

    if ( respawnPick > -1 )
    {
      vector3df vP = WORLD.GetMap()->GetRespawn()->points[respawnPick]->getPosition();
      IRR.video->draw3DBox( aabbox3df( vP - vector3df( pickRadius, pickRadius, pickRadius ), vP + vector3df( pickRadius, pickRadius, pickRadius ) ), SColor( 255, 255, 52, 240 ) );
    }

    for ( int i = 0; i < WORLD.GetMap()->Sprites.size(); i++ )
    {
      vector3df vP = WORLD.GetMap()->Sprites[i]->getPosition();
      IRR.video->draw3DBox( aabbox3df( vP - vector3df( pickRadius, pickRadius, pickRadius ), vP + vector3df( pickRadius, pickRadius, pickRadius ) ), SColor( 255, 55, 52, 240 ) );
    }
}

void CEditor::DeleteActorAtCursor( f32 pickradius )
{
    if ( actorPick == -1 )
    {
      return;
    }

    int i = actorPick;
    CNewtonNode* childnode = CActor::actorsList[i]->DropChild();
    while ( childnode )
    {
      if ( childnode->getType() == NODECLASS_WEAPON )
      {
        CActor* actor = static_cast<CActor*>( childnode );
        WORLD.RemoveEntity( actor );
      }
      childnode = CActor::actorsList[i]->DropChild();
    }

    WORLD.RemoveEntity( CActor::actorsList[i] );
    actorPick = -1;
}

void CEditor::DeleteRespawnAtCursor( f32 pickradius )
{
    if ( respawnPick == -1 )
    {
      return;
    }

    WORLD.GetMap()->GetRespawn()->points.erase( respawnPick );
    respawnPick = -1;
}

void CEditor::SelectAtCursor( f32 pickradius )
{
    int i;
    actorPick = respawnPick = -1;

    for ( i = 0; i < CActor::actorsList.size(); i++ )
    {
      if ( ( CActor::actorsList[i]->getPosition() - menuStartMousePosWorld ).getLength() < pickradius )
      {
        actorPick = i;
        break;
      }
    }

    for ( i = 0; i < WORLD.GetMap()->GetRespawn()->points.size(); i++ )
    {
      if ( ( WORLD.GetMap()->GetRespawn()->points[i]->getPosition() - menuStartMousePosWorld ).getLength() < pickradius )
      {
        respawnPick = i;
        break;
      }
    }
}