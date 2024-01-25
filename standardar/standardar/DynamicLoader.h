#ifndef HELLO_AR_C_SRC_DYNAMICLOADER_H
#define HELLO_AR_C_SRC_DYNAMICLOADER_H

#include <jni.h>
#include <string>
#include <vector>
#include "ArMath.h"
#include "ArCamera.h"
#include "StandardAR.h"

namespace standardar {
    class DexClassLoaderWrapper;
    class DynamicLoader {
    public:
        /**
         * call before use DynamicLoader
         * @param env
         * @param context
         */
        static void init(JNIEnv* env, jobject context);
        /**
         * find class in default classpath
         * @param className
         * @return
         */
        static jclass findClassGlobalRef(std::string className);
        /**
         * get method id helper function
         * @param clz
         * @param name
         * @param sig
         * @return
         */
        static jmethodID getMethodId(jclass clz, std::string name, std::string sig);
        /**
         * get static method id helper function
         * @param clz
         * @param name
         * @param sig
         * @return
         */
        static jmethodID getStaticMethodId(jclass clz, std::string name, std::string sig);
        /**
         * get field id helper function
         * @param clz
         * @param name
         * @param sig
         * @return
         */
        static jfieldID getFieldId(jclass clz, std::string name, std::string sig);
        /**
         * get static field id helper function
         * @param clz
         * @param name
         * @param sig
         * @return
         */
        static jfieldID getStaticFieldId(jclass clz, std::string name, std::string sig);
    public:
        static JavaVM* s_JVM;
        static DexClassLoaderWrapper* s_DexClassLoader;
        static jobject s_server_context;
        static jobject s_asset_manager;
        static bool s_bInit;
    };

    class JavaClassWrapper {
    public:
        JavaClassWrapper(jobject obj, bool is_global);
        ~JavaClassWrapper();
    public:
        jobject m_obj;
        bool m_global;
    };

    class ClassWrapper : public JavaClassWrapper {
    public:
        ClassWrapper(jobject obj, bool is_global);
        virtual ~ClassWrapper(){}
        jobject getClassLoader();
        static void init();
    private:
        static jmethodID s_getClassLoader_method;
        static std::string s_class_name;
        static jclass s_class;
    };

    class ActivityWrapper : public JavaClassWrapper {
    public:
        ActivityWrapper(jobject obj, bool is_global);
        ~ActivityWrapper(){}
        jobject getApplication();
        static void init();
    private:
        static jmethodID s_getApplication_method;
        static jclass s_class;
        static std::string s_class_name;
    };

    class ContextWrapper : public JavaClassWrapper {
    public:
        ContextWrapper(jobject obj, bool is_global);
        virtual ~ContextWrapper(){}
        jobject createPackageContext(std::string package_name, int flags);
        jobject getAssets();
        jobject getApplicationInfo();
        jobject getDir(std::string name, int mode);
        jobject getClass();
        static void init();
    private:
        static jmethodID s_createPackageContext_method;
        static jmethodID s_getAsset_method;
        static jmethodID s_getApplicationInfo_method;
        static jmethodID s_getDir_method;
        static jmethodID s_getClass_method;
        static jclass s_class;
        static std::string s_class_name;
    };

    class DexClassLoaderWrapper : public JavaClassWrapper {
    public:
        DexClassLoaderWrapper(jobject obj, bool is_global);
        virtual ~DexClassLoaderWrapper(){}
        jclass loadClassGlobalRef(std::string class_name);
        static void init();
        static jobject construct(jstring dex_path, jstring opt_dir, jstring lib_path, jobject classloader);
    private:
        static std::string s_class_name;
        static jclass s_class;
        static jmethodID s_loadClass_method;
        static jmethodID s_ctor_method;
    };

    class FileClassWrapper : public JavaClassWrapper {
    public:
        FileClassWrapper(jobject obj, bool is_global);
        virtual ~FileClassWrapper(){}
        jstring getAbsolutePath();
        static void init();
    private:
        static jmethodID s_getAbsolutePath_method;
        static jclass s_class;
        static std::string s_class_name;
    };

