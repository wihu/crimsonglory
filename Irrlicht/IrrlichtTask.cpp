// irrlichtTask.cpp: implementation of the CIrrlichtTask class.
//
//////////////////////////////////////////////////////////////////////

#include "IrrlichtTask.h"
#include "../Game/GameDLL.h"
#include "../IrrConsole/console_vars.h"
#include "../Engine/misc.h"

#include "../World/controls.h"
#include "../World/world.h"

#define DEFAULT_BACKGROUND SColor(0,67,1,2)
#define DEFAULT_BACKGROUND_MENU SColor(110,67,1,2)
#define GUIEVENT_FILE "Scripts/guievent.gm"
#define GUIMAIN_FILE "Scripts/gui.gm"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIrrlichtTask::CIrrlichtTask()
{
    myControls.NoKeysPressed();
}

CIrrlichtTask::~CIrrlichtTask()
{
}

bool CIrrlichtTask::Start()
{
    CONSOLE_VAR( "v_width", int, scrWidth, DEFAULT_SCREEN_WIDTH, L"v_width [pixels]. Ex. v_width 640", L"Requires v_restart to take effect." );
    CONSOLE_VAR( "v_height", int, scrHeight, DEFAULT_SCREEN_HEIGHT, L"v_height [pixels]. Ex. v_height 480", L"Requires v_restart to take effect." );
    CONSOLE_VAR( "v_bpp", int, scrBPP, 16, L"v_bpp [16/32]. Ex. v_bpp 32", L"Requires v_restart to take effect." );
    CONSOLE_VAR( "v_fullscreen", int, scrFullscreen, 0, L"v_fullscreen [0/1]. Ex. v_fullscreen 1", L"Requires v_restart to take effect." );
    CONSOLE_VAR( "v_driver", int, driver, 1, L"v_driver [0-4]. Ex. v_driver 1", L"Rendering driver (0 - null, 1 - software, 2 - dx8, 3 - dx9, 4 - ogl).  Requires v_restart to take effect." );
    CONSOLE_VAR( "v_lighting", int, useLighting, 0, L"v_lighting [0-1]. Ex. v_lighting 1", L"Use lighting for the scene or not.  Requires v_restart to take effect." );
    CONSOLE_VAR( "v_shadows", int, useShadows, 0, L"v_shadows [0-1]. Ex. v_shadows 1", L"Use dynamic shadows for the scene or not.  Requires v_restart to take effect." );
    CONSOLE_VAR( "v_vsync", int, scrVSync, 0, L"v_vsync [0/1]. Ex. v_vsync 1", L"Vertical synchronization of video with monitor. Requires v_restart to take effect." );
    CONSOLE_VAR( "v_showfps", int, showFPS, 0, L"v_showfps [real]. Ex. v_showfps 1", L"Show the current FPS on screen." );
    CONSOLE_VAR( "g_guikey", wchar_t, guikey, 0x1B, L"g_guikey [keycode]. Ex. g_guikey 0x1B", L"The key used for accessing the game menu." );


    // init irrlicht device
#ifndef _CLIENT
    driver = EDT_NULL;
#endif

    dimension2d<s32> screenDim( scrWidth, scrHeight );
    device = createDevice( ( E_DRIVER_TYPE )driver, screenDim, scrBPP, ( bool )scrFullscreen,  ( bool )useShadows, scrVSync, this );
    if ( !device )
    {
      CONSOLE.add( "Video initialization failed.", COLOR_ERROR );
      GAME.startupError = 1;
      GAME.Shutdown();
      return false;
    }

    video = device->getVideoDriver();
    smgr = device->getSceneManager();
    gui = device->getGUIEnvironment();
    gpu = video->getGPUProgrammingServices();


    device->setWindowCaption( GAME.buildName.c_str() );

#ifdef _CLIENT
    //shaders
    UseHighLevelShaders = true;

    if ( !video->queryFeature( video::EVDF_PIXEL_SHADER_1_1 ) && !video->queryFeature( video::EVDF_ARB_FRAGMENT_PROGRAM_1 ) )
    {
      device->getLogger()->log( "WARNING: Pixel shaders disabled "\
            "because of missing driver/hardware support." );
    }

    if ( !video->queryFeature( video::EVDF_VERTEX_SHADER_1_1 ) && !video->queryFeature( video::EVDF_ARB_VERTEX_PROGRAM_1 ) )
    {
      device->getLogger()->log( "WARNING: Vertex shaders disabled "\
            "because of missing driver/hardware support." );
    }
    if ( !video->queryFeature( video::EVDF_RENDER_TO_TARGET ) )
    {
      device->getLogger()->log( "WARNING: Render to texture disabled " );
    }

    chatConsole = new IC_ChatConsole();

    // renders
    GUIRender = new CGUIRender();
    DebugRender = new CDebugRender();

#endif

    // vars
    mirrorTexture = 0;
    bShowMenu = false;
    bCanCloseMenu = false;

    return true;
}

