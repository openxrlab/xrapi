#ifndef FACE_MESH_C_SRC_ARFACEMESH_H
#define FACE_MESH_C_SRC_ARFACEMESH_H

#include "ArTrackable.h"
#include "FaceMeshAlgorithmResult.h"
namespace standardar
{
    class ArFaceMesh : public ITrackable
    {
    public:
        int32_t  m_face_id;
        int vertices_count;
        float* vertices;

        float* normals;
        int normal_count;

        int triangle_indices_count;
        int *triangle_indices;

        int texture_coordinates_count;
        float* texture_coordinates;


        float* headPose;




    public:
        ArFaceMesh(int32_t face_id);
        virtual ~ArFaceMesh();

        int32_t  getVertexCount(FaceMeshAlgorithmResult* result);
        int32_t  getIndexCount(FaceMeshAlgorithmResult* result);

        int32_t  getTextureCoordinateCount(FaceMeshAlgorithmResult* result);

        int32_t  getNormalCount(FaceMeshAlgorithmResult* result);

        float*  getVertices(FaceMeshAlgorithmResult* result);

        int32_t*   getIndices(FaceMeshAlgorithmResult* result);

        float*   getTextureCoordinates(FaceMeshAlgorithmResult* result);

        float*  getNormals(FaceMeshAlgorithmResult* result);

        void getPose(FaceMeshAlgorithmResult* result,float *out_pose_ptr);

    };


}



#endif //FACE_MESH_C_SRC_ARFACEMESH_H