    class ApplicationInfoWrapper : public JavaClassWrapper {
    public:
        ApplicationInfoWrapper(jobject obj, bool is_global);
        virtual ~ApplicationInfoWrapper(){}
        static void init();
        jstring getNativeLibrarDir();
        jstring getSourceDir();
    private:
        static jfieldID s_nativeLibraryDir_filed;
        static jfieldID s_sourceDir_field;
        static jclass s_class;
        static std::string s_class_name;
    };

    class PointWrapper : public JavaClassWrapper {
    public:
        PointWrapper(jobject obj, bool is_global);
        int getX();
        int getY();
        virtual ~PointWrapper(){}
        static void init();
    private:
        static jfieldID s_x_field;
        static jfieldID s_y_field;
        static jclass s_class;
        static std::string s_class_name;
    };

    class FrameworkConstantWrapper {
    public:
        static void init();
        static std::string getARSERVICE_PACKAGE_NAME();
    private:
        FrameworkConstantWrapper(){}
    private:
        static jfieldID s_ARSERVICE_PACKAGE_NAME;
        static jclass s_class;
        static std::string s_class_name;
    };

    class CameraSourceWrapper : public JavaClassWrapper {
    public:
        CameraSourceWrapper(jobject obj, bool is_global);
        virtual ~CameraSourceWrapper(){}
        void setTextureId(int id);
        void startPreview();
        void stopPreview();
        void setCameraNotify(jobject camera_notify);
        void setRenderCamera(int id);
        void openCamera(std::vector<int>& camera_ids);
        void setPreviewSize(int w, int h);
        void closeCamera();
        void setScreenRotate(int screen_rotate);
        void updateTexture();
        void waitForNextFrame();
        float getFovH();
        float getFovV();
        std::string getSupportSizeString();
        std::vector<int> getDisplaySize();
        static void init();
        int64_t getCurImageTimestamp();
        int getCurImageWidth();
        int getCurImageHeight();
        int getCurImageStride();
        int getCurImageFormat();
        bool getCurImageData(unsigned char *y_data, unsigned char *u_data, unsigned char *v_data);
        bool isImageBufferDirty();
        void getPreviewSize(int& width, int& height);
        void getProcessSize(int& widht, int& height);
        static jobject getInstance(jobject context);
    private:
        static jmethodID s_setTextureId_method;
        static jmethodID s_startPreview_method;
        static jmethodID s_stopPreview_method;
        static jmethodID s_setCameraNotify_method;
        static jmethodID s_setRenderCamera_method;
        static jmethodID s_openCamera_method;
        static jmethodID s_setPreviewSize_method;
        static jmethodID s_closeCamera_method;
        static jmethodID s_setScreenRotate_method;
        static jmethodID s_updateTexture_method;
        static jmethodID s_waitForNextFrame_method;
        static jmethodID s_getFovH_method;
        static jmethodID s_getFovV_method;
        static jmethodID s_getSupportSizeString_method;
        static jmethodID s_getDisplaySize_method;
        static jmethodID s_getCurImageTimestamp_method;
        static jmethodID s_getCurImageWidth_method;
        static jmethodID s_getCurImageHeight_method;
        static jmethodID s_getCurImageStride_method;
        static jmethodID s_getCurImageFormat_method;
        static jmethodID s_getCurImageData_method;
        static jmethodID s_isImageBufferDirty_method;
        static jmethodID s_getInstance_method;
        static jmethodID s_init_method;
        static jmethodID s_getPreviewSize_method;
        static jmethodID s_getProcessSize_method;
        static jclass s_class;
        static std::string s_class_name;
    };

    class IMUReaderWrapper : public JavaClassWrapper {
    public:
        IMUReaderWrapper(jobject obj, bool is_global);
        ~IMUReaderWrapper(){}
        void openSensor();
        void start();
        void stop();
        void closeSensor();
        void setSensorNotify(jobject client);
        static void init();
        static jobject construct(jobject context);
    private:
        static jmethodID s_ctor_method;
        static jmethodID s_openSensor_method;
        static jmethodID s_start_method;
        static jmethodID s_stop_method;
        static jmethodID s_closeSensor_method;
        static jmethodID s_setSensorNotify_method;
        static jclass s_class;
        static std::string s_class_name;
    };

