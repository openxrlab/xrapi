#ifndef STANDARD_AR_ARWORLDLIFECYCLE_H
#define STANDARD_AR_ARWORLDLIFECYCLE_H

#include "ArSession.h"

#define LIFE_STATE_DECLARE(name)                  \
class On##name##State : public IArWorldLifeState  \
{                                                 \
public:                                           \
    On##name##State(ArWorldLifeCycle* lifeCycle); \
    virtual int onCreate();                       \
    virtual int onResume();                       \
    virtual int onPause();                        \
    virtual int onStart();                        \
    virtual int onStop();                         \
    virtual int onDestroy();                      \
};

namespace standardar
{
    class ArWorldLifeCycle;
    class IArWorldLifeState
    {
    public:
        virtual int onCreate() = 0;
        virtual int onResume() = 0;
        virtual int onPause() = 0;
        virtual int onStart() = 0;
        virtual int onStop() = 0;
        virtual int onDestroy() = 0;
    public:
        ArWorldLifeCycle* m_LifeCycle;
        std::string m_State;
    };

    LIFE_STATE_DECLARE(Create)
    LIFE_STATE_DECLARE(Resume)
    LIFE_STATE_DECLARE(Pause)
    LIFE_STATE_DECLARE(Start)
    LIFE_STATE_DECLARE(Stop)
    LIFE_STATE_DECLARE(Destroy)

    class ArWorldLifeCycle
    {
    public:
        ArWorldLifeCycle(CSession* session);
        int onCreate();
        int onResume();
        int onPause();
        int onStart();
        int onStop();
        int onDestroy();
        void setState(IArWorldLifeState* state);
        CSession* getSession();
    public:
        OnCreateState* m_OnCreateState;
        OnResumeState* m_OnResumeState;
        OnPauseState* m_OnPauseState;
        OnStartState* m_OnStartState;
        OnStopState* m_OnStopState;
        OnDestroyState* m_OnDestroyState;
    private:
        CSession* m_Session;
        IArWorldLifeState* m_LifeState;
    };
}
#endif //STANDARD_AR_ARWORLDLIFECYCLE_H