void CIrrlichtTask::Update()
{
    PROFILE( "irrlicht task" );
    UpdateNow();
}

void CIrrlichtTask::Stop()
{
#ifdef _CLIENT
    DebugRender->Kill();
    GUIRender->Kill();
#endif
    delete chatConsole;
    chatConsole = 0;
    device->drop();
    device = NULL;
    video = NULL;
    smgr = NULL;
    gui = NULL;
}

void CIrrlichtTask::UpdateNow()
{
#ifdef _CLIENT
    calcMousePos();

    if ( device->isWindowActive() )
    {
      RenderAll();
    }
#endif

    if ( !device->run() )
    {
      GAME.Shutdown();
    }
}

bool CIrrlichtTask::OnEvent( irr::SEvent event )
{
    if ( event.EventType == irr::EET_KEY_INPUT_EVENT )
    {
      //CONSOLE key managing
      if ( event.KeyInput.PressedDown )
      {
        if ( event.KeyInput.Key == CONSOLE.getConfig().key_tilde )
        {
          if ( !CONSOLE.isVisible() )
          {
            CONSOLE.setVisible( true );
            //chatConsole->setVisible( false );
            chatConsole->inPrompt = false;
            return true;
          }
          else if ( !event.KeyInput.Control )
          {
            CONSOLE.setVisible( false );
            //chatConsole->setVisible( true );
            return true;
          }
        }

        if ( CONSOLE.isVisible() )
        {
          CONSOLE.handleKeyPress( event.KeyInput.Char, event.KeyInput.Key, event.KeyInput.Shift, event.KeyInput.Control, device );
          CONSOLE.inPrompt = true;
          return true;
        }

        if ( !CONSOLE.isVisible() )
        {
          if ( event.KeyInput.Key == IRR.getChatConsole()->getConfig().key_tilde )
          {
            if ( !chatConsole->inPrompt )
            {
              chatConsole->inPrompt = true;
              return true;
            }
          }


          if ( ( chatConsole->isVisible() ) && ( chatConsole->inPrompt ) )
          {
            chatConsole->handleKeyPress( event.KeyInput.Char, event.KeyInput.Key, event.KeyInput.Shift, event.KeyInput.Control, device );
            return true;
          }
        }


        if ( event.KeyInput.Key == guikey )
        {
          if ( !bShowMenu )
          {
            SCRIPT.RunFile( GUIMAIN_FILE );
            bShowMenu = true;
            bCanCloseMenu = false;
          }
          else
          {
            bCanCloseMenu = true;
          }

          return true;
        }
      }//if(event.KeyInput.PressedDown)

      if ( !event.KeyInput.PressedDown )
      {
        if ( event.KeyInput.Key == guikey )
        {
          if ( bCanCloseMenu )
          {
            clearGUI();
            bShowMenu = false;
            return true;
          }
        }
      }


      //MYCONTROLS
      if ( !bShowMenu )
      {
        myControls.SetKey( event.KeyInput.Key, event.KeyInput.PressedDown );
      }

      return false;
    }
    else if ( event.EventType == irr::EET_LOG_TEXT_EVENT )
    {
      CONSOLE.logMessage_ANSI( event.LogEvent.Level, event.LogEvent.Text );
      return true;
    }
    else if ( event.EventType == irr::EET_MOUSE_INPUT_EVENT )
    {
      //if (IRR.gui->getRootGUIElement()->getElementFromPoint( myControls.mousePos ))
      //            return true;
      //MYCONTROLS
      if ( !bShowMenu )
      {
        if ( !myControls.isContextMenuActive() )
        {
          switch ( event.MouseInput.Event )
          {
            case EMIE_LMOUSE_PRESSED_DOWN :
              myControls.SetKey( KEY_LBUTTON, true );
              break;
            case EMIE_RMOUSE_PRESSED_DOWN :
              myControls.SetKey( KEY_RBUTTON, true );
              break;
            case EMIE_MMOUSE_PRESSED_DOWN :
              myControls.SetKey( KEY_MBUTTON, true );
              break;
            case EMIE_LMOUSE_LEFT_UP :
              myControls.SetKey( KEY_LBUTTON, false );
              break;
            case EMIE_RMOUSE_LEFT_UP :
              myControls.SetKey( KEY_RBUTTON, false );
              break;
            case EMIE_MMOUSE_LEFT_UP :
              myControls.SetKey( KEY_MBUTTON, false );
              break;
          }
        }
      }
      //MYCONTROLS
      //myControls.RemoveContextMenu();

      return false;
    }
    else 
      //                                                                            +++++++++++ GUI EVENT +++++++++++
    if ( event.EventType == EET_GUI_EVENT )
    {
      s32 id = event.GUIEvent.Caller->getID();

      //CONSOLE.addx( "menu: %i", id );

      if ( id >= 10000 )
      {
        if ( ( event.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED ) )
        {
          // a menu item was clicked
          IGUIContextMenu* menu = ( IGUIContextMenu* )event.GUIEvent.Caller;
          s32 itemid = menu->getItemCommandId( menu->getSelectedItem() );   
          //CONSOLE.addx( "menu: %i", itemid );
          //MYCONTROLS
          myControls.setContextMenuSelection( itemid );
          myControls.setContextMenuSelectionText( menu->getItemText( menu->getSelectedItem() ) );
          return true;
        }
      }

      if ( id < 10000 )
      {
        if ( ( event.GUIEvent.EventType == EGET_BUTTON_CLICKED ) || ( event.GUIEvent.EventType == EGET_SCROLL_BAR_CHANGED ) || ( event.GUIEvent.EventType == EGET_CHECKBOX_CHANGED ) || ( event.GUIEvent.EventType == EGET_LISTBOX_CHANGED ) || ( event.GUIEvent.EventType == EGET_EDITBOX_ENTER ) || ( event.GUIEvent.EventType == EGET_FILE_SELECTED ) || ( event.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED ) || ( event.GUIEvent.EventType == EGET_COMBO_BOX_CHANGED ) )
        {
          if ( event.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED )
          {
            // a menu item was clicked
            IGUIContextMenu* menu = ( IGUIContextMenu* )event.GUIEvent.Caller;
            id = menu->getItemCommandId( menu->getSelectedItem() );
          }
          if ( event.GUIEvent.EventType == EGET_FILE_SELECTED )
          {
            // Irrlicht changes directory with the file box
            changeWorkingDir( APP.appDir + APP.modDir );
            if ( APP.DebugMode )
            {
              CONSOLE.addx( "Changing game working directory to '%s%s'", APP.appDir.c_str(), APP.modDir.c_str() );
            }

            IGUIFileOpenDialog* dialog = ( IGUIFileOpenDialog* )event.GUIEvent.Caller;                  
            WideString wstr = "global GUI_selected_file = `";
            wstr += dialog->getFilename(); wstr += "`;";
            SCRIPT.RunString( wstr );
          }

          // global GUIid = id; run script guievent.gm
          WideString wstr = "global GUI_clickedId = ";
          wstr += id; wstr += ";";
          SCRIPT.RunString( wstr );
          SCRIPT.RunFile( GUIEVENT_FILE );
          return true;
        }
      }
    }
    //                                                                          ----------- GUI EVENT -----------
    return true;
}