    class ClientWrapper : public JavaClassWrapper {
    public:
        ClientWrapper(jobject obj, bool is_global);
        ~ClientWrapper(){}
        static void init();
        static jobject construct(jobject server_context, jobject sdk_context);
        void setPackageName(jobject context, const char* packageName);
        void setPlaneFindingMode(int mode);
        void setCloudAnchorMode(int mode);
        void setLightingEstimateMode(int mode);
        void changeLightingEstimateMode(int mode);
        void setSLAMMode(int mode);
        void setBluetoothUUID(std::string uuid);
        void setUrl(std::string url);
        void setWorldAligmentMode(int mode);
        void setDenseReconMode(int mode);
        void setObjectTrackingMode(int mode);
        void setObjectScanningMode(int mode);
        void setAlgorithmStreamMode(int algorithmType, int mode);
        void setDeviceType(int algorithmType, int mode);
        void initAlgorithm();
        void stopAlgorithm(int type);
        void startAlgorithm(int type);
        void destroyAlgorithm();
        void hostAnchor(jobject context, bool override, float *anchor, std::string mapid,
                std::string app_key, std::string app_secret);
        void resolveAnchor(jobject context, std::string anchorid,
                            std::string app_key, std::string app_secret);
        void setUserJson(std::string json);
        jobject enableAlgorithm(int type);
        void disableAlgorithm(int type);
        void pushKeyFrame(char *imgBuff, uint64_t timestamp, int width, int height,
                          double *rot, double *trans);
        void runDenseRecon();
        void getSLAMResult();
        void setResumeFlag();
        void updateAlgorithmResult();
        void setAxisUpMode(int mode);
    private:
        static jmethodID s_ctor_method;
        static jmethodID s_setPackageName_method;
        static jmethodID s_setSLAMPlaneMode_method;
        static jmethodID s_setCloudAnchorMode_method;
        static jmethodID s_setSLAMLightEstimateMode_method;
        static jmethodID s_changeLightEstimationMode_method;
        static jmethodID s_setSLAMMode_method;
        static jmethodID s_setBluetoothUUID_method;
        static jmethodID s_setUrl_method;
        static jmethodID s_setSLMWorldAlignmentMode_mothod;
        static jmethodID s_setDenseReconMode_method;
        static jmethodID s_setObjectTrackingMode_method;
        static jmethodID s_setObjectScanningMode_method;
        static jmethodID s_setAlgorithmStreamMode_method;
        static jmethodID s_setDeviceType_method;
        static jmethodID s_initAlgorithm_method;
        static jmethodID s_stopAlgorithm_method;
        static jmethodID s_startAlgorithm_method;
        static jmethodID s_destroyAlgorithm_method;
        static jmethodID s_hostAnchor_method;
        static jmethodID s_resolveAnchor_method;
        static jmethodID s_setUserJson_method;
        static jmethodID s_enableAlgorithm_method;
        static jmethodID s_disableAlgorithm_method;
        static jmethodID s_pushKeyFrame_method;
        static jmethodID s_runDenseRecon_method;
        static jmethodID s_getDenseReconResult_method;
        static jmethodID s_getSLAMResult_method;
        static jmethodID s_setResumeFlag_method;
        static jmethodID s_updateAlgorithmResult_method;
        static jmethodID s_setAxisUpMode_method;
        static jclass s_class;
        static std::string s_class_name;
    };

