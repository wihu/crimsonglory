#include "scriptGUIFunctions.h"
#include "../Game/GameDLL.h"
#include "../Game/SingletonIncludes.h"

// SCRIPTBIND(gmGUIClear, "GUIClear");
int GM_CDECL gmGUIClear( gmThread* a_thread )
{
    IRR.clearGUI();
    return GM_OK;
}

// SCRIPTBIND(gmGUIAddButton, "GUIAddButton");
int GM_CDECL gmGUIAddButton( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 7 ); 
    GM_CHECK_INT_PARAM( x1, 0 );
    GM_CHECK_INT_PARAM( y1, 1 );
    GM_CHECK_INT_PARAM( x2, 2 );
    GM_CHECK_INT_PARAM( y2, 3 );
    GM_CHECK_INT_PARAM( owner, 4 );
    GM_CHECK_INT_PARAM( id, 5 );
    GM_CHECK_STRING_PARAM( caption, 6 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );
    WideString wcaption = caption;

    f32 ratiox = ( f32 )IRR.getScreenWidth() / DEFAULT_SCREEN_WIDTH;
    f32 ratioy = ( f32 )IRR.getScreenHeight() / DEFAULT_SCREEN_HEIGHT;

    IRR.gui->addButton( GUIRECT, parent, id, wcaption.c_str() );

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddStaticText, "GUIAddStaticText");
int GM_CDECL gmGUIAddStaticText( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 7 ); 
    GM_CHECK_INT_PARAM( x1, 0 );
    GM_CHECK_INT_PARAM( y1, 1 );
    GM_CHECK_INT_PARAM( x2, 2 );
    GM_CHECK_INT_PARAM( y2, 3 );
    GM_CHECK_INT_PARAM( owner, 4 );
    GM_CHECK_INT_PARAM( border, 5 );
    GM_CHECK_STRING_PARAM( caption, 6 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );
    WideString wcaption = caption;

    f32 ratiox = ( f32 )IRR.getScreenWidth() / DEFAULT_SCREEN_WIDTH;
    f32 ratioy = ( f32 )IRR.getScreenHeight() / DEFAULT_SCREEN_HEIGHT;

    IRR.gui->addStaticText( wcaption.c_str(), GUIRECT, ( bool )border, false, parent );

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddComboBox, "GUIAddComboBox");
// ??
int GM_CDECL gmGUIAddComboBox( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 6 ); 
    GM_CHECK_INT_PARAM( x1, 0 );
    GM_CHECK_INT_PARAM( y1, 1 );
    GM_CHECK_INT_PARAM( x2, 2 );
    GM_CHECK_INT_PARAM( y2, 3 );
    GM_CHECK_INT_PARAM( owner, 4 );
    GM_CHECK_INT_PARAM( id, 5 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );

    f32 ratiox = ( f32 )IRR.getScreenWidth() / DEFAULT_SCREEN_WIDTH;
    f32 ratioy = ( f32 )IRR.getScreenHeight() / DEFAULT_SCREEN_HEIGHT;

    IRR.gui->addComboBox( GUIRECT, parent, id );

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddCheckBox, "GUIAddCheckBox");
int GM_CDECL gmGUIAddCheckBox( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 8 ); 
    GM_CHECK_INT_PARAM( x1, 0 );
    GM_CHECK_INT_PARAM( y1, 1 );
    GM_CHECK_INT_PARAM( x2, 2 );
    GM_CHECK_INT_PARAM( y2, 3 );
    GM_CHECK_INT_PARAM( owner, 4 );
    GM_CHECK_INT_PARAM( id, 5 );
    GM_CHECK_INT_PARAM( checked, 6 );
    GM_CHECK_STRING_PARAM( caption, 7 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );
    WideString wcaption = caption;

    f32 ratiox = ( f32 )IRR.getScreenWidth() / DEFAULT_SCREEN_WIDTH;
    f32 ratioy = ( f32 )IRR.getScreenHeight() / DEFAULT_SCREEN_HEIGHT;

    IRR.gui->addCheckBox( checked, GUIRECT, parent, id, wcaption.c_str() );

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddMessageBox, "GUIAddMessageBox");
int GM_CDECL gmGUIAddMessageBox( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 2 ); 
    GM_CHECK_STRING_PARAM( caption, 0 );
    GM_CHECK_STRING_PARAM( text, 1 );
    //GM_CHECK_INT_PARAM(owner, 2);
    //GM_CHECK_INT_PARAM(id, 3);
    //GM_CHECK_INT_PARAM(flags, 4);
    //GM_CHECK_INT_PARAM(modal, 5);

    //  IGUIElement *parent = IRR.gui->getRootGUIElement()->getElementFromId(owner, true);
    WideString wcaption = caption;
    WideString wtext = text;

    f32 ratiox = ( f32 )IRR.getScreenWidth() / DEFAULT_SCREEN_WIDTH;
    f32 ratioy = ( f32 )IRR.getScreenHeight() / DEFAULT_SCREEN_HEIGHT;

    //IRR.gui->addMessageBox(wcaption.c_str(), wtext.c_str(), (bool)modal, flags, parent, id);
    IGUIWindow* wnd = IRR.gui->addMessageBox( wcaption.c_str(), wtext.c_str(), false, EMBF_OK, 0, -1 );

    IRR.gui->getRootGUIElement()->bringToFront( wnd );

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddWindow, "GUIAddWindow");
int GM_CDECL gmGUIAddWindow( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 8 ); 
    GM_CHECK_INT_PARAM( x1, 0 );
    GM_CHECK_INT_PARAM( y1, 1 );
    GM_CHECK_INT_PARAM( x2, 2 );
    GM_CHECK_INT_PARAM( y2, 3 );
    GM_CHECK_INT_PARAM( owner, 4 );
    GM_CHECK_INT_PARAM( id, 5 );
    GM_CHECK_INT_PARAM( modal, 6 );
    GM_CHECK_STRING_PARAM( caption, 7 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );
    WideString wcaption = caption;

    f32 ratiox = ( f32 )IRR.getScreenWidth() / DEFAULT_SCREEN_WIDTH;
    f32 ratioy = ( f32 )IRR.getScreenHeight() / DEFAULT_SCREEN_HEIGHT;

    IGUIWindow* wnd = IRR.gui->addWindow( GUIRECT, ( bool )modal, wcaption.c_str(), parent, id );

    IGUIElement* exists = IRR.gui->getRootGUIElement()->getElementFromId( id, true );
    if ( !exists )
    {
      IRR.gui->getRootGUIElement()->bringToFront( wnd );
    }

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddMenu, "GUIAddMenu");
int GM_CDECL gmGUIAddMenu( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 2 ); 
    GM_CHECK_INT_PARAM( owner, 0 );
    GM_CHECK_INT_PARAM( id, 1 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );
    IRR.gui->addMenu( parent, id );

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddMenuItem, "GUIAddMenuItem");
int GM_CDECL gmGUIAddMenuItem( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 4 ); 
    GM_CHECK_INT_PARAM( owner, 0 );
    GM_CHECK_INT_PARAM( id, 1 );
    GM_CHECK_INT_PARAM( hassubmenu, 2 );
    GM_CHECK_STRING_PARAM( caption, 3 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );
    IGUIContextMenu* menu = ( IGUIContextMenu* )parent;
    WideString wcaption = caption;

    menu->addItem( wcaption.c_str(), -1, true, ( bool )hassubmenu );

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddSubmenu, "GUIAddSubmenu");
int GM_CDECL gmGUIAddSubmenu( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 4 ); 
    GM_CHECK_INT_PARAM( owner, 0 );
    GM_CHECK_INT_PARAM( id, 1 );
    GM_CHECK_INT_PARAM( menunum, 2 );
    GM_CHECK_STRING_PARAM( caption, 3 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );
    IGUIContextMenu* menu = ( IGUIContextMenu* )parent;
    WideString wcaption = caption;

    IGUIContextMenu* submenu;
    submenu = menu->getSubMenu( menunum );

    if ( wcaption == L"-" )
    {
      submenu->addSeparator();
    }
    else
    {
      submenu->addItem( wcaption.c_str(), id );
    }

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddEditBox, "GUIAddEditBox");
int GM_CDECL gmGUIAddEditBox( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 8 ); 
    GM_CHECK_INT_PARAM( x1, 0 );
    GM_CHECK_INT_PARAM( y1, 1 );
    GM_CHECK_INT_PARAM( x2, 2 );
    GM_CHECK_INT_PARAM( y2, 3 );
    GM_CHECK_INT_PARAM( owner, 4 );
    GM_CHECK_INT_PARAM( id, 5 );
    GM_CHECK_INT_PARAM( border, 6 );
    GM_CHECK_STRING_PARAM( caption, 7 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );
    WideString wcaption = caption;

    f32 ratiox = ( f32 )IRR.getScreenWidth() / DEFAULT_SCREEN_WIDTH;
    f32 ratioy = ( f32 )IRR.getScreenHeight() / DEFAULT_SCREEN_HEIGHT;

    IRR.gui->addEditBox( wcaption.c_str(), GUIRECT, ( bool )border, parent, id );

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddListBox, "GUIAddListBox");
int GM_CDECL gmGUIAddListBox( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 7 ); 
    GM_CHECK_INT_PARAM( x1, 0 );
    GM_CHECK_INT_PARAM( y1, 1 );
    GM_CHECK_INT_PARAM( x2, 2 );
    GM_CHECK_INT_PARAM( y2, 3 );
    GM_CHECK_INT_PARAM( owner, 4 );
    GM_CHECK_INT_PARAM( id, 5 );
    GM_CHECK_INT_PARAM( background, 6 );


    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );

    f32 ratiox = ( f32 )IRR.getScreenWidth() / DEFAULT_SCREEN_WIDTH;
    f32 ratioy = ( f32 )IRR.getScreenHeight() / DEFAULT_SCREEN_HEIGHT;

    IRR.gui->addListBox( GUIRECT, parent, id, ( bool )background );

    return GM_OK;
}

