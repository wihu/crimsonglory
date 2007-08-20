#include "controls.h"

#include "../Irrlicht/IrrlichtTask.h"
#include "../IrrConsole/console_vars.h"

CControls myControls;

static bool keys_loaded = false;

#define MENUITEM_ID 10000

CControls::CControls()
{
    ClearKeys();

    menu = NULL;
}

CControls::~CControls()
{
    if ( ( IRR.smgr ) && ( isContextMenuActive() ) )
    {
      menu->remove();
    }
}

void CControls::MapKeys( bool fromConsole )
{
    // set default action key mapping
    if ( ( fromConsole ) && ( !keys_loaded ) )
    {
      CONSOLE_VAR( "key_moveleft", s32, actionkeys[AK_MOVE_LEFT], KEY_KEY_A, L"key_moveleft [hex]. Ex. key_moveleft 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_moveright", s32, actionkeys[AK_MOVE_RIGHT], KEY_KEY_D, L"key_moveright [hex]. Ex. key_moveright 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_moveup", s32, actionkeys[AK_MOVE_UP], KEY_KEY_W, L"key_moveup [hex]. Ex. key_moveup 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_movedown", s32, actionkeys[AK_MOVE_DOWN], KEY_KEY_S, L"key_movedown [hex]. Ex. key_movedown 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_mount", s32, actionkeys[AK_MOUNT], KEY_KEY_F, L"key_mount [hex]. Ex. key_mount 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_dismount", s32, actionkeys[AK_DISMOUNT], KEY_KEY_G, L"key_dismount [hex]. Ex. key_dismount 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_pickup", s32, actionkeys[AK_PICKUP], KEY_KEY_V, L"key_pickup [hex]. Ex. key_pickup 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_drop", s32, actionkeys[AK_DROP], KEY_KEY_B, L"key_drop [hex]. Ex. key_drop 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_thrust", s32, actionkeys[AK_THRUST], KEY_SHIFT, L"key_thrust [hex]. Ex. key_thrust 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_flip", s32, actionkeys[AK_FLIP], KEY_SPACE, L"key_flip [hex]. Ex. key_flip 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_fire", s32, actionkeys[AK_FIRE], KEY_LBUTTON, L"key_fire [hex]. Ex. key_fire 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_fire2", s32, actionkeys[AK_FIRE2], KEY_RBUTTON, L"key_fire2 [hex]. Ex. key_fire2 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_reload", s32, actionkeys[AK_RELOAD], KEY_KEY_R, L"key_reload [hex]. Ex. key_reload 0xc0", L"Sets the key mapping." );
      CONSOLE_VAR( "key_brake", s32, actionkeys[AK_BRAKE], KEY_KEY_X, L"key_brake [hex]. Ex. key_brake 0xc0", L"Sets the key mapping." );
      keys_loaded = true;
    }

    if ( !fromConsole )
    {
      actionkeys[AK_MOVE_LEFT] = KEY_KEY_A;
      actionkeys[AK_MOVE_RIGHT] = KEY_KEY_D;
      actionkeys[AK_MOVE_UP] = KEY_KEY_W;
      actionkeys[AK_MOVE_DOWN] = KEY_KEY_S;
      actionkeys[AK_MOUNT] = KEY_KEY_F;
      actionkeys[AK_DISMOUNT] = KEY_KEY_G;
      actionkeys[AK_PICKUP] = KEY_KEY_V;
      actionkeys[AK_DROP] = KEY_KEY_B;
      actionkeys[AK_THRUST] = KEY_SHIFT;
      actionkeys[AK_FLIP] = KEY_SPACE;
      actionkeys[AK_FIRE] = KEY_LBUTTON;
      actionkeys[AK_FIRE2] = KEY_RBUTTON;
      actionkeys[AK_RELOAD] = KEY_KEY_R;
      actionkeys[AK_BRAKE] = KEY_KEY_X;
    }
}

bool CControls::ActionKeyPressed( s32 ak )
{
    if ( ( actionkeys[ak] > -1 ) && ( actionkeys[ak] < irr::KEY_KEY_CODES_COUNT ) )
    {
      return KeyPressed( actionkeys[ak] );
    }
    else
    {
      return false;
    }
}

bool CControls::ActionKeyReleased( s32 ak )
{
    return false;
}

