#include "StandardARCommon.h"
#include "ArPose.h"
#include "ArConfig.h"
#include "ArAnchor.h"
#include "ArFrame.h"
#include "ArHitResult.h"
#include "ArPlane.h"
#include "ArSession.h"
#include "ArTrackable.h"

using namespace standardar;

extern "C" {


void ArPoseTostPose(const ARPose_& arpose, CPose& stpose)
{
    stpose.qx = arpose.qx;
    stpose.qy = arpose.qy;
    stpose.qz = arpose.qz;
    stpose.qw = arpose.qw;
    stpose.tx = arpose.tx;
    stpose.ty = arpose.ty;
    stpose.tz = arpose.tz;
}

void stPoseToArPose(const CPose& stpose, ARPose_& arpose)
{
    arpose.qx = stpose.qx;
    arpose.qy = stpose.qy;
    arpose.qz = stpose.qz;
    arpose.qw = stpose.qw;
    arpose.tx = stpose.tx;
    arpose.ty = stpose.ty;
    arpose.tz = stpose.tz;
}

bool IsARAnchorAvalid(const ARAnchor_ *anchor) {
    if (anchor == NULL || anchor->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARAnchorlistAvalid(const ARAnchorList_ *anchorlist) {
    if (anchorlist == NULL || anchorlist->m_pImpl == NULL)
        return false;

    return true;
}


bool IsARConfigAvalid(const ARConfig_ *config) {
    if (config == NULL || config->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARReferenceImageDatabaseAvalid(const ARReferenceImageDatabase_ *image_node_mgr) {
    if (image_node_mgr == NULL || image_node_mgr->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARReferenceObjectDatabaseAvalid(const ARReferenceObjectDatabase_ *object_database) {
    if (object_database == NULL || object_database->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARCameraConfigAvalid(const ARCameraConfig_ *camera_confg) {
    if (camera_confg == NULL || camera_confg->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARCameraConfigListAvalid(const ARCameraConfigList_ *camera_config_list) {
    if (camera_config_list == NULL || camera_config_list->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARCameraIntrinsicsAvalid(const ARCameraIntrinsics_ *camera_intrinsics) {
    if (camera_intrinsics == NULL || camera_intrinsics->m_pImpl == NULL)
        return false;

    return true;
}
bool IsARCameraAvalid(const ARCamera_ *camera) {
    if (camera == NULL || camera->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARFrameAvalid(const ARFrame_* frame)
{
    if (frame == NULL || frame->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARPointCloudAvalid(const ARPointCloud_* ptcloud)
{
    if (ptcloud == NULL || ptcloud->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARImageNodeAvalid(const ARImageNode_ *image_node) {
    if (image_node == NULL || image_node->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARObjectNodeAvalid(const ARObjectNode_ *object_node) {
    if (object_node == NULL || object_node->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARDenseMeshAvalid(const ARDenseMesh_* ptDenseMesh)
{
    if (ptDenseMesh == NULL || ptDenseMesh->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARPlaneNodeAvalid(const ARPlaneNode_* plane)
{
    if (plane == NULL || plane->m_pImpl == NULL)
        return false;

    return true;
}

bool IsARSessionAvalid(const ARSession_ *session)
{
    if (session == nullptr || session->m_pImpl == nullptr)
        return false;

    return true;
}

bool IsARMapAvalid(const ARMap_ *map)
{
    if (map == nullptr || map->m_pImpl == nullptr)
        return false;

    return true;
}

bool IsARNodeAvalid(const ARNode_ *node)
{
    if (node == nullptr || node->m_pImpl == nullptr)
        return false;

    return true;
}

bool IsARNodeListAvalid(const ARNodeList_ * nodelist)
{
    if (nodelist == nullptr || nodelist->m_pImpl == nullptr)
        return false;

    return true;
}

bool IsARIlluminationEstimateAvalid(const ARIlluminationEstimate_* illuminationestimate)
{
    if (illuminationestimate == nullptr || illuminationestimate->m_pImpl == nullptr)
        return false;

    return true;
}

bool IsARQueryResultListAvalid(const ARQueryResultList_* queryResultList)
{
    if (queryResultList == nullptr || queryResultList->m_pImpl == nullptr)
        return false;

    return true;
}

bool IsARQueryResultAvalid(const ARQueryResult_* queryResult)
{
    if (queryResult == nullptr || queryResult->m_pImpl == nullptr)
        return false;

    return true;
}



bool IsARHandGestureAvalid(const ARHandGestureNode_* handGesture)
{
    if (handGesture == nullptr || handGesture->m_pImpl == nullptr)
        return false;

    return true;
}

bool IsARFaceMeshAvalid(const ARFaceMeshNode * arFaceMeshNode){

    if (arFaceMeshNode == nullptr || arFaceMeshNode->m_pImpl == nullptr)
        return false;

    return true;
}

bool IsARBodyDetectAvalid(const ARBodyDetectNode_* arBodyDetectNode){
    if (arBodyDetectNode == nullptr || arBodyDetectNode->m_pImpl == nullptr)
        return false;

    return true;
}

/// @}
}