// SCRIPTBIND(gmGUIAddListBoxItem, "GUIAddListBoxItem");
int GM_CDECL gmGUIAddListBoxItem( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 2 ); 
    GM_CHECK_INT_PARAM( owner, 0 );
    GM_CHECK_STRING_PARAM( caption, 1 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );
    IGUIListBox* listbox = ( IGUIListBox* )parent;
    WideString wcaption = caption;

    listbox->addItem( wcaption.c_str() );

    return GM_OK;
}

// SCRIPTBIND(gmGUITabControl, "GUITabControl");
int GM_CDECL gmGUITabControl( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 8 ); 
    GM_CHECK_INT_PARAM( x1, 0 );
    GM_CHECK_INT_PARAM( y1, 1 );
    GM_CHECK_INT_PARAM( x2, 2 );
    GM_CHECK_INT_PARAM( y2, 3 );
    GM_CHECK_INT_PARAM( owner, 4 );
    GM_CHECK_INT_PARAM( id, 5 );
    GM_CHECK_INT_PARAM( background, 6 );
    GM_CHECK_INT_PARAM( border, 7 );


    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );

    f32 ratiox = ( f32 )IRR.getScreenWidth() / DEFAULT_SCREEN_WIDTH;
    f32 ratioy = ( f32 )IRR.getScreenHeight() / DEFAULT_SCREEN_HEIGHT;

    IRR.gui->addTabControl( GUIRECT, parent, ( bool )background, ( bool )border, id );

    return GM_OK;
}

