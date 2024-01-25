#ifndef FACE_MESH_C_SRC_FACEMESHALGORITHMRESULT_H
#define FACE_MESH_C_SRC_FACEMESHALGORITHMRESULT_H

#include "AlgorithmResult.h"

namespace standardar {

    class FaceMeshAlgorithmResult : public AlgorithmResult {
    public:
        FaceMeshAlgorithmResult();

        virtual void update();
        virtual void cleanResult();

        bool resultIsEmpty();
        ~FaceMeshAlgorithmResult();
        void getResult(std::vector<ITrackable*>& trackalbe_array);

        std::vector<int> getAllFaceMeshId();


        int32_t getVertexCount(int32_t face_id);
        int32_t  getIndexCount(int32_t face_id);

        int32_t  getTextureCoordinateCount(int32_t face_id);

        int32_t  getNormalCount(int32_t face_id);

        float*  getVertices(int32_t face_id);
        int32_t*  getIndices(int32_t face_id);
        float*  getTextureCoordinates(int32_t face_id);

        float*  getNormals(int32_t face_id);
        void  getPose(int32_t face_id,float *out_pose_ptr);




    private:

        pthread_mutex_t m_Mutex;

        std::map<int32_t , ITrackable*>    m_FaceMeshs;


    };

}

#endif //FACE_MESH_C_SRC_FACEMESHALGORITHMRESULT_H
