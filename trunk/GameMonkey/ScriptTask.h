#ifndef SCRIPTTASK_H_INCLUDED
#define SCRIPTTASK_H_INCLUDED

class CScriptTask : public ITask
{
  public:
    CScriptTask()
    {
    }
    virtual ~CScriptTask()
    {
    }
    AUTO_SIZE;

    bool Start()
    {
        return true;
    }

    void Update()
    {
        PROFILE( "Script task" );
        SCRIPT.Run();
    }

    void Stop()
    {
    }

  protected:

  private:
};

#endif