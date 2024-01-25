#ifndef AUGMENTED_IMAGE_C_SRC_MARKERALGORITHMRESULT_H
#define AUGMENTED_IMAGE_C_SRC_MARKERALGORITHMRESULT_H

#include <pthread.h>
#include "AlgorithmResult.h"
#include "ArCamera.h"
#include "ArConfig.h"
#include "ArImageNodeMgr.h"

namespace standardar {
    class MarkerAlgorithmResult : public AlgorithmResult {
    public:
        MarkerAlgorithmResult();
        virtual ~MarkerAlgorithmResult();
        virtual void updateResult(jbyte *result_ptr, int pos, int length, int order){};
        virtual void update(){};
        virtual void cleanResult(){};
        void setReferenceImageDatabase(CImageNodeMgr* database);
        void sendReferenceImageDatabase();
        void getReferenceImageDatabase(CImageNodeMgr* database);
        CImageNodeMgr* getReferenceImageDatabase();

        void getCenterPose(int32_t marker_id, int screen_rotate, float* viewArray, CPose& out_pose);
        void getCorners2d(int32_t marker_id, float* corners_2d);
        float getExtentX(int32_t marker_id);
        float getExtentY(int32_t marker_id);
        float getExtentZ(int32_t marker_id);
        std::vector<int> getAllMarkerId();
        friend class MarkerAlgorithmResultWrapper;
    private:
        pthread_mutex_t m_Mutex;

        CImageNodeMgr m_AugmentedImageDatabase;
    };

    class MarkerAlgorithmResultWrapper
    {
    public:
        MarkerAlgorithmResultWrapper(MarkerAlgorithmResult* result) :
                m_marker_algorithm_result(nullptr)
        {
            if (result)
            {
                pthread_mutex_lock(&(result->m_Mutex));
                m_marker_algorithm_result = result;
            }
        }
        ~MarkerAlgorithmResultWrapper()
        {
            if (m_marker_algorithm_result)
                pthread_mutex_unlock(&(m_marker_algorithm_result->m_Mutex));
        }

        MarkerAlgorithmResult* getResult()
        {
            return m_marker_algorithm_result;
        }

    private:
        MarkerAlgorithmResult* m_marker_algorithm_result;

    };
}
#endif //AUGMENTED_IMAGE_C_SRC_MARKERALGORITHMRESULT_H
