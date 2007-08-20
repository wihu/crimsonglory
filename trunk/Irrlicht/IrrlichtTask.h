// IrrlichtRun.h: interface for the CIrrlichtTask class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOUPDATE_H__FB4B263B_4FA7_4700_BB70_EE5CB1768E83__INCLUDED_)
#define AFX_VIDEOUPDATE_H__FB4B263B_4FA7_4700_BB70_EE5CB1768E83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Engine/engine.h"

#define DEFAULT_SCREEN_WIDTH 640
#define DEFAULT_SCREEN_HEIGHT 480

class CDebugRender;
class CGUIRender;
class IC_ChatConsole;

#define IRR CIrrlichtTask::GetSingleton()

class CIrrlichtTask : public irr::IEventReceiver, public ITask, public Singleton<CIrrlichtTask>
{
  public:
    CIrrlichtTask();
    virtual ~CIrrlichtTask();
    AUTO_SIZE;

    virtual void UpdateNow();

    void OnSetConstants( video::IMaterialRendererServices* services );

    IrrlichtDevice* device;
    IVideoDriver* video;
    ISceneManager* smgr;
    IGUIEnvironment* gui;
    IGPUProgrammingServices* gpu;

    // console vars
    int useLighting, useShadows;
    int showFPS;
    wchar_t guikey;

    // GUI stuff
    s32 alpha;
    IGUIFont* guiFont;
    WideString fontName;

    int getScreenWidth()
    {
        return scrWidth;
    }
    int getScreenHeight()
    {
        return scrHeight;
    }

    void QFontDraw( IGUIFont* font, const wchar_t* text, const core::rect<s32>& position, video::SColor color );

    void drawAsyncMessage( const c8* text );

    IC_ChatConsole* getChatConsole()
    {
        return chatConsole;
    }
    bool getInMenu()
    {
        return bShowMenu;
    }

    void clearGUI();

    video::ITexture* mirrorTexture;

  protected:
    // Irrlicht callbacks
    virtual bool OnEvent( irr::SEvent event );

    virtual bool Start();
    virtual void Update();
    virtual void Stop();

    virtual void RenderAll();

    void calcMousePos();

    int driver, scrWidth, scrHeight, scrBPP, scrFullscreen, scrVSync;
    bool UseHighLevelShaders;

    bool bShowMenu, bCanCloseMenu;

    // Renderables
    CMMPointer<CGUIRender> GUIRender;
    CMMPointer<CDebugRender> DebugRender;

    IC_ChatConsole* chatConsole;
};

//////////////////////////////
// CGUIRender
//////////////////////////////

class CGUIRender : public CRenderable
{
  public:
    CGUIRender();
    virtual ~CGUIRender();
    AUTO_SIZE;

    virtual void Render();
};


//////////////////////////////
// CDebugRender
//////////////////////////////

class CDebugRender : public CRenderable
{
  public:
    CDebugRender() : CRenderable( RENDER_ABOVE_GUI )
    {
    }
    virtual ~CDebugRender()
    {
    }
    AUTO_SIZE;

    virtual void Render();
};


#endif // !defined(AFX_VIDEOUPDATE_H__FB4B263B_4FA7_4700_BB70_EE5CB1768E83__INCLUDED_)
