#include "HandGestureAlgorithmResult.h"
#include "ArHandGesture.h"

using namespace standardar;


HandGestureAlgorithmResult::HandGestureAlgorithmResult():m_CvHandTrackingResultBuff(nullptr),m_CvHandTrackingResultSize(0){
    m_Mutex =AR_PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
    m_ScreenRotate =0;
    m_VideoWidth =0;
    m_VideoHeight =0;
};

void HandGestureAlgorithmResult::updateResult(jbyte *result_ptr, int pos, int length, int order,int video_width,int video_height,int screen_rotate){
    ScopeMutex m(&m_Mutex);
    DirectByteBufferWrapper result_buffer(result_ptr + pos, length, (BUFFER_ORDER)order);
    m_VideoWidth = video_width;
    m_VideoHeight = video_height;
    m_ScreenRotate = screen_rotate;

    m_cvHandTrackingResult_vector.clear();
    m_CHandGesture_vector.clear();

    if( length == 0) {
        StandardAR_SAFE_DELETE(m_CvHandTrackingResultBuff);
        m_CvHandTrackingResultSize = 0;
        return;
    }

    if(m_CvHandTrackingResultBuff == NULL && (length != 0))
    {
        m_CvHandTrackingResultBuff = new char[length];
        m_CvHandTrackingResultSize = length;
    }

    if((m_CvHandTrackingResultSize != length) && (length != 0))
    {
        StandardAR_SAFE_DELETE(m_CvHandTrackingResultBuff);
        m_CvHandTrackingResultBuff = new char[length];
        m_CvHandTrackingResultSize = length;
    }

    result_buffer.get(m_CvHandTrackingResultBuff,length);

};


void HandGestureAlgorithmResult::update() {

    ScopeMutex m(&m_Mutex);

   std::vector<int> handIds = getAllHandGestureId();

   int size = handIds.size();

   if(size>0){


    for (int i = 0; i < size; ++i) {
        int hand_id = handIds[i];
        auto pos = m_HandGestures.find(hand_id);
        if(pos==m_HandGestures.end())
        {
            m_HandGestures.clear();
            CHandGesture* psHandGesture = new CHandGesture(hand_id);
          //  psHandGesture->m_isUpdated = is_updated;
            m_HandGestures.insert(std::make_pair(hand_id, psHandGesture));
            LOGI("Insert a HandGesture:%i", hand_id);
        }
    }
   } else{
       m_HandGestures.clear();
   }

};

void HandGestureAlgorithmResult::cleanResult() {
    ScopeMutex m(&m_Mutex);
    if (m_CvHandTrackingResultBuff != nullptr)
    {
        delete[] m_CvHandTrackingResultBuff;
        m_CvHandTrackingResultBuff = nullptr;
        m_CvHandTrackingResultSize = 0;
        m_cvHandTrackingResult_vector.clear();
        m_CHandGesture_vector.clear();
    }
};

HandGestureAlgorithmResult::~HandGestureAlgorithmResult() {
    cleanResult();
};

