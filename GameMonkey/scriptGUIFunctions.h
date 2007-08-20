#ifndef SCRIPTGUIFUNCTIONS_H_INCLUDED
#define SCRIPTGUIFUNCTIONS_H_INCLUDED

#include "../App/app.h"
#include "scriptFunctions.h"

//#define GUIRECT rect<s32>((s32)x1*ratiox,(s32)y1*ratioy,(s32)x2*ratiox,(s32)y2*ratioy)
#define GUIRECT rect<s32>((s32)x1,(s32)y1,(s32)x2,(s32)y2)

// SCRIPTBIND(gmGUIClear, "GUIClear");
int GM_CDECL gmGUIClear( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddButton, "GUIAddButton");
int GM_CDECL gmGUIAddButton( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddStaticText, "GUIAddStaticText");
int GM_CDECL gmGUIAddStaticText( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddComboBox, "GUIAddComboBox");
// ??
int GM_CDECL gmGUIAddComboBox( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddCheckBox, "GUIAddCheckBox");
int GM_CDECL gmGUIAddCheckBox( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddMessageBox, "GUIAddMessageBox");
int GM_CDECL gmGUIAddMessageBox( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddWindow, "GUIAddWindow");
int GM_CDECL gmGUIAddWindow( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddMenu, "GUIAddMenu");
int GM_CDECL gmGUIAddMenu( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddMenuItem, "GUIAddMenuItem");
int GM_CDECL gmGUIAddMenuItem( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddSubmenu, "GUIAddSubmenu");
int GM_CDECL gmGUIAddSubmenu( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddEditBox, "GUIAddEditBox");
int GM_CDECL gmGUIAddEditBox( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddListBox, "GUIAddListBox");
int GM_CDECL gmGUIAddListBox( gmThread* a_thread );

// SCRIPTBIND(gmGUIAddListBoxItem, "GUIAddListBoxItem");
int GM_CDECL gmGUIAddListBoxItem( gmThread* a_thread );

// SCRIPTBIND(gmGUITabControl, "GUITabControl");
int GM_CDECL gmGUITabControl( gmThread* a_thread );

// SCRIPTBIND(gmExecuteMainTasks, "ExecuteMainTasks");
int GM_CDECL gmExecuteMainTasks( gmThread* a_thread );

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\

extern void BindScriptGUIFunctions();

#endif