    class CReferenceObjectDatabase;
    class SLAMAlgorithmProxyWrapper : public JavaClassWrapper {
    public:
        SLAMAlgorithmProxyWrapper(jobject obj, bool is_global);
        ~SLAMAlgorithmProxyWrapper(){}
        static void init();
        CCameraIntrinsics getCameraIntrinsics();
        int getTrackingState();
        std::string getSLAMInfo();
        int getCloudResult();
        std::string getMapId();
        std::string getAnchorId();
        void getCloudAnchorPose(float* pose);
        void getDenseMeshIndex(unsigned short *index_array);
        int32_t getDenseMeshIndexCount();
        int32_t getSLAMPlaneIndex(int32_t planeid);
        void getViewMatrix(float* view, int screen_rotate);
        float getSLAMLightIntensity();
        vec3f insectPlaneWithSlamResult(vec3f ray_origin, vec3f ray_direction, int32_t& plane_id, vec3f& quat);
        vec3f getPlaneCenter(int plane_id);
        void getDenseMeshVertex(float* vertex_array);
        int getDenseMeshVertexCount();
        void getDenseMeshNormal(float* vertex_normal);
        int getDenseMeshFormat();
        vec3f getCenterPose(int plane_id);
        vec3f insectSurfaceMesh(float x, float y, standardar::vec3f &normal, float screen_aspect, int screen_rotate);
        int getNumberLandMarks();
        float* getLandMarks();
        float getDistanceFromCamera(standardar::vec3f hit_pose);
        int getPlaneVertexCount();
        void getPlaneVertexArray(float* vertext_array);
        int getPlaneIndexCount();
        void getPlaneIndexArray(unsigned short *index_array);
        int getPlaneType(int plane_id);
        void getPlaneNormal(float* out_plane_normal, int plane_id);
        int getPolygonSize(int plane_id);
        void getPolygon(float *out_data, int plane_id);
        float getExtent(int axis, int plane_id);
        int getPolygon3DSize(int plane_id);
        void getPolygon3D(float *out_data, int plane_id);
        int isPoseInPolygon(vec3f pose, int plane_id);
        vec3f getPlaneOriginPoint(int plane_id);
        int isPoseInExtents(vec3f& p, int plane_id);
        std::vector<int> getAllPlaneId();
        bool isDepthImageAvaliable();
        bool getDepthImage(unsigned char* depth);
        int  getDepthImageWidth();
        int  getDepthImageHeight();
        void setBoundingBox(const float* vertex, const float* matrix);
        void getBoundingBoxPointCloud(float** points, int* points_num);
        void getScanningResult(uint8_t **out_raw_bytes, int64_t *out_raw_bytes_size);
        ARIlluminationEstimateState getLightEstimateState();
        ARIlluminationEstimateMode getLightEstimateMode();
        float getPixelIntensity();
        void getSphericalHarmonicLighting(float* out);
        void getEnvironmentTextureSize(int* out);
        void getColorCorrection(float* out);
        void getEnvironmentalHdrMainLightDirection(float* out);
        void getEnvironmentalHdrMainLightIntensity(float* out);
        void getEnvironmentalHdrCubemap(float* buffer);
        int64_t getLightTimeStamp();
        void getPredictedViewMatrix(float *view_mat);
        void getProjectionMatrixSeeThrough(float near, float far, float *proj_mat);
        void setWindow(void *native_window, void *app_context, bool newPbuffer);
        void beginRenderEye(int eye_side);
        void endRenderEye(int eye_side);
        void submitFrame(int left_tex_id, int right_tex_id);
        void setReferenceObjectDatabase(CReferenceObjectDatabase& database);
        std::string getObjectTrackingInfo();
        void getCameraViewMatrix(float* view, int screen_rotate);
        void getObjectCenterPose(int marker_id, float* pose_ptr);
        float getExtentX(int marker_id);
        float getExtentY(int marker_id);
        float getExtentZ(int marker_id);
        void getBoundingBox(int object_id, float* boundingBox);
        std::vector<int> getAllObjectId();
        std::string getRegionInfo();