int32_t UnserializeToCvHandTrackingResultVersion1(const char* pbuffer,int bufsize, int itemOffset,int itemCount,std::vector<CvHandTrackingResult>& result)
{
    const char *ptempbuf = pbuffer;
    int offset = itemOffset;
    int count = itemCount;
    int i;
    for(i = 0;i < count; i++) {
        CvHandTrackingResult handResult;
        memset(&handResult, 0, sizeof(CvHandTrackingResult));

        memcpy(&(handResult.id), ptempbuf+offset, sizeof(handResult.id));
        offset += sizeof(handResult.id);

        memcpy(&(handResult.handType), ptempbuf+offset, sizeof(handResult.handType));
        offset += sizeof(handResult.handType);

        //rect
        memcpy(&(handResult.handRect.left), ptempbuf+offset, sizeof(handResult.handRect.left));
        offset += sizeof(handResult.handRect.left);
        memcpy(&(handResult.handRect.top), ptempbuf+offset, sizeof(handResult.handRect.top));
        offset += sizeof(handResult.handRect.top);
        memcpy(&(handResult.handRect.right), ptempbuf+offset, sizeof(handResult.handRect.right));
        offset += sizeof(handResult.handRect.right);
        memcpy(&(handResult.handRect.bottom), ptempbuf+offset, sizeof(handResult.handRect.bottom));
        offset += sizeof(handResult.handRect.bottom);

        memcpy(&(handResult.score), ptempbuf+offset, sizeof(handResult.score));
        offset += sizeof(handResult.score);


        memcpy(&(handResult.handSide), ptempbuf+offset, sizeof(handResult.handSide));
        offset += sizeof(handResult.handSide);

        memcpy(&(handResult.handToward), ptempbuf+offset, sizeof(handResult.handToward));
        offset += sizeof(handResult.handToward);

        //palmNormal
        int32_t palmNormal;
        memcpy(&(palmNormal), ptempbuf+offset, sizeof(palmNormal));
        offset += sizeof(palmNormal);
        if(palmNormal > 0)
        {
            Point3F point3F;
            memcpy(&(point3F.x), ptempbuf+offset, sizeof(point3F.x));
            offset += sizeof(point3F.x);
            memcpy(&(point3F.y), ptempbuf+offset, sizeof(point3F.y));
            offset += sizeof(point3F.y);
            memcpy(&(point3F.z), ptempbuf+offset, sizeof(point3F.z));
            offset += sizeof(point3F.z);
            handResult.palmNormal = point3F;
        }

        //palmCenter
        int32_t palmCenter;
        memcpy(&(palmCenter), ptempbuf+offset, sizeof(palmCenter));
        offset += sizeof(palmCenter);
        if(palmCenter > 0)
        {
            Point3F point3F;
            memcpy(&(point3F.x), ptempbuf+offset, sizeof(point3F.x));
            offset += sizeof(point3F.x);
            memcpy(&(point3F.y), ptempbuf+offset, sizeof(point3F.y));
            offset += sizeof(point3F.y);
            memcpy(&(point3F.z), ptempbuf+offset, sizeof(point3F.z));
            offset += sizeof(point3F.z);
            handResult.palmCenter = point3F;
        }

        //points2D
        memcpy(&(handResult.point2DCount), ptempbuf+offset, sizeof(handResult.point2DCount));
        offset += sizeof(handResult.point2DCount);
        if(handResult.point2DCount > 0)
        {
            int k = 0;
            for(;k < handResult.point2DCount;k++)
            {
                Point3F point3F;
                memcpy(&(point3F.x), ptempbuf+offset, sizeof(point3F.x));
                offset += sizeof(point3F.x);
                memcpy(&(point3F.y), ptempbuf+offset, sizeof(point3F.y));
                offset += sizeof(point3F.z);
                memcpy(&(point3F.z), ptempbuf+offset, sizeof(point3F.z));
                offset += sizeof(point3F.z);
                handResult.points2D.push_back(point3F);
            }
        }

        //points3D
        memcpy(&(handResult.point3DCount), ptempbuf+offset, sizeof(handResult.point3DCount));
        offset += sizeof(handResult.point3DCount);
        if(handResult.point3DCount > 0)
        {
            int k = 0;
            for(;k < handResult.point3DCount;k++)
            {
                Point3F point3F;
                memcpy(&(point3F.x), ptempbuf+offset, sizeof(point3F.x));
                offset += sizeof(point3F.x);
                memcpy(&(point3F.y), ptempbuf+offset, sizeof(point3F.y));
                offset += sizeof(point3F.y);
                memcpy(&(point3F.z), ptempbuf+offset, sizeof(point3F.z));
                offset += sizeof(point3F.z);
                handResult.points3D.push_back(point3F);
            }
        }



        //jointsNorm
        memcpy(&(handResult.jointsNormCount), ptempbuf+offset, sizeof(handResult.jointsNormCount));
        offset += sizeof(handResult.jointsNormCount);
        if(handResult.jointsNormCount > 0)
        {
            int k = 0;
            for(;k < handResult.jointsNormCount;k++)
            {
                float jointsNorm;
                memcpy(&(jointsNorm), ptempbuf+offset, sizeof(jointsNorm) );
                offset += sizeof(jointsNorm);
                handResult.jointsNorm.push_back(jointsNorm);
            }
        }

        if(offset > bufsize) {
            LOGI("UnserializeToCvHandTrackingResult offset:%i, bufsize:%i", offset, bufsize);
        }
        //bgrSegData
        memcpy(&(handResult.rgbSegWidth), ptempbuf+offset, sizeof(handResult.rgbSegWidth));
        offset += sizeof(handResult.rgbSegWidth);
        memcpy(&(handResult.rgbSegHeight), ptempbuf+offset, sizeof(handResult.rgbSegHeight));
        offset += sizeof(handResult.rgbSegHeight);
        int32_t rgbSegDataSize = handResult.rgbSegWidth * handResult.rgbSegHeight;
        handResult.rgbSegData = (char *)(ptempbuf+offset);
        handResult.rgbSegDataSize = rgbSegDataSize;
        offset += rgbSegDataSize;

        //depthSegData
        memcpy(&(handResult.depthSegWidth), ptempbuf+offset, sizeof(handResult.depthSegWidth));
        offset += sizeof(handResult.depthSegWidth);
        memcpy(&(handResult.depthSegHeight), ptempbuf+offset, sizeof(handResult.depthSegHeight));
        offset += sizeof(handResult.depthSegHeight);
        int32_t depthSegDataSize = handResult.depthSegWidth * handResult.depthSegHeight;
        handResult.depthSegData = (char *)(ptempbuf+offset);
        handResult.depthSegDataSize = depthSegDataSize;
        offset += depthSegDataSize;

        result.push_back(handResult);
    }
    return offset;
}


