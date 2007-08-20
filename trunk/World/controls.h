#ifndef CONTROLS_H_INCLUDED
#define CONTROLS_H_INCLUDED

#include "../Engine/engine.h"

enum E_ACTIONKEYS { AK_MOVE_LEFT = 0, AK_MOVE_RIGHT, AK_MOVE_UP, AK_MOVE_DOWN, AK_MOUNT, AK_DISMOUNT, AK_PICKUP, AK_DROP, AK_THRUST, AK_FLIP, AK_FIRE, AK_FIRE2, AK_RELOAD, AK_BRAKE, AK_NUM };

static char* ActionKeyNames[] =
{
  "AK_MOVE_LEFT", "AK_MOVE_RIGHT", "AK_MOVE_UP", "AK_MOVE_DOWN", "AK_MOUNT", "AK_DISMOUNT", "AK_PICKUP", "AK_DROP", "AK_THRUST", "AK_FLIP", "AK_FIRE", "AK_FIRE2", "AK_RELOAD", "AK_BRAKE"
};

#define MENUITEM_ID 10000


class CControls
{
  public:
    CControls();
    virtual ~CControls();

    void MapKeys( bool fromConsole );

    bool ActionKeyPressed( s32 ak );
    bool ActionKeyReleased( s32 ak );
    void MapActionKey( s32 ak, s32 keycode );
    void SetActionKey( s32 ak, bool pressed );

    bool KeyPressed( s32 keycode );
    void SetKey( s32 keycode, bool pressed );
    bool NoKeysPressed();
    bool OnlyKeyPressed( s32 keycode );
    void ClearKeys();

    // Context Menu
    void NewContextMenu();
    void NewContextMenu( position2d<s32> pos );
    void RemoveContextMenu();
    s32 AddContextMenuItem( WideString name, s32 id, IGUIContextMenu* submenu = NULL, bool useSubmenu = false );
    bool isContextMenuActive();
    void PositionContextMenu( position2d<s32> pos );
    s32 popContextMenuSelection(); // return number < 0 if nothing
    void setContextMenuSelection( s32 sel );
    void setContextMenuFirstItemSelection();
    void setContextMenuSelectionText( WideString text );
    s32 getContextMenuItemsCount();
    IGUIContextMenu* getContextMenuSubMenu( s32 id );
    WideString getContextMenuLastSelectionText();

    bool mousePressed1, mousePressed2;
    position2d<s32> mousePos;
    vector3df mousePosWorld;

  private:
    bool keys[irr::KEY_KEY_CODES_COUNT];
    s32 actionkeys[AK_NUM];
    bool releasedActionkeys[AK_NUM];

    IGUIContextMenu* menu;
    s32 selectMenuItem;
    WideString selectMenuText;
};

extern CControls myControls;

#endif