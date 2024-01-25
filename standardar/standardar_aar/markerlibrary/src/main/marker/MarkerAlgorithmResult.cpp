#include "MarkerAlgorithmResult.h"
#include "ArPose.h"
using namespace standardar;
MarkerAlgorithmResult::MarkerAlgorithmResult()
{
    m_Mutex = AR_PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
}

MarkerAlgorithmResult::~MarkerAlgorithmResult()
{

}

//void MarkerAlgorithmResult::update()
//{
//
//}

void MarkerAlgorithmResult::setReferenceImageDatabase(CImageNodeMgr* database)
{
    m_AugmentedImageDatabase.setReferenceImageDatabase(database);
}

void MarkerAlgorithmResult::sendReferenceImageDatabase()
{
    MarkerAlgorithmProxyWrapper markerAlgorithmProxyWrapper(m_AlgorithmCore, true);
    markerAlgorithmProxyWrapper.setReferenceImageDatabase(m_AugmentedImageDatabase);
}

void MarkerAlgorithmResult::getReferenceImageDatabase(CImageNodeMgr* database)
{
    m_AugmentedImageDatabase.getReferenceImageDatabase(database);
}

CImageNodeMgr* MarkerAlgorithmResult::getReferenceImageDatabase()
{
    return &m_AugmentedImageDatabase;
}

void MarkerAlgorithmResult::getCenterPose(int32_t marker_id, int screen_rotate, float* viewArray, CPose& out_pose)
{
    MarkerAlgorithmProxyWrapper markerAlgorithmProxyWrapper(m_AlgorithmCore, true);
    float pose_array[7];
    markerAlgorithmProxyWrapper.getCenterPose(marker_id, viewArray, screen_rotate, pose_array);

    out_pose.qx = pose_array[0];
    out_pose.qy = pose_array[1];
    out_pose.qz = pose_array[2];
    out_pose.qw = pose_array[3];
    out_pose.tx = pose_array[4];
    out_pose.ty = pose_array[5];
    out_pose.tz = pose_array[6];
}

void MarkerAlgorithmResult::getCorners2d(int32_t marker_id, float* corners_2d)
{
    MarkerAlgorithmProxyWrapper markerAlgorithmProxyWrapper(m_AlgorithmCore, true);
    markerAlgorithmProxyWrapper.get2DCorners(marker_id, corners_2d);
}

float MarkerAlgorithmResult::getExtentX(int32_t marker_id)
{
    MarkerAlgorithmProxyWrapper markerAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return markerAlgorithmProxyWrapper.getExtentX(marker_id);
}

float MarkerAlgorithmResult::getExtentY(int32_t marker_id)
{
    MarkerAlgorithmProxyWrapper markerAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return markerAlgorithmProxyWrapper.getExtentY(marker_id);
}

float MarkerAlgorithmResult::getExtentZ(int32_t marker_id)
{
    MarkerAlgorithmProxyWrapper markerAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return markerAlgorithmProxyWrapper.getExtentZ(marker_id);
}

std::vector<int> MarkerAlgorithmResult::getAllMarkerId()
{
    MarkerAlgorithmProxyWrapper markerAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return markerAlgorithmProxyWrapper.getAllMarkerId();
}