int32_t HandGestureAlgorithmResult::UnserializeToCvHandTrackingResult(const char *pbuffer,
                                                                      int bufsize,
                                                                      std::vector<CvHandTrackingResult> &result) {


    if(pbuffer == nullptr || bufsize == 0)
    {
        return -1;
    }
    const char *ptempbuf = pbuffer;
    int32_t offset = 0;
    int32_t count = 0;
    // count
    memcpy(&(count), ptempbuf+offset, sizeof(count));
    offset += sizeof(count);
    if(count == 0)
    {
        return offset;
    }

    int32_t version = 1;
    // version
    memcpy(&(version), ptempbuf+offset, sizeof(version));
    offset += sizeof(version);
    if(version == 1) {
        offset = UnserializeToCvHandTrackingResultVersion1(pbuffer,bufsize,offset,count,result);
    }

    return offset;
};


/*
void HandGestureAlgorithmResult::cvHandTrackingResultToCHandGesture(
        CvHandTrackingResult &cvHandTrackingResult, standardar::CHandGesture &out_CHandGesture) {

    out_CHandGesture.m_gesture_type = cvHandTrackingResult.handType;
    out_CHandGesture.m_gesture_type_confidence = cvHandTrackingResult.score;

    out_CHandGesture.m_side = cvHandTrackingResult.handSide;
    out_CHandGesture.m_towards = cvHandTrackingResult.handToward;

    out_CHandGesture.m_rect_left = cvHandTrackingResult.handRect.left;
    out_CHandGesture.m_rect_top = cvHandTrackingResult.handRect.top;
    out_CHandGesture.m_rect_right = cvHandTrackingResult.handRect.right;
    out_CHandGesture.m_rect_bottom = cvHandTrackingResult.handRect.bottom;
    
    out_CHandGesture.m_palm_center[0] = cvHandTrackingResult.palmCenter.x;
    out_CHandGesture.m_palm_center[1] = cvHandTrackingResult.palmCenter.y;
    out_CHandGesture.m_palm_center[2] = cvHandTrackingResult.palmCenter.z;

    out_CHandGesture.m_palm_normal[0] = cvHandTrackingResult.palmNormal.x;
    out_CHandGesture.m_palm_normal[1] = cvHandTrackingResult.palmNormal.y;
    out_CHandGesture.m_palm_normal[2] = cvHandTrackingResult.palmNormal.z;

    out_CHandGesture.m_landmark2d_count = cvHandTrackingResult.point2DCount;
    out_CHandGesture.m_landmark3d_count = cvHandTrackingResult.point3DCount;



    for(int i=0; i<cvHandTrackingResult.point2DCount; i++)
    {

        out_CHandGesture.m_landmark_2D[i*2] = cvHandTrackingResult.points2D[i].x;
        out_CHandGesture.m_landmark_2D[i*2+1] = cvHandTrackingResult.points2D[i].y;

//            LOGI("czz cvHandTrackingResult landmark 2d %d %f x, %f y",i, cvHandTrackingResult.points2D[i].x, cvHandTrackingResult.points2D[i].y);
    }

    for(int i=0; i<cvHandTrackingResult.point3DCount; i++)
    {
        out_CHandGesture.m_landmark_3D[i*3] = cvHandTrackingResult.points3D[i].x;
        out_CHandGesture.m_landmark_3D[i*3+1] = cvHandTrackingResult.points3D[i].y;
        out_CHandGesture.m_landmark_3D[i*3+2] = cvHandTrackingResult.points3D[i].z;

//            LOGI("czz cvHandTrackingResult landmark 3d %d %f x, %f y, %f z",i, cvHandTrackingResult.points3D[i].x, cvHandTrackingResult.points3D[i].y, cvHandTrackingResult.points3D[i].z);
    }


    out_CHandGesture.m_rgb_seg_height = cvHandTrackingResult.rgbSegHeight;
    out_CHandGesture.m_rgb_seg_width = cvHandTrackingResult.rgbSegWidth;
    out_CHandGesture.m_rgb_seg_array = &cvHandTrackingResult.rgbSegData[0];


}
*/