    private:
        static jmethodID s_getCameraIntrinsics_method;
        static jmethodID s_getTrackingState_method;
        static jmethodID s_getSLAMInfo_method;
        static jmethodID s_getDenseMeshIndex_method;
        static jmethodID s_getDenseMeshIndexCount_method;
        static jmethodID s_getSLAMPlaneIndex_method;
        static jmethodID s_getViewMatrix_method;
        static jmethodID s_getSLAMLightIntensity_method;
        static jmethodID s_insectPlaneWithSlamResult_method;
        static jmethodID s_getPlaneCenter_method;
        static jmethodID s_getDenseMeshVertex_method;
        static jmethodID s_getDenseMeshVertexCount_method;
        static jmethodID s_getDenseMeshNormal_method;
        static jmethodID s_getDenseMeshFormat_method;
        static jmethodID s_getCenterPose_method;
        static jmethodID s_insectSurfaceMesh_method;
        static jmethodID s_getNumberLandMarks_method;
        static jmethodID s_getLandMarks_method;
        static jmethodID s_getDistanceFromCamera_method;
        static jmethodID s_getPlaneVertexCount_method;
        static jmethodID s_getPlaneVertexArray_method;
        static jmethodID s_getPlaneIndexCount_method;
        static jmethodID s_getPlaneIndexArray_method;
        static jmethodID s_getPlaneType_method;
        static jmethodID s_getPlaneNormal_method;
        static jmethodID s_getPolygonSize_method;
        static jmethodID s_getPolygon_method;
        static jmethodID s_getExtent_method;
        static jmethodID s_getPolygon3DSize_method;
        static jmethodID s_getPolygon3D_method;
        static jmethodID s_isPoseInPolygon_method;
        static jmethodID s_getPlaneOriginPoint_method;
        static jmethodID s_isPoseInExtents_method;
        static jmethodID s_getAllPlaneId_method;
        static jmethodID s_isDepthImageAvaliable_method;
        static jmethodID s_getDepthImage_method;
        static jmethodID s_getDepthImageWidth_method;
        static jmethodID s_getDepthImageHeight_method;
        static jmethodID s_setBoundingBox_method;
        static jmethodID s_getBoundingBoxPointCloud_method;
        static jmethodID s_getScanningResult_method;
        static jmethodID s_getCloudResult_method;
        static jmethodID s_getMapId_method;
        static jmethodID s_getAnchorId_method;
        static jmethodID s_getCloudAnchorPose_method;
        static jmethodID s_getLightEstimateState_method;
        static jmethodID s_getLightEstimateMode_method;
        static jmethodID s_getPixelIntensity_method;
        static jmethodID s_getSphericalHarmonicLighting_method;
        static jmethodID s_getEnvironmentTextureSize_method;
        static jmethodID s_getColorCorrection_method;
        static jmethodID s_getEnvironmentalHdrMainLightDirection_method;
        static jmethodID s_getEnvironmentalHdrMainLightIntensity_method;
        static jmethodID s_getEnvironmentalHdrCubemap_method;
        static jmethodID s_getLightTimeStamp_method;
        static jmethodID s_getPredictedViewMatrix_method;
        static jmethodID s_getProjectionMatrixSeeThrough_method;
        static jmethodID s_setWindow_method;
        static jmethodID s_beginRenderEye_method;
        static jmethodID s_endRenderEye_method;
        static jmethodID s_submitFrame_method;
        static jmethodID s_setReferenceObjectDatabase_method;
        static jmethodID s_getObjectTrackingInfo_method;
        static jmethodID s_getCameraViewMatrix_method;
        static jmethodID s_getObjectCenterPose_method;
        static jmethodID s_getExtentX_method;
        static jmethodID s_getExtentY_method;
        static jmethodID s_getExtentZ_method;
        static jmethodID s_getBoundingBox_method;
        static jmethodID s_getAllObjectId_method;
        static jclass s_class;
        static std::string s_class_name;
        static jmethodID s_getRegionInfo_method;
    };

    class CImageNodeMgr;
    class MarkerAlgorithmProxyWrapper : public JavaClassWrapper {
    public:
        MarkerAlgorithmProxyWrapper(jobject obj, bool is_global);
        ~MarkerAlgorithmProxyWrapper(){}
        static void init();
        void setReferenceImageDatabase(CImageNodeMgr& database);
        void getCenterPose(int marker_id, float* view_matrix, int screenRotate, float* pose_ptr);
        void get2DCorners(int marker_id, float* corners);
        float getExtentX(int marker_id);
        float getExtentY(int marker_id);
        float getExtentZ(int marker_id);
        std::vector<int> getAllMarkerId();
    private:
        static jmethodID s_setReferenceImageDatabase_method;
        static jmethodID s_getCenterPose_method;
        static jmethodID s_get2DCorners_method;
        static jmethodID s_getExtentX_method;
        static jmethodID s_getExtentY_method;
        static jmethodID s_getExtentZ_method;
        static jmethodID s_getAllMarkerId_method;
        static jclass s_class;
        static std::string s_class_name;
    };