void CControls::MapActionKey( s32 ak, s32 keycode )
{
    actionkeys[ak] = keycode;
}

void CControls::SetActionKey( s32 ak, bool pressed )
{
    if ( ( actionkeys[ak] > -1 ) && ( actionkeys[ak] < irr::KEY_KEY_CODES_COUNT ) )
    {
      keys[actionkeys[ak]] = pressed;
    }
}

bool CControls::KeyPressed( s32 keycode )
{
    if ( ( keycode > -1 ) && ( keycode < irr::KEY_KEY_CODES_COUNT ) )
    {
      if ( keys[keycode] )
      {
        return true;
      }
    }
    return false;
}

void CControls::SetKey( s32 keycode, bool pressed )
{
    keys[keycode] = pressed;
}

bool CControls::NoKeysPressed()
{
    //if (mouse1)
    //  return false;

    for ( s32 x = 0; x < irr::KEY_KEY_CODES_COUNT; x++ )
    {
      if ( keys[x] )
      {
        return false;
      }
    }

    return true;
}

bool CControls::OnlyKeyPressed( s32 keycode )
{
    if ( !keys[keycode] )
    {
      return false;
    }

    //if (mouse1) 
    //  return false;

    for ( s32 x = 0; x < irr::KEY_KEY_CODES_COUNT; x++ )
    {
      if ( x != keycode )
      {
        if ( keys[x] )
        {
          return false;
        }
      }
    }

    return true;
}

void CControls::ClearKeys()
{
    for ( s32 x = 0; x < irr::KEY_KEY_CODES_COUNT; x++ )
    {
      keys[x] = false;
    }
}

///

void CControls::NewContextMenu()
{
    RemoveContextMenu();
    menu = IRR.gui->addContextMenu( rect<s32>( 19, 19, 19 + 100, 19 + 10 ), 0, MENUITEM_ID );
}

void CControls::NewContextMenu( position2d<s32> pos )
{
    RemoveContextMenu();
    menu = IRR.gui->addContextMenu( rect<s32>( pos.X, pos.Y, pos.X + 100, pos.Y + 10 ), 0, MENUITEM_ID );
}

void CControls::RemoveContextMenu()
{
    if ( isContextMenuActive() )
    {
      menu->remove();
    }
}

s32 CControls::AddContextMenuItem( WideString name, s32 id, IGUIContextMenu* submenu, bool useSubmenu )
{
    s32 idx = -1;
    if ( isContextMenuActive() )
    {
      //CONSOLE.addx( "AddContextMenuItem ID %i", MENUITEM_ID + id + 1 );
      if ( !submenu )
      {
        idx = menu->addItem( name.c_str(), MENUITEM_ID + id + 1, true, useSubmenu );
      }
      else
      {
        idx = submenu->addItem( name.c_str(), MENUITEM_ID + id + 1, true, useSubmenu );
      }
    }

    return idx;
}

bool CControls::isContextMenuActive()
{
    IGUIElement* e = NULL;
    e = IRR.gui->getRootGUIElement()->getElementFromId( MENUITEM_ID, true );
    return e;
}

void CControls::PositionContextMenu( position2d<s32> pos )
{
    rect<s32> rect = menu->getAbsolutePosition();
    position2d<s32> oldpos = menu->getAbsolutePosition().UpperLeftCorner;
    oldpos.X *= -1;
    oldpos.Y *= -1;

    menu->move( oldpos );
    menu->move( pos );
}

s32 CControls::popContextMenuSelection()
{
    s32 r = selectMenuItem - MENUITEM_ID - 1;
    selectMenuItem = -1;
    return r;
}

void CControls::setContextMenuSelection( s32 sel )
{
    selectMenuItem = sel;
}

void CControls::setContextMenuFirstItemSelection()
{
    selectMenuItem = menu->getItemCommandId( 0 );
    menu->remove();
}

void CControls::setContextMenuSelectionText( WideString text )
{
    selectMenuText = text;
}

s32 CControls::getContextMenuItemsCount()
{
    return menu->getItemCount();
}

IGUIContextMenu* CControls::getContextMenuSubMenu( s32 id )
{
    IGUIContextMenu* submenu = ( IGUIContextMenu* )menu->getSubMenu( id );
    return submenu;
}

WideString CControls::getContextMenuLastSelectionText()
{
    return selectMenuText;
}