bool HandGestureAlgorithmResult::resultIsEmpty() {

    if(m_HandGestures.size()==0){
        return true;
    }

    return false;
}

void HandGestureAlgorithmResult::getResult(
        std::vector<standardar::ITrackable *> &trackalbe_array) {
    ScopeMutex m(&m_Mutex);
    for (auto citer = m_HandGestures.begin(); citer != m_HandGestures.end(); citer++) {
        if(citer->second->m_State == ARTRACKING_STATE_SUCCESS)
            trackalbe_array.push_back(citer->second);
    }

}

std::vector<int> HandGestureAlgorithmResult::getAllHandGestureId() {

    HandGestureAlgorithmProxyWrapper handGestureAlgorithmProxyWrapper(m_AlgorithmCore, true);

    return handGestureAlgorithmProxyWrapper.getAllHandGestureId();
}

int32_t HandGestureAlgorithmResult::getHandSide(int32_t handId) {

    HandGestureAlgorithmProxyWrapper handGestureAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return handGestureAlgorithmProxyWrapper.getHandSide(handId);
}


int32_t HandGestureAlgorithmResult::getHandGestureType(int32_t handId) {
    HandGestureAlgorithmProxyWrapper handGestureAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return handGestureAlgorithmProxyWrapper.getHandGestureType(handId);

}

int32_t HandGestureAlgorithmResult::getHandToward(int32_t handId){
    HandGestureAlgorithmProxyWrapper handGestureAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return handGestureAlgorithmProxyWrapper.getHandToward(handId);
}


float_t HandGestureAlgorithmResult::getHandTypeConfidence(int32_t handId) {
    HandGestureAlgorithmProxyWrapper handGestureAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return handGestureAlgorithmProxyWrapper.getHandTypeConfidence(handId);
}

int32_t HandGestureAlgorithmResult::getLandMark2DCount(int32_t handId) {
    HandGestureAlgorithmProxyWrapper handGestureAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return handGestureAlgorithmProxyWrapper.getLandMark2DCount(handId);
}


void HandGestureAlgorithmResult::getLandMark2DArray(int32_t handId, float *out_landMark2DArray) {
    HandGestureAlgorithmProxyWrapper handGestureAlgorithmProxyWrapper(m_AlgorithmCore, true);
     handGestureAlgorithmProxyWrapper.getLandMark2DArray(handId,out_landMark2DArray);
}