void CIrrlichtTask::RenderAll()
{
    video->beginScene( true, true, DEFAULT_BACKGROUND );

    smgr->drawAll();

    CRenderable::RenderAll();

    video->endScene();
}

void CIrrlichtTask::QFontDraw( IGUIFont* font, const wchar_t* text, const core::rect<s32>& position, video::SColor color )
{
}

void CIrrlichtTask::calcMousePos()
{
    if ( bShowMenu )
    {
      return;
    }

    line3d<f32> raytrace; 
    plane3d<f32> plane;
    vector3df p;
    ICameraSceneNode* cam = smgr->getActiveCamera();
    if ( !cam )
    {
      return;
    }

    position2d<s32> pos2d = device->getCursorControl()->getPosition();

    raytrace = smgr->getSceneCollisionManager()->getRayFromScreenCoordinates( device->getCursorControl()->getPosition(), cam );
    plane.setPlane( cam->getTarget(), cam->getTarget() - cam->getAbsolutePosition() );

    plane.getIntersectionWithLimitedLine( raytrace.start, raytrace.end, p );

    //MYCONTROLS
    myControls.mousePos = pos2d;
    myControls.mousePosWorld.X = p.X;
    myControls.mousePosWorld.Y = p.Y;
    myControls.mousePosWorld.Z = 0;
}

