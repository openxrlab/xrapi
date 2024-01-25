#include "ArFaceMesh.h"
#include <cstring>

namespace standardar
{

    ArFaceMesh::ArFaceMesh(int32_t face_id):ITrackable(ARNODE_TYPE_FACE_MESH) ,m_face_id(face_id){

    }


    ArFaceMesh::~ArFaceMesh() {

    }


    int32_t ArFaceMesh::getVertexCount(standardar::FaceMeshAlgorithmResult *result) {
        return result->getVertexCount(m_face_id);
    }


    int32_t ArFaceMesh::getIndexCount(standardar::FaceMeshAlgorithmResult *result) {
        return result->getIndexCount(m_face_id);
    }

    int32_t ArFaceMesh::getTextureCoordinateCount(standardar::FaceMeshAlgorithmResult *result) {
        return result->getTextureCoordinateCount(m_face_id);
    }


    int32_t ArFaceMesh::getNormalCount(standardar::FaceMeshAlgorithmResult *result) {
        return result->getNormalCount(m_face_id);
    }


    float* ArFaceMesh::getVertices(standardar::FaceMeshAlgorithmResult *result) {

        return result->getVertices(m_face_id);
    }

    int32_t *  ArFaceMesh::getIndices(standardar::FaceMeshAlgorithmResult *result) {
        return result->getIndices(m_face_id);
    }


    float*  ArFaceMesh::getTextureCoordinates(standardar::FaceMeshAlgorithmResult *result) {
        return result->getTextureCoordinates(m_face_id);

    }


    float*  ArFaceMesh::getNormals(standardar::FaceMeshAlgorithmResult *result) {
        return result->getNormals(m_face_id);
    }


    void ArFaceMesh::getPose(standardar::FaceMeshAlgorithmResult *result,float *out_pose_ptr) {
        result->getPose(m_face_id,out_pose_ptr);
    }


}
