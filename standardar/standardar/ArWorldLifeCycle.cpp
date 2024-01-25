#include "ArWorldLifeCycle.h"

using namespace standardar;

#define LIFECYCLE_WARN LOGW("Warn:Life from %s to %s", m_State.c_str(), __FUNCTION__)
#define LIFECYCLE_DEBUG LOGD("Debug:Life from %s to %s", m_State.c_str(), __FUNCTION__)

ArWorldLifeCycle::ArWorldLifeCycle(CSession* session)
{
    m_Session = session;
    m_OnCreateState = new OnCreateState(this);
    m_OnResumeState = new OnResumeState(this);
    m_OnPauseState = new OnPauseState(this);
    m_OnStartState = new OnStartState(this);
    m_OnStopState = new OnStopState(this);
    m_OnDestroyState = new OnDestroyState(this);
    m_LifeState = m_OnCreateState;
}

int ArWorldLifeCycle::onCreate()
{
    return m_LifeState->onCreate();
}

int ArWorldLifeCycle::onResume()
{
    return m_LifeState->onResume();
}

int ArWorldLifeCycle::onPause()
{
    return m_LifeState->onPause();
}

int ArWorldLifeCycle::onStart()
{
    return m_LifeState->onStart();
}

int ArWorldLifeCycle::onStop()
{
    return m_LifeState->onStop();
}

int ArWorldLifeCycle::onDestroy()
{
    return m_LifeState->onDestroy();
}

CSession* ArWorldLifeCycle::getSession()
{
    return m_Session;
}

void ArWorldLifeCycle::setState(standardar::IArWorldLifeState *state)
{
    m_LifeState = state;
}

/// OnCreateState Implementation

OnCreateState::OnCreateState(standardar::ArWorldLifeCycle *lifeCycle)
{
    m_LifeCycle = lifeCycle;
    m_State = "onCreate";
}

int OnCreateState::onCreate()
{
    LIFECYCLE_DEBUG;
    return 0;
}

int OnCreateState::onResume()
{
    LIFECYCLE_DEBUG;
    CSession* session = m_LifeCycle->getSession();
    int res = session->resume();
    m_LifeCycle->setState(m_LifeCycle->m_OnResumeState);
    return res;
}

int OnCreateState::onPause()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnCreateState::onStart()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnCreateState::onStop()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnCreateState::onDestroy()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

/// OnResumeState Implementation

OnResumeState::OnResumeState(standardar::ArWorldLifeCycle *lifeCycle)
{
    m_LifeCycle = lifeCycle;
    m_State = "onResume";
}

int OnResumeState::onCreate()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnResumeState::onResume()
{
    LIFECYCLE_DEBUG;
    return ARRESULT_SUCCESS;
}

int OnResumeState::onPause()
{
    LIFECYCLE_DEBUG;
    int res = m_LifeCycle->getSession()->pause();
    m_LifeCycle->setState(m_LifeCycle->m_OnPauseState);
    return res;
}

int OnResumeState::onStart()
{
    LIFECYCLE_DEBUG;
    int res = m_LifeCycle->getSession()->startAlgorithm();
    m_LifeCycle->setState(m_LifeCycle->m_OnStartState);
    return res;
}

int OnResumeState::onStop()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnResumeState::onDestroy()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

/// OnPauseState Implementation

OnPauseState::OnPauseState(standardar::ArWorldLifeCycle *lifeCycle)
{
    m_LifeCycle = lifeCycle;
    m_State = "onPause";
}

int OnPauseState::onCreate()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnPauseState::onResume()
{
    LIFECYCLE_DEBUG;
    int res = m_LifeCycle->getSession()->resume();
    m_LifeCycle->setState(m_LifeCycle->m_OnResumeState);
    return res;
}

int OnPauseState::onPause()
{
    LIFECYCLE_DEBUG;
    return ARRESULT_SUCCESS;
}

int OnPauseState::onStart()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnPauseState::onStop()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnPauseState::onDestroy()
{
    LIFECYCLE_DEBUG;
    m_LifeCycle->getSession()->stopAlgorithm();
    m_LifeCycle->getSession()->destory();
    m_LifeCycle->setState(m_LifeCycle->m_OnDestroyState);
    return ARRESULT_SUCCESS;
}

/// OnStartState Implementation

OnStartState::OnStartState(standardar::ArWorldLifeCycle *lifeCycle)
{
    m_LifeCycle = lifeCycle;
    m_State = "onStart";
}

int OnStartState::onCreate()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnStartState::onResume()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnStartState::onPause()
{
    LIFECYCLE_DEBUG;
    int res = m_LifeCycle->getSession()->pause();
    m_LifeCycle->setState(m_LifeCycle->m_OnPauseState);
    return res;
}

int OnStartState::onStart()
{
    LIFECYCLE_DEBUG;
    return ARRESULT_SUCCESS;
}

int OnStartState::onStop()
{
    LIFECYCLE_DEBUG;
    int res = m_LifeCycle->getSession()->stopAlgorithm();
    m_LifeCycle->setState(m_LifeCycle->m_OnStopState);
    return res;
}

int OnStartState::onDestroy()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

/// OnStopState Implementation

OnStopState::OnStopState(standardar::ArWorldLifeCycle *lifeCycle)
{
    m_LifeCycle = lifeCycle;
    m_State = "onStop";
}

int OnStopState::onCreate()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnStopState::onResume()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnStopState::onPause()
{
    LIFECYCLE_DEBUG;
    int res = m_LifeCycle->getSession()->pause();
    m_LifeCycle->setState(m_LifeCycle->m_OnPauseState);
    return res;
}

int OnStopState::onStart()
{
    LIFECYCLE_DEBUG;
    int res = m_LifeCycle->getSession()->startAlgorithm();
    m_LifeCycle->setState(m_LifeCycle->m_OnStartState);
    return res;
}

int OnStopState::onStop()
{
    return ARRESULT_SUCCESS;
}

int OnStopState::onDestroy()
{
    LIFECYCLE_DEBUG;
    m_LifeCycle->getSession()->pause();
    m_LifeCycle->getSession()->destory();
    m_LifeCycle->setState(m_LifeCycle->m_OnDestroyState);
    return ARRESULT_SUCCESS;
}

/// OnDestroyState Implementation

OnDestroyState::OnDestroyState(standardar::ArWorldLifeCycle *lifeCycle)
{
    m_LifeCycle = lifeCycle;
    m_State = "onState";
}

int OnDestroyState::onCreate()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnDestroyState::onResume()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnDestroyState::onStart()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnDestroyState::onPause()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnDestroyState::onStop()
{
    LIFECYCLE_WARN;
    return ARRESULT_ERROR_FATAL;
}

int OnDestroyState::onDestroy()
{
    LIFECYCLE_DEBUG;
    return ARRESULT_SUCCESS;
}