void CIrrlichtTask::drawAsyncMessage( const c8* text )
{
    device->run();
    video->beginScene( true, true, DEFAULT_BACKGROUND );

    WideString wstr;
    wstr += "Precaching file: ";
    wstr += text;
    core::dimension2d<s32> dim = gui->getBuiltInFont()->getDimension( wstr.c_str() );

    video->draw2DRectangle( SColor( 255, 255, 255, 255 ), rect<s32>( IRR.getScreenWidth() / 2 - dim.Width, IRR.getScreenHeight() / 2 - dim.Height, IRR.getScreenWidth() / 2 + dim.Width, IRR.getScreenHeight() / 2 + dim.Height ) ) ;

    gui->getBuiltInFont()->draw( wstr.c_str(), core::rect<s32>( 0, 0, IRR.getScreenWidth(), IRR.getScreenHeight() ), irr::video::SColor( 255, 5, 5, 5 ), true, true );

    CRenderable::RenderAll();


    video->endScene();
}

void CIrrlichtTask::clearGUI()
{
    core::list<IGUIElement*>::Iterator it;
    array<IGUIElement*> elements;
    for ( it = IRR.gui->getRootGUIElement()->getChildren().begin(); it != IRR.gui->getRootGUIElement()->getChildren().end(); it++ )
    {
      elements.push_back( ( *it ) );
    }

    for ( int i = 0; i < elements.size(); i++ )
    {
      elements[i]->remove();
    }
}

//////////////////
// CGUIRender //
//////////////////

