#include "BodyDetectAlgorithmResult.h"
#include "ArBodyDetctNode.h"
using namespace standardar;
BodyDetectAlgorithmResult::BodyDetectAlgorithmResult()
{

    m_Mutex = AR_PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
}

BodyDetectAlgorithmResult::~BodyDetectAlgorithmResult()
{
    cleanResult();
}

void BodyDetectAlgorithmResult::updateResult(jbyte *result_ptr, int pos, int length, int order)
{


   /* DirectByteBufferWrapper result_buffer(result_ptr + pos, length, (BUFFER_ORDER)order);
    int body_count = result_buffer.get<int>();

    if(body_count>0){


        int body_id = result_buffer.get<int>();
        int body_joints2d_count = result_buffer.get<int>();

         float joints2d_array[body_joints2d_count*2];

        result_buffer.get(joints2d_array, sizeof(joints2d_array));

        float joints2d_confidence_array[body_joints2d_count];

        result_buffer.get(joints2d_confidence_array, sizeof(joints2d_confidence_array));


        int joints3d_count = result_buffer.get<int>();
        float joints3d_array[joints3d_count*3];

        result_buffer.get(joints3d_array, sizeof(joints3d_array));

        float joints3d_confidence_array[joints3d_count];

        result_buffer.get(joints3d_confidence_array, sizeof(joints3d_confidence_array));


        int cache_face_num = m_bodys.size();
        auto pos =  m_bodys.find(body_id);

        if (pos == m_bodys.end()) {
            //do not find

            clearBodyMap();

            CBodyDetectNode * cBodyDetectNode = new CBodyDetectNode(body_id,body_joints2d_count,joints3d_count);
            cBodyDetectNode->updateJoints2dArray(joints2d_array);
            cBodyDetectNode->updateJoints2dConfidence(joints2d_confidence_array);
            cBodyDetectNode->updateJoints3dArray(joints3d_array);
            cBodyDetectNode->updateJoints3dConfidence(joints3d_confidence_array);

            m_bodys.insert(std::make_pair(body_id, cBodyDetectNode));

        } else{
            reinterpret_cast<CBodyDetectNode*>(pos->second)->updateJoints2dArray(joints2d_array);
            reinterpret_cast<CBodyDetectNode*>(pos->second)->updateJoints2dConfidence(joints2d_confidence_array);
            reinterpret_cast<CBodyDetectNode*>(pos->second)->updateJoints3dArray(joints3d_array);
            reinterpret_cast<CBodyDetectNode*>(pos->second)->updateJoints3dConfidence(joints3d_confidence_array);



        }


        LOGI("===========updateResult====body_count %d ======body_id %d ===== body_joints2d_count %d===",body_count,body_id,body_joints2d_count);

    } else{

            clearBodyMap();

    }*/

}

void BodyDetectAlgorithmResult::update()
{
    ScopeMutex m(&m_Mutex);
    std::vector<int> bodyIds = getAllBodyId();
    int size = bodyIds.size();

    if(size>0){


        for (int i = 0; i < size; ++i) {
            int body_id = bodyIds[i];
            auto pos = m_bodys.find(body_id);
            if(pos==m_bodys.end())
            {
              clearBodyMap();
                CBodyDetectNode * cBodyDetectNode = new CBodyDetectNode(body_id);

//                CHandGesture* psHandGesture = new CHandGesture(hand_id);
//                //  psHandGesture->m_isUpdated = is_updated;
                m_bodys.insert(std::make_pair(body_id, cBodyDetectNode));
//                LOGI("Insert a HandGesture:%i", body_id);
            }
        }
    } else{
        clearBodyMap();
    }
}


void BodyDetectAlgorithmResult::clearBodyMap() {

  /*  std::map<int32_t,CBodyDetectNode*>::iterator iter;
    for (iter=m_bodys.begin(); iter!=m_bodys.end(); iter++) {
        delete iter->second;
    }
*/
    m_bodys.clear();
}

void BodyDetectAlgorithmResult::cleanResult()
{
    ScopeMutex m(&m_Mutex);

  //clearBodyMap();
}

void BodyDetectAlgorithmResult::getResult(std::vector<standardar::ITrackable *> &trackalbe_array) {
    ScopeMutex m(&m_Mutex);
    std::map<int32_t,CBodyDetectNode*>::iterator iter;
    for (iter=m_bodys.begin(); iter!=m_bodys.end(); iter++) {
        trackalbe_array.push_back((iter->second) );
    }
}


bool BodyDetectAlgorithmResult::resultIsEmpty() {
    return m_bodys.size()>0? false: true;
}


std::vector<int> BodyDetectAlgorithmResult::getAllBodyId() {
    BodyAlgorithmProxyWrapper bodyAlgorithmProxyWrapper(m_AlgorithmCore, true);

    return bodyAlgorithmProxyWrapper.getAllBodyId();
}


int BodyDetectAlgorithmResult::getSkeletonPoint2dCount(int32_t body_id) {
    BodyAlgorithmProxyWrapper bodyAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return bodyAlgorithmProxyWrapper.getSkeletonPoint2dCount(body_id);
}

void BodyDetectAlgorithmResult::getSkeletonPoint2d(int32_t body_id, float *out_point2d) {
    BodyAlgorithmProxyWrapper bodyAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return bodyAlgorithmProxyWrapper.getSkeletonPoint2d(body_id,out_point2d);
}


void BodyDetectAlgorithmResult::getSkeletonPoint2dConfidence(int32_t body_id,
                                                             float *out_point2d_confidence) {
    BodyAlgorithmProxyWrapper bodyAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return bodyAlgorithmProxyWrapper.getSkeletonPoint2dConfidence(body_id,out_point2d_confidence);

}





