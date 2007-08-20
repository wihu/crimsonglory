#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#define RENDER_UNDER_GUI 10
#define RENDER_GUI 50
#define RENDER_ABOVE_GUI 100

class CRenderable : public IMMObject
{
  public:
    CRenderable( int renderLevel );
    virtual ~CRenderable();

    virtual void Render() = 0;
    virtual void Kill();

    static void RenderAll();

    int getRenderLevel()
    {
        return level;
    }
    int getVisible()
    {
        return visible;
    }
    void setVisible( bool v )
    {
        visible = v;
    }

  protected:
    static std::list<CMMPointer<CRenderable> > renderables;

  private:
    int level;
    bool visible;
};

#endif