CGUIRender::CGUIRender() : CRenderable( RENDER_GUI )
{
    // init CONSOLE
    CONSOLE_VAR( "c_dimension_x", float, CONSOLE.getConfig().dimensionRatios.X, 1.0f, L"c_dimension_x [0.0-1.0]. Ex. c_dimension_x 1.0", L"Width of console in percents of screen." );
    CONSOLE_VAR( "c_dimension_y", float, CONSOLE.getConfig().dimensionRatios.Y, 0.9f, L"c_dimension_y [0.0-1.0]. Ex. c_dimension_y 0.66", L"Height of console in percents of screen." );
    CONSOLE_VAR( "c_bgshow", bool, CONSOLE.getConfig().bShowBG, true, L"c_showbg [0/1]. Ex. c_showbg 1", L"Draws background of console." );
    CONSOLE_VAR_S( "c_fontfile", CONSOLE.getConfig().fontName, "Fonts/console.bmp", L"c_fontfile [filename]. Ex. c_fontfile Fonts/console.bmp", L"Sets the font file of the console." );
    CONSOLE_VAR_C( "c_bgcolor", CONSOLE.getConfig().bgColor, irr::video::SColor( 105, 10, 10, 55 ), L"c_bgcolor [0-255] [0-255] [0-255] [0-255]. Ex. c_bgcolor 255 0 0 0", L"Background color of console." );
    CONSOLE_VAR_C( "c_fontcolor", CONSOLE.getConfig().fontColor, irr::video::SColor( 255, 245, 245, 245 ), L"c_fontcolor [0-255] [0-255] [0-255] [0-255]. Ex. c_fontcolor 255 250 250 250", L"Default color of console font." );
    CONSOLE_VAR( "c_linespacing", u32, CONSOLE.getConfig().lineSpacing, 1, L"c_linespacing [pixels]. Ex. c_linespacing 1", L"Space between lines in console." );
    CONSOLE_VAR( "c_indent", u32, CONSOLE.getConfig().indent, 1, L"c_indent [pixels]. Ex. c_indent 1", L"Left indentation of messages in console." );
    CONSOLE_VAR( "c_key", wchar_t, CONSOLE.getConfig().key_tilde, 0xc0, L"c_key [keycode]. Ex. c_key 0xc0", L"The key for opening and closing the console." );
    CONSOLE_VAR_S( "c_prompt", CONSOLE.getConfig().prompt, "console", L"c_prompt [text]. Ex. c_prompt admin", L"Sets the command prompt text." );
    CONSOLE_VAR( "c_history_size", u32, CONSOLE.getConfig().commandHistorySize, 20, L"c_history_size [num]. Ex. c_history_size 10", L"Number of console commands stored in history." );
    CONSOLE_VAR( "c_halign", u32, CONSOLE.getConfig().halign, ( u32 )HAL_RIGHT, L"c_halign [0-2]. Ex. c_halign 2", L"Horizontal alignment of console 0-left, 1-center, 2-right." );
    CONSOLE_VAR( "c_valign", u32, CONSOLE.getConfig().valign, ( u32 )VAL_TOP, L"c_valign [0-2]. Ex. c_valign 2", L"Vertical alignment of console 0-top, 1-middle, 2-bottom." );

    //now initialize
    dimension2d<s32> screenDim( IRR.getScreenWidth(), IRR.getScreenHeight() );
    CONSOLE.initializeConsole( IRR.device->getGUIEnvironment(), screenDim, "../Logs/console.txt", true );

    // set GUI
    CONSOLE_VAR( "u_transparency", s32, IRR.alpha, 255, L"u_transparency [0-255]. Ex. u_transparency 255", L"Transparency of the game interface." );
    CONSOLE_VAR_S( "u_fontfile", IRR.fontName, "Fonts/gui.bmp", L"u_fontfile [filename]. Ex. u_fontfile Fonts/console.bmp", L"Sets the font file of the GUI." );

    gui::IGUISkin* newskin = IRR.gui->createSkin( gui::EGST_WINDOWS_METALLIC );

    for ( s32 i = 0; i < EGDC_COUNT ; ++i )
    {
      SColor col = newskin->getColor( ( EGUI_DEFAULT_COLOR )i );
      col.setAlpha( IRR.alpha );
      newskin->setColor( ( EGUI_DEFAULT_COLOR )i, col );
    }

    //load GUI font
    IRR.guiFont = IRR.gui->getFont( APP.useFile( wide2string( IRR.fontName ).c_str() ).c_str() );
    if ( IRR.guiFont == 0 )
    {
      IRR.guiFont = IRR.gui->getBuiltInFont();
      //CONSOLE.addx( COLOR_ERROR, "Could not load GUI font '%s'",  wide2string( fontName ).c_str() );
    }
    newskin->setFont( IRR.guiFont );

    IRR.gui->setSkin( newskin );
    newskin->drop();

    // CHAT

    // init CONSOLE
    CONSOLE_VAR( "cc_dimension_x", float, IRR.getChatConsole()->getConfig().dimensionRatios.X, 0.4f, L"cc_dimension_x [0.0-1.0]. Ex. cc_dimension_x 1.0", L"Width of console in percents of screen." );
    CONSOLE_VAR( "cc_dimension_y", float, IRR.getChatConsole()->getConfig().dimensionRatios.Y, 0.2f, L"cc_dimension_y [0.0-1.0]. Ex. cc_dimension_y 0.66", L"Height of console in percents of screen." );
    CONSOLE_VAR( "cc_bgshow", bool, IRR.getChatConsole()->getConfig().bShowBG, true, L"cc_showbg [0/1]. Ex. cc_showbg 1", L"Draws background of console." );
    CONSOLE_VAR_S( "cc_fontfile", IRR.getChatConsole()->getConfig().fontName, "Fonts/console.bmp", L"cc_fontfile [filename]. Ex. cc_fontfile Fonts/console.bmp", L"Sets the font file of the console." );
    CONSOLE_VAR_C( "cc_bgcolor", IRR.getChatConsole()->getConfig().bgColor, irr::video::SColor( 65, 10, 10, 55 ), L"cc_bgcolor [0-255] [0-255] [0-255] [0-255]. Ex. cc_bgcolor 255 0 0 0", L"Background color of console." );
    CONSOLE_VAR_C( "cc_fontcolor", IRR.getChatConsole()->getConfig().fontColor, irr::video::SColor( 255, 245, 245, 245 ), L"cc_fontcolor [0-255] [0-255] [0-255] [0-255]. Ex. cc_fontcolor 255 250 250 250", L"Default color of console font." );
    CONSOLE_VAR( "cc_linespacing", u32, IRR.getChatConsole()->getConfig().lineSpacing, 1, L"cc_linespacing [pixels]. Ex. cc_linespacing 1", L"Space between lines in console." );
    CONSOLE_VAR( "cc_indent", u32, IRR.getChatConsole()->getConfig().indent, 8, L"cc_indent [pixels]. Ex. cc_indent 1", L"Left indentation of messages in console." );
    CONSOLE_VAR( "cc_key", wchar_t, IRR.getChatConsole()->getConfig().key_tilde, 0x54, L"cc_key [keycode]. Ex. cc_key 0xc0", L"The key for opening and closing the console." );
    CONSOLE_VAR_S( "cc_prompt", IRR.getChatConsole()->getConfig().prompt, "console", L"cc_prompt [text]. Ex. cc_prompt admin", L"Sets the command prompt text." );
    CONSOLE_VAR( "cc_history_size", u32, IRR.getChatConsole()->getConfig().commandHistorySize, 20, L"cc_history_size [num]. Ex. cc_history_size 10", L"Number of console commands stored in history." );
    CONSOLE_VAR( "cc_halign", u32, IRR.getChatConsole()->getConfig().halign, ( u32 )HAL_RIGHT, L"cc_halign [0-2]. Ex. cc_halign 2", L"Horizontal alignment of console 0-left, 1-center, 2-right." );
    CONSOLE_VAR( "cc_valign", u32, IRR.getChatConsole()->getConfig().valign, ( u32 )VAL_TOP, L"cc_valign [0-2]. Ex. cc_valign 2", L"Vertical alignment of console 0-top, 1-middle, 2-bottom." );

    IRR.getChatConsole()->setVisible( true );
    IRR.getChatConsole()->initializeConsole( IRR.device->getGUIEnvironment(), screenDim, "../Logs/chat.txt", false );
    IRR.getChatConsole()->inPrompt = false;
}

