#ifndef OBJECT_C_SRC_OBJECTTRACKING_ALGORITHMRESULT_H
#define OBJECT_C_SRC_OBJECTTRACKING_ALGORITHMRESULT_H

#include <pthread.h>
#include <SLAMCommon.h>
#include "AlgorithmResult.h"
#include "ArCamera.h"
#include "ArConfig.h"
#include "ArReferenceObjectDatabase.h"

namespace standardar {
    class ObjectTrackingAlgorithmResult : public AlgorithmResult {
    public:
        ObjectTrackingAlgorithmResult();
        virtual void update(){};
        virtual void cleanResult(){};
        virtual ~ObjectTrackingAlgorithmResult(){};
        void setReferenceObjectDatabase(CReferenceObjectDatabase* database);
        void sendReferenceObjectDatabase();
        void getReferenceObjectDatabase(CReferenceObjectDatabase* database);
        CReferenceObjectDatabase* getReferenceObjectDatabase();
        std::string getObjectTrackingJsonInfo();

        void getCameraViewMatrix(float* view, int screen_rotate);
        void getCenterPose(int32_t object_id, CPose& out_pose);
        float getExtentX(int32_t object_id);
        float getExtentY(int32_t object_id);
        float getExtentZ(int32_t object_id);
        void getBoundingBox(int32_t object_id, float* boundingBox);
        std::vector<int> getAllObjectId();
        friend class ObjectTrackingAlgorithmResultWrapper;
    private:
        pthread_mutex_t m_Mutex;
        std::string m_ObjectTrackingJsonStr;
        CReferenceObjectDatabase m_ReferenceObjectDatabase;
    };

    class ObjectTrackingAlgorithmResultWrapper
    {
    public:
        ObjectTrackingAlgorithmResultWrapper(ObjectTrackingAlgorithmResult* result) :
                m_ObjectTracking_algorithm_result(nullptr)
        {
            if (result)
            {
                pthread_mutex_lock(&(result->m_Mutex));
                m_ObjectTracking_algorithm_result = result;
            }
        }
        ~ObjectTrackingAlgorithmResultWrapper()
        {
            if (m_ObjectTracking_algorithm_result)
                pthread_mutex_unlock(&(m_ObjectTracking_algorithm_result->m_Mutex));
        }

        ObjectTrackingAlgorithmResult* getResult()
        {
            return m_ObjectTracking_algorithm_result;
        }

    private:
        ObjectTrackingAlgorithmResult* m_ObjectTracking_algorithm_result;

    };
}
#endif //OBJECT_C_SRC_OBJECTTRACKING_ALGORITHMRESULT_H
