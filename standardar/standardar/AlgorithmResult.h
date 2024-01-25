#ifndef STANDARDAR_AAR_ALGORITHMRESULT_H
#define STANDARDAR_AAR_ALGORITHMRESULT_H

#include <jni.h>
#include <pthread.h>
#include <map>
#include "StandardAR.h"
#include "ArCommon.h"
#include "ArConfig.h"
#include "ArJniHelpler.h"
#include "DynamicLoader.h"
namespace standardar {
    class AlgorithmResult {
    public:
        AlgorithmResult():m_ReusltPtr(nullptr),
                            m_bStart(false),
                            m_AlgorithmCore(nullptr){}
        virtual ~AlgorithmResult(){
            JNIEnvProxy env(DynamicLoader::s_JVM);
            if (m_AlgorithmCore)
                env->DeleteGlobalRef(m_AlgorithmCore);
        }
        virtual void cleanResult() = 0;
        /**
         * call in CSession::update()
         */
        virtual void update() = 0;
        virtual bool getState(){ return m_bStart;}
        virtual void setState( bool state){m_bStart = state;}
        virtual void setAlgorithmCore(jobject core) {
            if (core == nullptr)
                return;
            JNIEnvProxy env(DynamicLoader::s_JVM);
            m_AlgorithmCore = env->NewGlobalRef(core);
        }
    protected:
        void* m_ReusltPtr;
        bool  m_bStart;
        jobject m_AlgorithmCore;
    };


    class AlgorithmResultPack
    {
    public:
        AlgorithmResultPack()
        {
            m_ResultMutex = AR_PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
        }

        void updateAlgorithmResult()
        {
            ScopeMutex m(&m_ResultMutex);
            for (std::map<int, AlgorithmResult*>::iterator iter = m_AlgorithmResultMap.begin();
                 iter != m_AlgorithmResultMap.end(); ++iter)
            {
                AlgorithmResult* result = iter->second;
                if (result && result->getState())
                    result->update();
            }
        }

        void deleteAlgorithmResult()
        {
            ScopeMutex m(&m_ResultMutex);
            for (std::map<int, AlgorithmResult*>::iterator iter = m_AlgorithmResultMap.begin();
                 iter != m_AlgorithmResultMap.end(); ++iter)
            {
                AlgorithmResult* result = iter->second;
                if (result)
                    delete result;
            }
            m_AlgorithmResultMap.clear();
        }

        void clearAlgorithmResult()
        {
            ScopeMutex m(&m_ResultMutex);
            for (std::map<int, AlgorithmResult*>::iterator iter = m_AlgorithmResultMap.begin();
                 iter != m_AlgorithmResultMap.end(); ++iter)
            {
                AlgorithmResult* result = iter->second;
                if (result)
                    result->cleanResult();
            }
        }


        void addAlgorithmResult(int type, AlgorithmResult* algorithmResult)
        {
            ScopeMutex m(&m_ResultMutex);
            AlgorithmResult* result = m_AlgorithmResultMap[type];
            if (result)
                delete result;
            m_AlgorithmResultMap[type] = algorithmResult;
        }

        AlgorithmResult* getAlgorithmResult(int type)
        {
            return m_AlgorithmResultMap[type];
        }

        friend class AlgorithmResultWrapper;
    private:
        pthread_mutex_t m_ResultMutex;
        std::map<int, AlgorithmResult*> m_AlgorithmResultMap;

    };

    class AlgorithmResultWrapper {
    public:
        AlgorithmResultWrapper(AlgorithmResultPack* pack, ARAlgorithmType type) :
                                    m_result(nullptr), m_AlgorithmResultPack(nullptr){
            if (pack)
            {
                pthread_mutex_lock(&(pack->m_ResultMutex));
                m_result = pack->m_AlgorithmResultMap[type];
                m_AlgorithmResultPack = pack;
            }
        }
        ~AlgorithmResultWrapper()
        {
            if (m_AlgorithmResultPack)
                pthread_mutex_unlock(&(m_AlgorithmResultPack->m_ResultMutex));
        }
    public:
        template <typename T>
        T getResult()
        {
            return (T)m_result;
        }
    protected:
        AlgorithmResult* m_result;
        AlgorithmResultPack* m_AlgorithmResultPack;
    };
}



#endif //STANDARDAR_AAR_ALGORIHMRESULT_H