    //class CReferenceObjectDatabase;
    class ObjectTrackingAlgorithmProxyWrapper : public JavaClassWrapper {
    public:
        ObjectTrackingAlgorithmProxyWrapper(jobject obj, bool is_global);
        ~ObjectTrackingAlgorithmProxyWrapper(){}
        static void init();
        void setReferenceObjectDatabase(CReferenceObjectDatabase& database);
        std::string getObjectTrackingInfo();
        void getCameraViewMatrix(float* view, int screen_rotate);
        void getCenterPose(int marker_id, float* pose_ptr);
        float getExtentX(int marker_id);
        float getExtentY(int marker_id);
        float getExtentZ(int marker_id);
        void getBoundingBox(int object_id, float* boundingBox);
        std::vector<int> getAllObjectId();
    private:
        static jmethodID s_setReferenceObjectDatabase_method;
        static jmethodID s_getObjectTrackingInfo_method;
        static jmethodID s_getCameraViewMatrix_method;
        static jmethodID s_getCenterPose_method;
        static jmethodID s_getExtentX_method;
        static jmethodID s_getExtentY_method;
        static jmethodID s_getExtentZ_method;
        static jmethodID s_getBoundingBox_method;
        static jmethodID s_getAllObjectId_method;
        static jclass s_class;
        static std::string s_class_name;
    };

    class HandGestureAlgorithmProxyWrapper : public JavaClassWrapper {
    public:
        HandGestureAlgorithmProxyWrapper(jobject obj, bool is_global);
        ~HandGestureAlgorithmProxyWrapper() {}
        static void init();
        std::vector<int> getAllHandGestureId();
        int32_t getHandSide(int32_t hand_id);
        int32_t getHandGestureType(int32_t hand_id);
        int32_t getHandToward(int32_t hand_id);
        float_t getHandTypeConfidence(int32_t hand_id);
        int32_t getLandMark2DCount(int32_t hand_id);
        void getLandMark2DArray(int32_t handId, float *out_landMark2DArray);
    private:
        static jclass s_class;
        static std::string s_class_name;
        static jmethodID s_getAllHandGestureId_method;
        static jmethodID s_getHandSide_method;
        static jmethodID s_getHandType_method;
        static jmethodID s_getHandToward_method;
        static jmethodID s_getHandTypeConfidence_method;
        static jmethodID s_getLandMark2DCount_method;
        static jmethodID s_getLandMark2DArray_method;
    };


    class FaceMeshAlgorithmProxyWrapper : public JavaClassWrapper {
    public:
        FaceMeshAlgorithmProxyWrapper(jobject obj, bool is_global);
        ~FaceMeshAlgorithmProxyWrapper() {}
        static void init();
        std::vector<int> getAllFaceMeshId();
        int32_t getVertexCount(int32_t face_id);
        int32_t getIndexCount(int32_t face_id);
        int32_t getTextureCoordinateCount(int32_t face_id);
        int32_t getNormalCount(int32_t face_id);
        float*  getVertices(int32_t face_id);
        int32_t*  getIndices(int32_t face_id);
        float*  getTextureCoordinates(int32_t face_id);
        float*  getNormals(int32_t face_id);
        void  getPose(int32_t face_id,float* out_pose_ptr);
    private:
        static jclass s_class;
        static std::string s_class_name;
        static jmethodID s_getAllFaceMeshId_method;
        static jmethodID s_getVertexCount_method;
        static jmethodID s_getIndexCount_method;
        static jmethodID s_getTextureCoordinateCount_method;
        static jmethodID s_getNormalCount_method;
        static jmethodID s_getVertices_method;
        static jmethodID s_getIndices_method;
        static jmethodID s_getTextureCoordinates_method;
        static jmethodID s_getNormals_method;
        static jmethodID s_getPose_method;
    };


    class BodyAlgorithmProxyWrapper : public JavaClassWrapper {
    public:
        BodyAlgorithmProxyWrapper(jobject obj, bool is_global);
        ~BodyAlgorithmProxyWrapper() {}
        static void init();
        std::vector<int> getAllBodyId();
        int32_t getSkeletonPoint2dCount(int32_t body_id);
        void  getSkeletonPoint2d(int32_t body_id,float* out_point2d);
        void  getSkeletonPoint2dConfidence(int32_t body_id,float* out_point2d_confidence);
    private:
        static jclass s_class;
        static std::string s_class_name;
        static jmethodID s_getAllBodyId_method;
        static jmethodID s_getSkeletonPoint2dCount_method;
        static jmethodID s_getSkeletonPoint2d_method;
        static jmethodID s_getSkeletonPoint2dConfidence_method;
    };

}




#endif //HELLO_AR_C_SRC_DYNAMICLOADER_H
