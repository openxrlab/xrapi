#include "FaceMeshAlgorithmResult.h"
#include "ArFaceMesh.h"
using namespace standardar;


FaceMeshAlgorithmResult::FaceMeshAlgorithmResult() {
    m_Mutex = AR_PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
}








void FaceMeshAlgorithmResult::update() {
    ScopeMutex m(&m_Mutex);

    std::vector<int> faceIds = getAllFaceMeshId();

    int size = faceIds.size();

    if(size>0){


        for (int i = 0; i < size; ++i) {
            int face_id = faceIds[i];
            auto pos = m_FaceMeshs.find(face_id);
            if(pos==m_FaceMeshs.end())
            {
                m_FaceMeshs.clear();
                ArFaceMesh* ptrArFaceMesh= new ArFaceMesh(face_id);
                //  psHandGesture->m_isUpdated = is_updated;
                m_FaceMeshs.insert(std::make_pair(face_id, ptrArFaceMesh));
                LOGI("Insert a ArFaceMesh:%i", face_id);
            }
        }
    } else{
        m_FaceMeshs.clear();
    }


}

bool FaceMeshAlgorithmResult::resultIsEmpty() {
    ScopeMutex m(&m_Mutex);
   return m_FaceMeshs.size()>0? false: true;
}


void FaceMeshAlgorithmResult::getResult(std::vector<standardar::ITrackable *> &trackalbe_array) {
    ScopeMutex m(&m_Mutex);
    for (auto citer = m_FaceMeshs.begin(); citer != m_FaceMeshs.end(); citer++) {
        if(citer->second->m_State == ARTRACKING_STATE_SUCCESS)
            trackalbe_array.push_back(citer->second);
    }

}

void FaceMeshAlgorithmResult::cleanResult() {
    ScopeMutex m(&m_Mutex);

}

std::vector<int> FaceMeshAlgorithmResult::getAllFaceMeshId() {
    FaceMeshAlgorithmProxyWrapper faceMeshAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return faceMeshAlgorithmProxyWrapper.getAllFaceMeshId();
}


FaceMeshAlgorithmResult::~FaceMeshAlgorithmResult() {
    cleanResult();
}


int32_t FaceMeshAlgorithmResult::getVertexCount(int32_t face_id) {
    FaceMeshAlgorithmProxyWrapper faceMeshAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return faceMeshAlgorithmProxyWrapper.getVertexCount(face_id);
}

int32_t FaceMeshAlgorithmResult::getIndexCount(int32_t face_id) {
    FaceMeshAlgorithmProxyWrapper faceMeshAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return faceMeshAlgorithmProxyWrapper.getIndexCount(face_id);
}


int32_t FaceMeshAlgorithmResult::getTextureCoordinateCount(int32_t face_id) {
    FaceMeshAlgorithmProxyWrapper faceMeshAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return faceMeshAlgorithmProxyWrapper.getTextureCoordinateCount(face_id);
}


int32_t FaceMeshAlgorithmResult::getNormalCount(int32_t face_id) {
    FaceMeshAlgorithmProxyWrapper faceMeshAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return faceMeshAlgorithmProxyWrapper.getNormalCount(face_id);
}


float* FaceMeshAlgorithmResult::getVertices(int32_t face_id) {
    FaceMeshAlgorithmProxyWrapper faceMeshAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return faceMeshAlgorithmProxyWrapper.getVertices(face_id);
}


int32_t* FaceMeshAlgorithmResult::getIndices(int32_t face_id) {
    FaceMeshAlgorithmProxyWrapper faceMeshAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return faceMeshAlgorithmProxyWrapper.getIndices(face_id);
}

float* FaceMeshAlgorithmResult::getTextureCoordinates(int32_t face_id) {
    FaceMeshAlgorithmProxyWrapper faceMeshAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return faceMeshAlgorithmProxyWrapper.getTextureCoordinates(face_id);
}


float* FaceMeshAlgorithmResult::getNormals(int32_t face_id) {
    FaceMeshAlgorithmProxyWrapper faceMeshAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return faceMeshAlgorithmProxyWrapper.getNormals(face_id);

}


void FaceMeshAlgorithmResult::getPose(int32_t face_id, float *out_pose_ptr) {
    FaceMeshAlgorithmProxyWrapper faceMeshAlgorithmProxyWrapper(m_AlgorithmCore, true);
    faceMeshAlgorithmProxyWrapper.getPose(face_id,out_pose_ptr);
}