CGUIRender::~CGUIRender()
{
}

void CGUIRender::Render()
{
    if ( IRR.getInMenu() )
    {
      IRR.video->draw2DRectangle( DEFAULT_BACKGROUND_MENU, rect<s32>( 0, 0, IRR.getScreenWidth(), IRR.getScreenHeight() ) );
    }

    IRR.gui->drawAll();
    IRR.getChatConsole()->renderConsole( IRR.gui, IRR.video, 0 );
    CONSOLE.renderConsole( IRR.gui, IRR.video, 0 );
}

//////////////////
// CDebugRender //
//////////////////

void CDebugRender::Render()
{
    // show FPS
    if ( IRR.showFPS )
    {
      WideString str = "FPS: ";
      str += IRR.video->getFPS();
      IRR.gui->getBuiltInFont()->draw( str.c_str(), core::rect<s32>( 5, IRR.getScreenHeight() - 30, IRR.getScreenWidth(), IRR.getScreenHeight() ), irr::video::SColor( 255, 255, 255, 255 ), false, true );
    }

    if ( !APP.DebugMode )
    {
      return;
    }

    //draw mouse box
    f32 radius = 0.1f;
    IRR.video->draw3DBox( aabbox3df( vector3df( myControls.mousePosWorld.X - radius, myControls.mousePosWorld.Y - radius, 0 - radius ), vector3df( myControls.mousePosWorld.X + radius, myControls.mousePosWorld.Y + radius, 0 + radius ) ), SColor( 0, 210, 225, 249 ) );

    // show buildname and FPS
    WideString str = GAME.buildName;
    str += " [";
    str += IRR.video->getName();
    str += "]";
    // str += IRR.video->getFPS();
    IRR.gui->getBuiltInFont()->draw( str.c_str(), core::rect<s32>( 5, IRR.getScreenHeight() - 12, IRR.getScreenWidth(), IRR.getScreenHeight() ), irr::video::SColor( 255, 255, 255, 255 ), false, true );
}

