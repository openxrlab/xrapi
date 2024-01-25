#ifndef STANDARDAR_AAR_BODYDETECTALGORITHMRESULT_H
#define STANDARDAR_AAR_BODYDETECTALGORITHMRESULT_H

#include <pthread.h>
#include "AlgorithmResult.h"
#include "ArCamera.h"
#include "ArConfig.h"
#include <map>

namespace standardar {
    class CBodyDetectNode;
    class BodyDetectAlgorithmResult : public AlgorithmResult {
    public:
        BodyDetectAlgorithmResult();
        virtual void updateResult(jbyte *result_ptr, int pos, int length, int order);
        virtual void update();
        virtual void cleanResult();
        virtual ~BodyDetectAlgorithmResult();
        void clearBodyMap();

        bool resultIsEmpty();

        void getResult(std::vector<ITrackable*>& trackalbe_array);

        std::vector<int> getAllBodyId();

        int getSkeletonPoint2dCount(int32_t body_id);

        void getSkeletonPoint2d(int32_t body_id,float *out_point2d);
        void getSkeletonPoint2dConfidence(int32_t body_id,float *out_point2d_confidence);

    private:
        pthread_mutex_t m_Mutex;


        std::map<int32_t,CBodyDetectNode *> m_bodys;
    };


}
#endif //STANDARDAR_AAR_BODYDETECTALGORITHMRESULT_H
