#ifndef HELLO_HANDGESTURE_C_SRC_HANDGESTUREALGORITHMRESULT_H
#define HELLO_HANDGESTURE_C_SRC_HANDGESTUREALGORITHMRESULT_H

#include "AlgorithmResult.h"
#include "ArCommon.h"
#include <pthread.h>

namespace standardar {
    class HandGestureAlgorithmResult : public AlgorithmResult {
    public:
        HandGestureAlgorithmResult();
        virtual void updateResult(jbyte *result_ptr, int pos, int length, int order, int video_width,int video_height,int screen_rotate);

        virtual void update();
        virtual void cleanResult();

        virtual ~HandGestureAlgorithmResult();

        int32_t UnserializeToCvHandTrackingResult(const char* pbuffer, int bufsize,std::vector<CvHandTrackingResult>& result);

        void cvHandTrackingResultToCHandGesture(CvHandTrackingResult &cvHandTrackingResult, CHandGesture &out_CHandGesture);

        bool resultIsEmpty();

        void getResult(std::vector<ITrackable*>& trackalbe_array);


        std::vector<int> getAllHandGestureId();

        int32_t getHandSide(int32_t handId);
        int32_t getHandGestureType(int32_t handId);
        int32_t getHandToward(int32_t handId);
        float_t getHandTypeConfidence(int32_t handId);
        int32_t getLandMark2DCount(int32_t handId);
        void  getLandMark2DArray(int32_t handId,float *out_landMark2DArray);


    public:
        pthread_mutex_t m_Mutex;
        char*       m_CvHandTrackingResultBuff;
        int32_t     m_CvHandTrackingResultSize;

        int m_ScreenRotate;
        int m_VideoHeight;
        int m_VideoWidth;
        std::vector<CvHandTrackingResult> m_cvHandTrackingResult_vector;
        std::vector<ITrackable*> m_CHandGesture_vector;

        std::map<int32_t , ITrackable*>    m_HandGestures;

        // CHandGesture m_CHandGesture;
    };
}


#endif //HELLO_HANDGESTURE_C_SRC_HANDGESTUREALGORITHMRESULT_H
