#include "ObjectTrackingAlgorithmResult.h"
using namespace standardar;
ObjectTrackingAlgorithmResult::ObjectTrackingAlgorithmResult()
{
    m_Mutex = AR_PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
}

void ObjectTrackingAlgorithmResult::setReferenceObjectDatabase(CReferenceObjectDatabase* database)
{
    m_ReferenceObjectDatabase.setObjectDatabase(database);
}

void ObjectTrackingAlgorithmResult::sendReferenceObjectDatabase()
{
    SLAMAlgorithmProxyWrapper objectTrackingAlgorithmProxyWrapper(m_AlgorithmCore, true);
    objectTrackingAlgorithmProxyWrapper.setReferenceObjectDatabase(m_ReferenceObjectDatabase);
}

void ObjectTrackingAlgorithmResult::getReferenceObjectDatabase(CReferenceObjectDatabase* database)
{
    m_ReferenceObjectDatabase.getObjectDatabase(database);
}

CReferenceObjectDatabase* ObjectTrackingAlgorithmResult::getReferenceObjectDatabase()
{
    return &m_ReferenceObjectDatabase;
}

std::string ObjectTrackingAlgorithmResult::getObjectTrackingJsonInfo()
{
    SLAMAlgorithmProxyWrapper objectTrackingAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return objectTrackingAlgorithmProxyWrapper.getObjectTrackingInfo();
}

void ObjectTrackingAlgorithmResult::getCameraViewMatrix(float* view, int screen_rotate)
{
    SLAMAlgorithmProxyWrapper objectTrackingAlgorithmProxyWrapper(m_AlgorithmCore, true);
    objectTrackingAlgorithmProxyWrapper.getCameraViewMatrix(view, screen_rotate);
}

void ObjectTrackingAlgorithmResult::getCenterPose(int32_t marker_id, CPose& out_pose)
{
    SLAMAlgorithmProxyWrapper objectTrackingAlgorithmProxyWrapper(m_AlgorithmCore, true);
    float pose_array[7];
    objectTrackingAlgorithmProxyWrapper.getObjectCenterPose(marker_id, pose_array);

    out_pose.qx = pose_array[0];
    out_pose.qy = pose_array[1];
    out_pose.qz = pose_array[2];
    out_pose.qw = pose_array[3];
    out_pose.tx = pose_array[4];
    out_pose.ty = pose_array[5];
    out_pose.tz = pose_array[6];
}

float ObjectTrackingAlgorithmResult::getExtentX(int32_t marker_id)
{
    SLAMAlgorithmProxyWrapper objectTrackingAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return objectTrackingAlgorithmProxyWrapper.getExtentX(marker_id);
}

float ObjectTrackingAlgorithmResult::getExtentY(int32_t marker_id)
{
    SLAMAlgorithmProxyWrapper objectTrackingAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return objectTrackingAlgorithmProxyWrapper.getExtentY(marker_id);
}

float ObjectTrackingAlgorithmResult::getExtentZ(int32_t marker_id)
{
    SLAMAlgorithmProxyWrapper objectTrackingAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return objectTrackingAlgorithmProxyWrapper.getExtentZ(marker_id);
}

void ObjectTrackingAlgorithmResult::getBoundingBox(int32_t object_id, float* boundingBox)
{
    SLAMAlgorithmProxyWrapper objectTrackingAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return objectTrackingAlgorithmProxyWrapper.getBoundingBox(object_id, boundingBox);
}

std::vector<int> ObjectTrackingAlgorithmResult::getAllObjectId()
{
    SLAMAlgorithmProxyWrapper objectTrackingAlgorithmProxyWrapper(m_AlgorithmCore, true);
    return objectTrackingAlgorithmProxyWrapper.getAllObjectId();
}