// SCRIPTBIND(gmGUIFileSelect, "GUIFileSelect");
int GM_CDECL gmGUIFileSelect( gmThread* a_thread )
{
    GM_CHECK_NUM_PARAMS( 4 ); 
    GM_CHECK_INT_PARAM( owner, 0 );
    GM_CHECK_INT_PARAM( id, 1 );
    GM_CHECK_INT_PARAM( modal, 2 );
    GM_CHECK_STRING_PARAM( caption, 3 );

    IGUIElement* parent = IRR.gui->getRootGUIElement()->getElementFromId( owner, true );
    WideString wcaption = caption;

    //   IGUIElement* exists = 0;
    //exists = IRR.gui->getRootGUIElement()->getElementFromId( id, true );
    //if (exists)
    //  exists->remove();

    IRR.gui->addFileOpenDialog( wcaption.c_str(), ( bool )modal, parent, id );

    return GM_OK;
}

//// SCRIPTBIND(gmExecuteMainTasks, "ExecuteMainTasks");
//int GM_CDECL gmExecuteMainTasks( gmThread* a_thread )
//{
//    GAME.AddMainTasks();
//    return GM_OK;
//}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\

void BindScriptGUIFunctions()
{
    // bind GM script functions
    SCRIPTBIND( gmGUIClear, "GUIClear" );
    SCRIPTBIND( gmGUIAddButton, "GUIAddButton" );
    SCRIPTBIND( gmGUIAddStaticText, "GUIAddStaticText" );
    SCRIPTBIND( gmGUIAddComboBox, "GUIAddComboBox" );
    SCRIPTBIND( gmGUIAddCheckBox, "GUIAddCheckBox" );
    SCRIPTBIND( gmGUIAddMessageBox, "GUIAddMessageBox" );
    SCRIPTBIND( gmGUIAddWindow, "GUIAddWindow" );
    SCRIPTBIND( gmGUIAddMenu, "GUIAddMenu" );
    SCRIPTBIND( gmGUIAddMenuItem, "GUIAddMenuItem" );
    SCRIPTBIND( gmGUIAddSubmenu, "GUIAddSubmenu" );
    SCRIPTBIND( gmGUIAddEditBox, "GUIAddEditBox" );
    SCRIPTBIND( gmGUIAddListBox, "GUIAddListBox" );
    SCRIPTBIND( gmGUIAddListBoxItem, "GUIAddListBoxItem" );
    SCRIPTBIND( gmGUITabControl, "GUITabControl" );
    SCRIPTBIND( gmGUIFileSelect, "GUIFileSelect" );

    // SCRIPTBIND( gmExecuteMainTasks, "ExecuteMainTasks" );
}

