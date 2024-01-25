#include <string>
#include "DynamicLoader.h"
#include "ArJniHelpler.h"
#include "ArCommon.h"
#include "ArImageNodeMgr.h"
#include "ArReferenceObjectDatabase.h"

using namespace standardar;
JavaVM* DynamicLoader::s_JVM = nullptr;
DexClassLoaderWrapper* DynamicLoader::s_DexClassLoader = nullptr;
jobject DynamicLoader::s_server_context = nullptr;
jobject DynamicLoader::s_asset_manager = nullptr;
bool DynamicLoader::s_bInit = false;
void DynamicLoader::init(JNIEnv* env, jobject context) {
    if(s_bInit)
        return;
    s_bInit = true;
    env->GetJavaVM(&s_JVM);
    ArJniHelpler::init(s_JVM, nullptr, nullptr);
    //default classpath init
    ContextWrapper::init();
    DexClassLoaderWrapper::init();
    FileClassWrapper::init();
    ApplicationInfoWrapper::init();
    ClassWrapper::init();
    PointWrapper::init();
    ActivityWrapper::init();
    //apk classpath int
    FrameworkConstantWrapper::init();
    ActivityWrapper activityWrapper(context, true);
    ContextWrapper sdkcontextWrapper(activityWrapper.getApplication(), true);
    int CONTEXT_IGNORE_SECURITY = 2;
    int CONTEXT_INCLUDE_CODE = 1;

    s_server_context = env->NewGlobalRef(sdkcontextWrapper.createPackageContext(FrameworkConstantWrapper::getARSERVICE_PACKAGE_NAME(), CONTEXT_IGNORE_SECURITY | CONTEXT_INCLUDE_CODE));
    ContextWrapper serverContextWrapper(s_server_context, true);
    s_asset_manager = env->NewGlobalRef(serverContextWrapper.getAssets());
    ApplicationInfoWrapper serverApplicationInfoWrapper(serverContextWrapper.getApplicationInfo(), false);

    jobject dir_file = sdkcontextWrapper.getDir("dex", 0);
    FileClassWrapper fileClassWrapper(dir_file, false);
    jobject obj = sdkcontextWrapper.getClass();
    ClassWrapper classWrapper(obj, false);

    jobject dexclassloader = DexClassLoaderWrapper::construct(serverApplicationInfoWrapper.getSourceDir(),
                                                              fileClassWrapper.getAbsolutePath(),
                                                              serverApplicationInfoWrapper.getNativeLibrarDir(),
                                                              classWrapper.getClassLoader());
    s_DexClassLoader = new DexClassLoaderWrapper(env->NewGlobalRef(dexclassloader), true);

    //server classpath init
    CameraSourceWrapper::init();
    IMUReaderWrapper::init();
    ClientWrapper::init();
    SLAMAlgorithmProxyWrapper::init();
    MarkerAlgorithmProxyWrapper::init();
    HandGestureAlgorithmProxyWrapper::init();
    FaceMeshAlgorithmProxyWrapper::init();
    BodyAlgorithmProxyWrapper::init();
}

jclass DynamicLoader::findClassGlobalRef(std::string className) {
    JNIEnvProxy env(s_JVM);
    jclass clz = env->FindClass(className.c_str());
    return (jclass)env->NewGlobalRef(clz);
}

jmethodID DynamicLoader::getMethodId(jclass clz, std::string name, std::string sig) {
    JNIEnvProxy env(s_JVM);
    return env->GetMethodID(clz, name.c_str(), sig.c_str());
}

jmethodID DynamicLoader::getStaticMethodId(jclass clz, std::string name, std::string sig) {
    JNIEnvProxy env(s_JVM);
    return env->GetStaticMethodID(clz, name.c_str(), sig.c_str());
}

jfieldID DynamicLoader::getFieldId(jclass clz, std::string name, std::string sig) {
    JNIEnvProxy env(s_JVM);
    return env->GetFieldID(clz, name.c_str(), sig.c_str());
}

jfieldID DynamicLoader::getStaticFieldId(jclass clz, std::string name, std::string sig) {
    JNIEnvProxy env(s_JVM);
    return env->GetStaticFieldID(clz, name.c_str(), sig.c_str());
}


JavaClassWrapper::JavaClassWrapper(jobject obj, bool is_global) {
    m_obj = obj;
    m_global = is_global;
}

JavaClassWrapper::~JavaClassWrapper() {
    if (!m_global) {
        JNIEnvProxy env(DynamicLoader::s_JVM);
        env->DeleteLocalRef(m_obj);
    }
}

std::string ClassWrapper::s_class_name = "java/lang/Class";
jmethodID ClassWrapper::s_getClassLoader_method = nullptr;
jclass ClassWrapper::s_class = nullptr;
ClassWrapper::ClassWrapper(jobject obj , bool is_global) : JavaClassWrapper(obj, is_global){}
void ClassWrapper::init() {
    s_class = DynamicLoader::findClassGlobalRef(s_class_name);
    s_getClassLoader_method = DynamicLoader::getMethodId(s_class, "getClassLoader", "()Ljava/lang/ClassLoader;");
}

jobject ClassWrapper::getClassLoader() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallObjectMethod(m_obj, s_getClassLoader_method);
}

jmethodID ActivityWrapper::s_getApplication_method = nullptr;
std::string ActivityWrapper::s_class_name = "android/app/Activity";
jclass ActivityWrapper::s_class = nullptr;
ActivityWrapper::ActivityWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global){}
void ActivityWrapper::init() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    s_class = DynamicLoader::findClassGlobalRef(s_class_name);
    s_getApplication_method = DynamicLoader::getMethodId(s_class, "getApplication",
                                                         "()Landroid/app/Application;");
}

jobject ActivityWrapper::getApplication() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallObjectMethod(m_obj, s_getApplication_method);
}

jmethodID ContextWrapper::s_createPackageContext_method = nullptr;
jmethodID ContextWrapper::s_getAsset_method = nullptr;
jmethodID ContextWrapper::s_getApplicationInfo_method = nullptr;
jmethodID ContextWrapper::s_getDir_method = nullptr;
jmethodID ContextWrapper::s_getClass_method = nullptr;
jclass ContextWrapper::s_class = nullptr;
std::string ContextWrapper::s_class_name = "android/content/Context";
ContextWrapper::ContextWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global) {}

void ContextWrapper::init() {
    s_class = DynamicLoader::findClassGlobalRef(s_class_name);
    s_createPackageContext_method = DynamicLoader::getMethodId(s_class, "createPackageContext", "(Ljava/lang/String;I)Landroid/content/Context;");
    s_getAsset_method = DynamicLoader::getMethodId(s_class, "getAssets", "()Landroid/content/res/AssetManager;");
    s_getApplicationInfo_method = DynamicLoader::getMethodId(s_class, "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
    s_getDir_method = DynamicLoader::getMethodId(s_class, "getDir", "(Ljava/lang/String;I)Ljava/io/File;");
    s_getClass_method = DynamicLoader::getMethodId(s_class, "getClass", "()Ljava/lang/Class;");
}

jobject ContextWrapper::createPackageContext(std::string package_name, int flags) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jstring pkg = env->NewStringUTF(package_name.c_str());
    jobject obj = env->CallObjectMethod(m_obj, s_createPackageContext_method, pkg, flags);
    env->DeleteLocalRef(pkg);
    return obj;
}

jobject ContextWrapper::getAssets() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallObjectMethod(m_obj, s_getAsset_method);
}

jobject ContextWrapper::getApplicationInfo() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallObjectMethod(m_obj, s_getApplicationInfo_method);
}

jobject ContextWrapper::getDir(std::string name, int mode) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jstring name_str = env->NewStringUTF(name.c_str());
    jobject dir_file = env->CallObjectMethod(m_obj, s_getDir_method, name_str, mode);
    env->DeleteLocalRef(name_str);
    return dir_file;
}

jobject ContextWrapper::getClass() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jobject obj = env->CallObjectMethod(m_obj, s_getClass_method);
    return obj;
}

std::string DexClassLoaderWrapper::s_class_name = "dalvik/system/DexClassLoader";
jclass DexClassLoaderWrapper::s_class = nullptr;
jmethodID DexClassLoaderWrapper::s_loadClass_method = nullptr;
jmethodID DexClassLoaderWrapper::s_ctor_method = nullptr;
DexClassLoaderWrapper::DexClassLoaderWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global) {}

void DexClassLoaderWrapper::init() {
    s_class = DynamicLoader::findClassGlobalRef(s_class_name);
    s_loadClass_method = DynamicLoader::getMethodId(s_class, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
    s_ctor_method = DynamicLoader::getMethodId(s_class, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V");
}

jclass DexClassLoaderWrapper::loadClassGlobalRef(std::string class_name) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jstring class_name_jstr = env->NewStringUTF(class_name.c_str());
    jclass clz = static_cast<jclass>(env->CallObjectMethod(m_obj, s_loadClass_method, class_name_jstr));
    env->DeleteLocalRef(class_name_jstr);
    return (jclass)env->NewGlobalRef(clz);
}

jobject DexClassLoaderWrapper::construct(jstring dex_path, jstring opt_dir, jstring lib_path,
                                         jobject classloader) {

    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->NewObject(s_class, s_ctor_method, dex_path, opt_dir, lib_path, classloader);
}

std::string FileClassWrapper::s_class_name = "java/io/File";
jmethodID FileClassWrapper::s_getAbsolutePath_method = nullptr;
jclass FileClassWrapper::s_class = nullptr;
FileClassWrapper::FileClassWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global) {}

void FileClassWrapper::init() {
    s_class = DynamicLoader::findClassGlobalRef(s_class_name);
    s_getAbsolutePath_method = DynamicLoader::getMethodId(s_class, "getAbsolutePath", "()Ljava/lang/String;");
}

jstring FileClassWrapper::getAbsolutePath() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return static_cast<jstring>(env->CallObjectMethod(m_obj, s_getAbsolutePath_method));
}

std::string ApplicationInfoWrapper::s_class_name = "android/content/pm/ApplicationInfo";
jfieldID ApplicationInfoWrapper::s_nativeLibraryDir_filed = nullptr;
jfieldID ApplicationInfoWrapper::s_sourceDir_field = nullptr;
jclass ApplicationInfoWrapper::s_class = nullptr;

ApplicationInfoWrapper::ApplicationInfoWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global) {}

void ApplicationInfoWrapper::init() {
    s_class = DynamicLoader::findClassGlobalRef(s_class_name);
    s_nativeLibraryDir_filed = DynamicLoader::getFieldId(s_class, "nativeLibraryDir", "Ljava/lang/String;");
    s_sourceDir_field = DynamicLoader::getFieldId(s_class, "sourceDir", "Ljava/lang/String;");
}

jstring ApplicationInfoWrapper::getNativeLibrarDir() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return static_cast<jstring>(env->GetObjectField(m_obj, s_nativeLibraryDir_filed));
}

jstring ApplicationInfoWrapper::getSourceDir() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return static_cast<jstring>(env->GetObjectField(m_obj, s_sourceDir_field));
}


jfieldID PointWrapper::s_x_field = nullptr;
jfieldID PointWrapper::s_y_field = nullptr;
jclass PointWrapper::s_class = nullptr;
std::string PointWrapper::s_class_name = "android/graphics/Point";
PointWrapper::PointWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global){}

void PointWrapper::init() {
    s_class = DynamicLoader::findClassGlobalRef(s_class_name);
    s_x_field = DynamicLoader::getFieldId(s_class, "x", "I");
    s_y_field = DynamicLoader::getFieldId(s_class, "y", "I");
}

int PointWrapper::getX() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->GetIntField(m_obj, s_x_field);
}

int PointWrapper::getY() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->GetIntField(m_obj, s_y_field);
}

jfieldID FrameworkConstantWrapper::s_ARSERVICE_PACKAGE_NAME = nullptr;
jclass FrameworkConstantWrapper::s_class = nullptr;
std::string FrameworkConstantWrapper::s_class_name = "com/standardar/common/FrameworkConstant";
void FrameworkConstantWrapper::init() {
    s_class = DynamicLoader::findClassGlobalRef(s_class_name);
    s_ARSERVICE_PACKAGE_NAME = DynamicLoader::getStaticFieldId(s_class, "ARSERVICE_PACKAGE_NAME", "Ljava/lang/String;");
}

std::string FrameworkConstantWrapper::getARSERVICE_PACKAGE_NAME() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jstring str = static_cast<jstring>(env->GetStaticObjectField(s_class, s_ARSERVICE_PACKAGE_NAME));
    jboolean is_copy;
    const char* str_ptr = env->GetStringUTFChars(str, &is_copy);
    std::string res = str_ptr;
    env->ReleaseStringUTFChars(str, str_ptr);
    return res;
}
jmethodID CameraSourceWrapper::s_setTextureId_method = nullptr;
jmethodID CameraSourceWrapper::s_startPreview_method = nullptr;
jmethodID CameraSourceWrapper::s_stopPreview_method = nullptr;
jmethodID CameraSourceWrapper::s_setCameraNotify_method = nullptr;
jmethodID CameraSourceWrapper::s_setRenderCamera_method = nullptr;
jmethodID CameraSourceWrapper::s_openCamera_method = nullptr;
jmethodID CameraSourceWrapper::s_setPreviewSize_method = nullptr;
jmethodID CameraSourceWrapper::s_closeCamera_method = nullptr;
jmethodID CameraSourceWrapper::s_setScreenRotate_method = nullptr;
jmethodID CameraSourceWrapper::s_updateTexture_method = nullptr;
jmethodID CameraSourceWrapper::s_waitForNextFrame_method = nullptr;
jmethodID CameraSourceWrapper::s_getFovH_method = nullptr;
jmethodID CameraSourceWrapper::s_getFovV_method = nullptr;
jmethodID CameraSourceWrapper::s_getSupportSizeString_method = nullptr;
jmethodID CameraSourceWrapper::s_getDisplaySize_method = nullptr;
jmethodID CameraSourceWrapper::s_getCurImageTimestamp_method = nullptr ;
jmethodID CameraSourceWrapper::s_getCurImageWidth_method = nullptr;
jmethodID CameraSourceWrapper::s_getCurImageHeight_method = nullptr;
jmethodID CameraSourceWrapper::s_getCurImageStride_method = nullptr;
jmethodID CameraSourceWrapper::s_getCurImageFormat_method = nullptr;
jmethodID CameraSourceWrapper::s_getCurImageData_method = nullptr;
jmethodID CameraSourceWrapper::s_isImageBufferDirty_method = nullptr;
jmethodID CameraSourceWrapper::s_getInstance_method = nullptr;
jmethodID CameraSourceWrapper::s_init_method = nullptr;
jmethodID CameraSourceWrapper::s_getPreviewSize_method = nullptr;
jmethodID CameraSourceWrapper::s_getProcessSize_method = nullptr;
jclass CameraSourceWrapper::s_class = nullptr;
std::string CameraSourceWrapper::s_class_name = "com/standardar/sensor/camera/CameraSource";
CameraSourceWrapper::CameraSourceWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global){}

void CameraSourceWrapper::init() {
    DexClassLoaderWrapper* dex = DynamicLoader::s_DexClassLoader;
    s_class = dex->loadClassGlobalRef(s_class_name);
    s_setTextureId_method = DynamicLoader::getMethodId(s_class, "setTextureId", "(I)V");
    s_startPreview_method = DynamicLoader::getMethodId(s_class, "startPreview", "()V");
    s_stopPreview_method = DynamicLoader::getMethodId(s_class, "stopPreview", "()V");
    s_setCameraNotify_method = DynamicLoader::getMethodId(s_class, "setCameraNotify",
                                                          "(Lcom/standardar/sensor/camera/CameraSource$ICameraNotifyCallback;)V");
    s_setRenderCamera_method = DynamicLoader::getMethodId(s_class, "setRenderCamera", "(I)V");
    s_openCamera_method = DynamicLoader::getMethodId(s_class, "openCamera", "([I)V");
    s_setPreviewSize_method = DynamicLoader::getMethodId(s_class, "setPreviewSize", "(II)V");
    s_closeCamera_method = DynamicLoader::getMethodId(s_class, "closeCamera", "()V");
    s_setScreenRotate_method = DynamicLoader::getMethodId(s_class, "setScreenRotate", "(I)V");
    s_updateTexture_method = DynamicLoader::getMethodId(s_class, "updateTexture", "()V");
    s_waitForNextFrame_method = DynamicLoader::getMethodId(s_class, "waitForNextFrame", "()V");
    s_getFovH_method = DynamicLoader::getMethodId(s_class, "getFovH", "()F");
    s_getFovV_method = DynamicLoader::getMethodId(s_class, "getFovV", "()F");
    s_getSupportSizeString_method = DynamicLoader::getMethodId(s_class, "getSupportSizeString", "()Ljava/lang/String;");
    s_getDisplaySize_method = DynamicLoader::getMethodId(s_class, "getDisplaySize", "()Landroid/graphics/Point;");
    s_getPreviewSize_method = DynamicLoader::getMethodId(s_class, "getPreviewSize", "()Landroid/graphics/Point;");
    s_getProcessSize_method = DynamicLoader::getMethodId(s_class, "getProcessSize", "()Landroid/graphics/Point;");
    s_getCurImageTimestamp_method = DynamicLoader::getMethodId(s_class, "getCurImageTimestamp", "()J");
    s_getCurImageWidth_method = DynamicLoader::getMethodId(s_class, "getCurImageWidth", "()I");
    s_getCurImageHeight_method = DynamicLoader::getMethodId(s_class, "getCurImageHeight", "()I");
    s_getCurImageStride_method = DynamicLoader::getMethodId(s_class, "getCurImageStride", "()I");
    s_getCurImageFormat_method = DynamicLoader::getMethodId(s_class, "getCurImageFormat", "()I");
    s_getCurImageData_method = DynamicLoader::getMethodId(s_class, "getCurImageData", "(JJJ)Z");
    s_isImageBufferDirty_method = DynamicLoader::getMethodId(s_class, "isImageBufferDirty", "()Z");
    s_getInstance_method = DynamicLoader::getStaticMethodId(s_class, "getInstance", "()Lcom/standardar/sensor/camera/CameraSource;");
    s_init_method = DynamicLoader::getMethodId(s_class, "init", "(Landroid/content/Context;)V");
}

void CameraSourceWrapper::setTextureId(int id) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setTextureId_method, id);
}

void CameraSourceWrapper::startPreview() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_startPreview_method);
}

void CameraSourceWrapper::stopPreview() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_stopPreview_method);
}

void CameraSourceWrapper::setCameraNotify(jobject camera_notify) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setCameraNotify_method, camera_notify);
}

void CameraSourceWrapper::setRenderCamera(int id) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setRenderCamera_method, id);
}

void CameraSourceWrapper::openCamera(std::vector<int> &camera_ids) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jintArray int_array = env->NewIntArray(camera_ids.size());
    env->SetIntArrayRegion(int_array, 0, camera_ids.size(), &camera_ids[0]);
    env->CallVoidMethod(m_obj, s_openCamera_method, int_array);
    env->DeleteLocalRef(int_array);
}

void CameraSourceWrapper::setPreviewSize(int w, int h) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setPreviewSize_method, w, h);
}

void CameraSourceWrapper::closeCamera() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_closeCamera_method);
}

void CameraSourceWrapper::setScreenRotate(int screen_rotate) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setScreenRotate_method, screen_rotate);
}

void CameraSourceWrapper::updateTexture() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_updateTexture_method);
}

void CameraSourceWrapper::waitForNextFrame() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_waitForNextFrame_method);
}

jobject CameraSourceWrapper::getInstance(jobject context) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jobject obj = env->CallStaticObjectMethod(s_class, s_getInstance_method);
    env->CallVoidMethod(obj, s_init_method, context);
    return obj;
}

float CameraSourceWrapper::getFovH() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallFloatMethod(m_obj, s_getFovH_method);
}

float CameraSourceWrapper::getFovV() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallFloatMethod(m_obj, s_getFovV_method);
}

std::string CameraSourceWrapper::getSupportSizeString() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jstring jstr = static_cast<jstring>(env->CallObjectMethod(m_obj, s_getSupportSizeString_method));
    jboolean is_copy;
    const char *jstr_ptr = env->GetStringUTFChars(jstr, &is_copy);
    std::string temp = jstr_ptr;
    env->ReleaseStringUTFChars(jstr, jstr_ptr);
    return temp;
}

std::vector<int> CameraSourceWrapper::getDisplaySize() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jobject point_obj = env->CallObjectMethod(m_obj, s_getDisplaySize_method);
    PointWrapper pointWrapper(point_obj, false);
    std::vector<int> size;
    size.push_back(pointWrapper.getX());
    size.push_back(pointWrapper.getY());
    return size;
}

int64_t CameraSourceWrapper::getCurImageTimestamp() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallLongMethod(m_obj, s_getCurImageTimestamp_method);
}

int CameraSourceWrapper::getCurImageWidth() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getCurImageWidth_method);
}

int CameraSourceWrapper::getCurImageHeight() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getCurImageHeight_method);
}

int CameraSourceWrapper::getCurImageStride() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getCurImageStride_method);
}

int CameraSourceWrapper::getCurImageFormat() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getCurImageFormat_method);
}

bool CameraSourceWrapper::getCurImageData(unsigned char *y_data,
                                                     unsigned char *u_data, unsigned char *v_data) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallBooleanMethod(m_obj, s_getCurImageData_method, reinterpret_cast<jlong>(y_data), reinterpret_cast<jlong>(u_data), reinterpret_cast<jlong>(v_data));
}

bool CameraSourceWrapper::isImageBufferDirty() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallBooleanMethod(m_obj, s_isImageBufferDirty_method);
}

void CameraSourceWrapper::getPreviewSize(int &width, int &height) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jobject point_obj = env->CallObjectMethod(m_obj, s_getPreviewSize_method);
    PointWrapper pointWrapper(point_obj, false);
    width = pointWrapper.getX();
    height = pointWrapper.getY();
}

void CameraSourceWrapper::getProcessSize(int &width, int &height) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jobject point_obj = env->CallObjectMethod(m_obj, s_getProcessSize_method);
    PointWrapper pointWrapper(point_obj, false);
    width = pointWrapper.getX();
    height = pointWrapper.getY();
}

jmethodID IMUReaderWrapper::s_ctor_method = nullptr;
jmethodID IMUReaderWrapper::s_openSensor_method = nullptr;
jmethodID IMUReaderWrapper::s_start_method = nullptr;
jmethodID IMUReaderWrapper::s_stop_method = nullptr;
jmethodID IMUReaderWrapper::s_closeSensor_method = nullptr;
jmethodID IMUReaderWrapper::s_setSensorNotify_method = nullptr;
jclass IMUReaderWrapper::s_class = nullptr;
std::string IMUReaderWrapper::s_class_name = "com/standardar/sensor/imu/IMUReader";
IMUReaderWrapper::IMUReaderWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global){}

void IMUReaderWrapper::init() {
    s_class = DynamicLoader::s_DexClassLoader->loadClassGlobalRef(s_class_name);
    s_ctor_method = DynamicLoader::getMethodId(s_class, "<init>", "(Landroid/content/Context;)V");
    s_openSensor_method = DynamicLoader::getMethodId(s_class, "openSensor", "()V");
    s_start_method = DynamicLoader::getMethodId(s_class, "start", "(Landroid/content/Context;)V");
    s_stop_method = DynamicLoader::getMethodId(s_class, "stop", "()V");
    s_closeSensor_method = DynamicLoader::getMethodId(s_class, "closeSensor", "()V");
    s_setSensorNotify_method = DynamicLoader::getMethodId(s_class, "setSensorNotify",
            "(Lcom/standardar/sensor/imu/IMUReader$ISensorNotifyCallback;)V");
}

jobject IMUReaderWrapper::construct(jobject context) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->NewObject(s_class, s_ctor_method, context);
}

void IMUReaderWrapper::openSensor() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_openSensor_method);
}

void IMUReaderWrapper::start() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_start_method, DynamicLoader::s_server_context);
}

void IMUReaderWrapper::stop() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_stop_method);
}

void IMUReaderWrapper::closeSensor() {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_closeSensor_method);
}

void IMUReaderWrapper::setSensorNotify(jobject client) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setSensorNotify_method, client);
}

std::string ClientWrapper::s_class_name = "com/standardar/sdkclient/Client";
jmethodID ClientWrapper::s_ctor_method = nullptr;
jmethodID ClientWrapper::s_setPackageName_method = nullptr;
jmethodID ClientWrapper::s_setSLAMPlaneMode_method = nullptr;
jmethodID ClientWrapper::s_setCloudAnchorMode_method = nullptr;
jmethodID ClientWrapper::s_setSLAMLightEstimateMode_method = nullptr;
jmethodID ClientWrapper::s_changeLightEstimationMode_method = nullptr;
jmethodID ClientWrapper::s_setSLAMMode_method = nullptr;
jmethodID ClientWrapper::s_setBluetoothUUID_method = nullptr;
jmethodID ClientWrapper::s_setUrl_method = nullptr;
jmethodID ClientWrapper::s_setSLMWorldAlignmentMode_mothod = nullptr;
jmethodID ClientWrapper::s_setDenseReconMode_method = nullptr;
jmethodID ClientWrapper::s_setObjectTrackingMode_method = nullptr;
jmethodID ClientWrapper::s_setObjectScanningMode_method = nullptr;
jmethodID ClientWrapper::s_setAlgorithmStreamMode_method = nullptr;
jmethodID ClientWrapper::s_setDeviceType_method = nullptr;
jmethodID ClientWrapper::s_initAlgorithm_method = nullptr;
jmethodID ClientWrapper::s_stopAlgorithm_method = nullptr;
jmethodID ClientWrapper::s_startAlgorithm_method = nullptr;
jmethodID ClientWrapper::s_destroyAlgorithm_method = nullptr;
jmethodID ClientWrapper::s_hostAnchor_method = nullptr;
jmethodID ClientWrapper::s_resolveAnchor_method = nullptr;
jmethodID ClientWrapper::s_setUserJson_method = nullptr;
jmethodID ClientWrapper::s_enableAlgorithm_method = nullptr;
jmethodID ClientWrapper::s_disableAlgorithm_method = nullptr;
jmethodID ClientWrapper::s_pushKeyFrame_method = nullptr;
jmethodID ClientWrapper::s_runDenseRecon_method = nullptr;
jmethodID ClientWrapper::s_getSLAMResult_method = nullptr;
jmethodID ClientWrapper::s_setResumeFlag_method = nullptr;
jmethodID ClientWrapper::s_updateAlgorithmResult_method = nullptr;
jmethodID ClientWrapper::s_setAxisUpMode_method = nullptr;
jclass ClientWrapper::s_class = nullptr;

ClientWrapper::ClientWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global){}

void ClientWrapper::init() {
    DexClassLoaderWrapper* dexClassLoaderWrapper = DynamicLoader::s_DexClassLoader;
    s_class = dexClassLoaderWrapper->loadClassGlobalRef(s_class_name);
    s_ctor_method = DynamicLoader::getMethodId(s_class, "<init>", "(Landroid/content/Context;Landroid/content/Context;)V");
    s_setPackageName_method = DynamicLoader::getMethodId(s_class, "setPackageName", "(Landroid/content/Context;Ljava/lang/String;)V");
    s_setSLAMPlaneMode_method = DynamicLoader::getMethodId(s_class, "setSLAMPlaneMode", "(I)V");
    s_setCloudAnchorMode_method = DynamicLoader::getMethodId(s_class, "setCloudAnchorMode", "(I)V");
    s_setSLAMLightEstimateMode_method = DynamicLoader::getMethodId(s_class, "setSLAMLightEstimateMode", "(I)V");
    s_changeLightEstimationMode_method = DynamicLoader::getMethodId(s_class, "changeLightEstimationMode", "(I)V");
    s_setSLAMMode_method = DynamicLoader::getMethodId(s_class, "setSLAMMode", "(I)V");
    s_setBluetoothUUID_method = DynamicLoader::getMethodId(s_class, "setBluetoothUUID", "(Ljava/lang/String;)V");
    s_setUrl_method = DynamicLoader::getMethodId(s_class, "setUrl", "(Ljava/lang/String;)V");
    s_setSLMWorldAlignmentMode_mothod = DynamicLoader::getMethodId(s_class, "setSLAMWorldAligmentMode", "(I)V");
    s_setDenseReconMode_method = DynamicLoader::getMethodId(s_class, "setDenseReconMode", "(I)V");
    s_setObjectTrackingMode_method = DynamicLoader::getMethodId(s_class, "setObjectTrackingMode", "(I)V");
    s_setObjectScanningMode_method = DynamicLoader::getMethodId(s_class, "setObjectScanningMode", "(I)V");
    s_setAlgorithmStreamMode_method = DynamicLoader::getMethodId(s_class, "setAlgorithmStreamMode", "(II)V");
    s_setDeviceType_method = DynamicLoader::getMethodId(s_class, "setDeviceType", "(II)V");
    s_initAlgorithm_method = DynamicLoader::getMethodId(s_class, "initAlgorithm", "()V");
    s_stopAlgorithm_method = DynamicLoader::getMethodId(s_class, "stopAlgorithm", "(I)V");
    s_startAlgorithm_method = DynamicLoader::getMethodId(s_class, "startAlgorithm", "(I)V");
    s_destroyAlgorithm_method = DynamicLoader::getMethodId(s_class, "destroyAlgorithm", "()V");
    s_hostAnchor_method = DynamicLoader::getMethodId(s_class, "hostAnchor", "(Landroid/content/Context;I[FLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    s_resolveAnchor_method = DynamicLoader::getMethodId(s_class, "resolveAnchor", "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    s_setUserJson_method = DynamicLoader::getMethodId(s_class, "setUserJson", "(Ljava/lang/String;)V");
    s_enableAlgorithm_method = DynamicLoader::getMethodId(s_class, "enableAlgorithm", "(I)Lcom/standardar/algorithm/AlgorithmProxy;");
    s_disableAlgorithm_method = DynamicLoader::getMethodId(s_class, "disableAlgorithm", "(I)V");
    s_pushKeyFrame_method = DynamicLoader::getMethodId(s_class, "pushKeyFrame", "([BJII[D[D)V");
    s_runDenseRecon_method = DynamicLoader::getMethodId(s_class, "runDenseRecon", "()V");
    s_getSLAMResult_method = DynamicLoader::getMethodId(s_class, "getSLAMResult", "()V");
    s_setResumeFlag_method = DynamicLoader::getMethodId(s_class, "setResumeFlag", "()V");
    s_updateAlgorithmResult_method = DynamicLoader::getMethodId(s_class, "updateAlgorithmResult", "()V");
    s_setAxisUpMode_method = DynamicLoader::getMethodId(s_class, "setAxisUpMode", "(I)V");
}

jobject ClientWrapper::construct(jobject server_context, jobject sdk_context) {
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->NewObject(s_class, s_ctor_method, server_context, sdk_context);
}

void ClientWrapper::setPackageName(jobject context, const char *packageName) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setPackageName_method, context, env->NewStringUTF(packageName));
}

void ClientWrapper::setPlaneFindingMode(int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setSLAMPlaneMode_method, mode);
}

void ClientWrapper::setCloudAnchorMode(int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setCloudAnchorMode_method, mode);
}

void ClientWrapper::setLightingEstimateMode(int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setSLAMLightEstimateMode_method, mode);
}

void ClientWrapper::changeLightingEstimateMode(int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_changeLightEstimationMode_method, mode);
}

void ClientWrapper::setSLAMMode(int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setSLAMMode_method, mode);
}

void ClientWrapper::setBluetoothUUID(std::string uuid) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setBluetoothUUID_method, env->NewStringUTF(uuid.c_str()));
}

void ClientWrapper::setUrl(std::string url) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setUrl_method, env->NewStringUTF(url.c_str()));
}

void ClientWrapper::setWorldAligmentMode(int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setSLMWorldAlignmentMode_mothod, mode);
}

void ClientWrapper::setDenseReconMode(int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setDenseReconMode_method, mode);
}

void ClientWrapper::setObjectTrackingMode(int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setObjectTrackingMode_method, mode);
}

void ClientWrapper::setObjectScanningMode(int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setObjectScanningMode_method, mode);
}

void ClientWrapper::setAlgorithmStreamMode(int algorithmType, int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setAlgorithmStreamMode_method, algorithmType, mode);
}

void ClientWrapper::setDeviceType(int algorithmType, int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setDeviceType_method, algorithmType, mode);
}

void ClientWrapper::initAlgorithm() {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_initAlgorithm_method);
}

void ClientWrapper::stopAlgorithm(int type) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_stopAlgorithm_method, type);
}

void ClientWrapper::startAlgorithm(int type) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_startAlgorithm_method, type);
}

void ClientWrapper::destroyAlgorithm() {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_destroyAlgorithm_method);
}

void ClientWrapper::hostAnchor(jobject context, bool override, float *anchor, std::string mapid,
        std::string app_key, std::string app_secret) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    jint isOverride;
    if(override)
        isOverride = 1;
    else
        isOverride = 0;
    jfloatArray jpose = env->NewFloatArray(7);
    jfloat* posePtr = env->GetFloatArrayElements(jpose, JNI_FALSE);
    for(int i = 0; i < 7; i++)
        posePtr[i] = anchor[i];
    env->ReleaseFloatArrayElements(jpose, posePtr, JNI_FALSE);
    env->CallVoidMethod(m_obj, s_hostAnchor_method, context, isOverride, jpose, env->NewStringUTF(mapid.c_str()), env->NewStringUTF(app_key.c_str()), env->NewStringUTF(app_secret.c_str()));
}

void ClientWrapper::resolveAnchor(jobject context, std::string anchorid,
        std::string app_key, std::string app_secret) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    jstring anchorid_str = env->NewStringUTF(anchorid.c_str());
    jstring app_key_str = env->NewStringUTF(app_key.c_str());
    jstring app_secret_str = env->NewStringUTF(app_secret.c_str());
    env->CallVoidMethod(m_obj, s_resolveAnchor_method, context , anchorid_str, app_key_str, app_secret_str);
    env->DeleteLocalRef(anchorid_str);
    env->DeleteLocalRef(app_key_str);
    env->DeleteLocalRef(app_secret_str);
}

void ClientWrapper::setUserJson(std::string json) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    jstring json_str = env->NewStringUTF(json.c_str());
    env->CallVoidMethod(m_obj, s_setUserJson_method, json_str);
    env->DeleteLocalRef(json_str);
}

jobject ClientWrapper::enableAlgorithm(int type) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    return env->CallObjectMethod(m_obj, s_enableAlgorithm_method, type);
}

void ClientWrapper::disableAlgorithm(int type) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_disableAlgorithm_method, type);
}

void ClientWrapper::pushKeyFrame(char *imgBuff, uint64_t timestamp, int width, int height,
                                 double *rot, double *trans) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    jbyteArray imgBuffJava = env->NewByteArray(width * height);
    jdoubleArray rotJava = env->NewDoubleArray(9), transJava = env->NewDoubleArray(3);

    env->SetByteArrayRegion(imgBuffJava, 0, width * height, reinterpret_cast<jbyte*>(imgBuff));
    env->SetDoubleArrayRegion(rotJava, 0, 9, rot);
    env->SetDoubleArrayRegion(transJava, 0, 3, trans);

    env->CallVoidMethod(m_obj, s_pushKeyFrame_method, imgBuffJava,
                        (jlong)timestamp, width, height,
                        rotJava, transJava);

    jbyte *imgBuff0 = env->GetByteArrayElements(imgBuffJava, 0);
    jdouble *rotJava0 = env->GetDoubleArrayElements(rotJava, 0),
            *transJava0 = env->GetDoubleArrayElements(transJava, 0);
    env->ReleaseByteArrayElements(imgBuffJava, imgBuff0, 0);
    env->ReleaseDoubleArrayElements(rotJava, rotJava0, 0);
    env->ReleaseDoubleArrayElements(transJava, transJava0, 0);
    env->DeleteLocalRef(imgBuffJava);
    env->DeleteLocalRef(rotJava);
    env->DeleteLocalRef(transJava);
}


void ClientWrapper::runDenseRecon() {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_runDenseRecon_method);
}

void ClientWrapper::getSLAMResult() {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getSLAMResult_method);
}

void ClientWrapper::setResumeFlag() {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setResumeFlag_method);
}

void ClientWrapper::updateAlgorithmResult() {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_updateAlgorithmResult_method);
}

void ClientWrapper::setAxisUpMode(int mode) {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setAxisUpMode_method, mode);
}

std::string SLAMAlgorithmProxyWrapper::s_class_name = "com/standardar/service/slam/algorithm/SLAMLibLoadAlgorithmProxy";
jclass SLAMAlgorithmProxyWrapper::s_class = nullptr;
SLAMAlgorithmProxyWrapper::SLAMAlgorithmProxyWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global){}
jmethodID SLAMAlgorithmProxyWrapper::s_getCameraIntrinsics_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getTrackingState_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getSLAMInfo_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getDenseMeshIndex_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getDenseMeshIndexCount_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getSLAMPlaneIndex_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getViewMatrix_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getSLAMLightIntensity_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_insectPlaneWithSlamResult_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPlaneCenter_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getDenseMeshVertex_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getDenseMeshVertexCount_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getDenseMeshNormal_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getDenseMeshFormat_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getCenterPose_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getNumberLandMarks_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getLandMarks_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getDistanceFromCamera_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPlaneVertexCount_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPlaneVertexArray_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPlaneIndexCount_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPlaneIndexArray_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPlaneType_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPlaneNormal_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPolygonSize_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPolygon_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getExtent_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPolygon3DSize_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPolygon3D_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_isPoseInPolygon_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPlaneOriginPoint_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_isPoseInExtents_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_insectSurfaceMesh_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getAllPlaneId_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_isDepthImageAvaliable_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getDepthImage_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getDepthImageWidth_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getDepthImageHeight_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_setBoundingBox_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getBoundingBoxPointCloud_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getScanningResult_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getCloudResult_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getMapId_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getAnchorId_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getCloudAnchorPose_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getLightEstimateState_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getLightEstimateMode_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPixelIntensity_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getSphericalHarmonicLighting_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getEnvironmentTextureSize_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getColorCorrection_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getEnvironmentalHdrMainLightDirection_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getEnvironmentalHdrMainLightIntensity_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getEnvironmentalHdrCubemap_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getLightTimeStamp_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getPredictedViewMatrix_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getProjectionMatrixSeeThrough_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_setWindow_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_beginRenderEye_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_endRenderEye_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_submitFrame_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_setReferenceObjectDatabase_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getObjectTrackingInfo_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getCameraViewMatrix_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getObjectCenterPose_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getExtentX_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getExtentY_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getExtentZ_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getBoundingBox_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getAllObjectId_method = nullptr;
jmethodID SLAMAlgorithmProxyWrapper::s_getRegionInfo_method = nullptr;

void SLAMAlgorithmProxyWrapper::init() {
    DexClassLoaderWrapper* dexClassLoaderWrapper = DynamicLoader::s_DexClassLoader;
    s_class = dexClassLoaderWrapper->loadClassGlobalRef(s_class_name);
    s_getCameraIntrinsics_method = DynamicLoader::getMethodId(s_class, "getCameraIntrinsics", "()Ljava/nio/ByteBuffer;");
    s_getTrackingState_method = DynamicLoader::getMethodId(s_class, "getTrackingState", "()I");
    s_getSLAMInfo_method = DynamicLoader::getMethodId(s_class, "getSLAMInfo", "()Ljava/lang/String;");
    s_getDenseMeshIndex_method = DynamicLoader::getMethodId(s_class, "getDenseMeshIndex", "(J)V");
    s_getDenseMeshIndexCount_method = DynamicLoader::getMethodId(s_class, "getDenseMeshIndexCount", "()I");
    s_getSLAMPlaneIndex_method = DynamicLoader::getMethodId(s_class, "getSLAMPlaneIndex", "(I)I");
    s_getViewMatrix_method = DynamicLoader::getMethodId(s_class, "getViewMatrix","(JI)V");
    s_getSLAMLightIntensity_method = DynamicLoader::getMethodId(s_class, "getSLAMLightIntensity", "()F");
    s_insectPlaneWithSlamResult_method = DynamicLoader::getMethodId(s_class,
            "insectPlaneWithSlamResult", "([F[F[I[F)[F");
    s_getPlaneCenter_method = DynamicLoader::getMethodId(s_class, "getPlaneCenter", "(I)[F");
    s_getDenseMeshVertex_method = DynamicLoader::getMethodId(s_class, "getDenseMeshVertex", "(J)V");
    s_getDenseMeshVertexCount_method = DynamicLoader::getMethodId(s_class, "getDenseMeshVertexCount", "()I");
    s_getDenseMeshNormal_method = DynamicLoader::getMethodId(s_class, "getDenseMeshNormal", "(J)V");
    s_getDenseMeshFormat_method = DynamicLoader::getMethodId(s_class, "getDenseMeshFormat", "()I");
    s_getCenterPose_method = DynamicLoader::getMethodId(s_class, "getCenterPose", "(I)[F");
    s_insectSurfaceMesh_method = DynamicLoader::getMethodId(s_class, "insectSurfaceMesh", "(FF[FFI)[F");
    s_getNumberLandMarks_method = DynamicLoader::getMethodId(s_class, "getNumberLandMarks", "()I");
    s_getLandMarks_method = DynamicLoader::getMethodId(s_class, "getLandMarks", "()J");
    s_getDistanceFromCamera_method = DynamicLoader::getMethodId(s_class, "getDistanceFromCamera", "([F)F");
    s_getPlaneVertexCount_method = DynamicLoader::getMethodId(s_class, "getPlaneVertexCount", "()I");
    s_getPlaneVertexArray_method = DynamicLoader::getMethodId(s_class, "getPlaneVertexArray", "(J)V");
    s_getPlaneIndexCount_method = DynamicLoader::getMethodId(s_class, "getPlaneIndexCount", "()I");
    s_getPlaneIndexArray_method = DynamicLoader::getMethodId(s_class, "getPlaneIndexArray", "(J)V");
    s_getPlaneType_method = DynamicLoader::getMethodId(s_class, "getPlaneType", "(I)I");
    s_getPlaneNormal_method = DynamicLoader::getMethodId(s_class, "getPlaneNormal", "(JI)V");
    s_getPolygonSize_method = DynamicLoader::getMethodId(s_class, "getPolygonSize", "(I)I");
    s_getPolygon_method = DynamicLoader::getMethodId(s_class, "getPolygon", "(JI)V");
    s_getExtent_method = DynamicLoader::getMethodId(s_class, "getExtent", "(II)F");
    s_getPolygon3DSize_method = DynamicLoader::getMethodId(s_class, "getPolygon3DSize", "(I)I");
    s_getPolygon3D_method = DynamicLoader::getMethodId(s_class, "getPolygon3D", "(JI)V");
    s_isPoseInPolygon_method = DynamicLoader::getMethodId(s_class, "isPoseInPolygon", "([FI)I");
    s_getPlaneOriginPoint_method = DynamicLoader::getMethodId(s_class, "getPlaneOriginPoint", "(I)[F");
    s_isPoseInExtents_method = DynamicLoader::getMethodId(s_class, "isPoseInExtents", "([FI)I");
    s_getAllPlaneId_method = DynamicLoader::getMethodId(s_class, "getAllPlaneId", "()[I");
    s_isDepthImageAvaliable_method = DynamicLoader::getMethodId(s_class, "isDepthImageAvaliable", "()Z");
    s_getDepthImage_method = DynamicLoader::getMethodId(s_class, "getDepthImage", "(J)Z");
    s_getDepthImageWidth_method = DynamicLoader::getMethodId(s_class, "getDepthImageWidth", "()I");
    s_getDepthImageHeight_method = DynamicLoader::getMethodId(s_class, "getDepthImageHeight", "()I");
    s_setBoundingBox_method = DynamicLoader::getMethodId(s_class, "setBoundingBox", "(JJ)V");
    s_getBoundingBoxPointCloud_method = DynamicLoader::getMethodId(s_class, "getScanningBoundingBoxPointCloud", "(JJ)V");
    s_getScanningResult_method = DynamicLoader::getMethodId(s_class, "getScanningResult", "(JJ)V");
    s_getCloudResult_method = DynamicLoader::getMethodId(s_class, "getCloudResult", "()I");
    s_getMapId_method = DynamicLoader::getMethodId(s_class, "getMapId", "()Ljava/lang/String;");
    s_getAnchorId_method = DynamicLoader::getMethodId(s_class, "getAnchorId", "()Ljava/lang/String;");
    s_getCloudAnchorPose_method = DynamicLoader::getMethodId(s_class, "getCloudAnchorPose", "()[F");
    s_getLightEstimateState_method = DynamicLoader::getMethodId(s_class, "getLightEstimateState", "()I");
    s_getLightEstimateMode_method = DynamicLoader::getMethodId(s_class, "getLightEstimateMode", "()I");
    s_getPixelIntensity_method = DynamicLoader::getMethodId(s_class, "getPixelIntensity", "()F");
    s_getSphericalHarmonicLighting_method = DynamicLoader::getMethodId(s_class, "getSphericalHarmonicLighting", "()[F");
    s_getEnvironmentTextureSize_method = DynamicLoader::getMethodId(s_class, "getEnvironmentTextureSize", "()[I");
    s_getColorCorrection_method = DynamicLoader::getMethodId(s_class, "getColorCorrection", "()[F");
    s_getEnvironmentalHdrMainLightDirection_method = DynamicLoader::getMethodId(s_class, "getEnvironmentalHdrMainLightDirection", "()[F");
    s_getEnvironmentalHdrMainLightIntensity_method = DynamicLoader::getMethodId(s_class, "getEnvironmentalHdrMainLightIntensity", "()[F");
    s_getEnvironmentalHdrCubemap_method = DynamicLoader::getMethodId(s_class, "getEnvironmentalHdrCubemap", "()[F");
    s_getLightTimeStamp_method = DynamicLoader::getMethodId(s_class, "getLightTimeStamp", "()J");
    s_getPredictedViewMatrix_method = DynamicLoader::getMethodId(s_class, "getPredictedViewMatrix", "()[F");
    s_getProjectionMatrixSeeThrough_method = DynamicLoader::getMethodId(s_class, "getProjectionMatrixSeeThrough", "(FF)[F");
    s_setWindow_method = DynamicLoader::getMethodId(s_class, "setWindow", "(JJZ)V");
    s_beginRenderEye_method = DynamicLoader::getMethodId(s_class, "beginRenderEye", "(I)V");
    s_endRenderEye_method = DynamicLoader::getMethodId(s_class, "endRenderEye", "(I)V");
    s_submitFrame_method = DynamicLoader::getMethodId(s_class, "submitFrame", "(II)V");
    s_setReferenceObjectDatabase_method = DynamicLoader::getMethodId(s_class, "setReferenceObjectDatabase", "(Ljava/nio/ByteBuffer;)V");
    s_getObjectTrackingInfo_method = DynamicLoader::getMethodId(s_class, "getObjectTrackingInfo", "()Ljava/lang/String;");
    s_getCameraViewMatrix_method = DynamicLoader::getMethodId(s_class, "getViewMatrix","(JI)V");
    s_getObjectCenterPose_method = DynamicLoader::getMethodId(s_class, "getObjectCenterPose", "(IJ)V");
    s_getExtentX_method = DynamicLoader::getMethodId(s_class, "getExtentX", "(I)F");
    s_getExtentY_method = DynamicLoader::getMethodId(s_class, "getExtentY", "(I)F");
    s_getExtentZ_method = DynamicLoader::getMethodId(s_class, "getExtentZ", "(I)F");
    s_getBoundingBox_method = DynamicLoader::getMethodId(s_class, "getBoundingBox", "(IJ)V");
    s_getAllObjectId_method = DynamicLoader::getMethodId(s_class, "getAllObjectId", "()[I");
    s_getRegionInfo_method = DynamicLoader::getMethodId(s_class, "getRegionInfo", "()Ljava/lang/String;");
}

CCameraIntrinsics SLAMAlgorithmProxyWrapper::getCameraIntrinsics() {
    CCameraIntrinsics cameraIntrinsics;
    if (m_obj == nullptr) {
        return cameraIntrinsics;
    }
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jobject buffer = env->CallObjectMethod(m_obj, s_getCameraIntrinsics_method);
    if (buffer == nullptr)
        return cameraIntrinsics;
    DirectByteBufferWrapper result_buffer(env->GetDirectBufferAddress(buffer),
                                          env->GetDirectBufferCapacity(buffer),
                                          BUFFER_LITTLER_ENDIAN);
    result_buffer.get<int>();
    result_buffer.get<int>();

    cameraIntrinsics.m_ImageWidth = result_buffer.get<int>();
    cameraIntrinsics.m_ImageHeight = result_buffer.get<int>();
    cameraIntrinsics.m_FocalLengthX = result_buffer.get<float>();
    cameraIntrinsics.m_FocalLengthY = result_buffer.get<float>();
    cameraIntrinsics.m_PrincipalPTX = result_buffer.get<float>();
    cameraIntrinsics.m_PrincipalPTY = result_buffer.get<float>();
    cameraIntrinsics.m_Distortion = result_buffer.get<float*>();

    cameraIntrinsics.m_ImageWidthSlv = result_buffer.get<int>();
    cameraIntrinsics.m_ImageHeightSlv = result_buffer.get<int>();
    cameraIntrinsics.m_FocalLengthSlvX = result_buffer.get<float>();
    cameraIntrinsics.m_FocalLengthSlvY = result_buffer.get<float>();
    cameraIntrinsics.m_PrincipalPTSlvX = result_buffer.get<float>();
    cameraIntrinsics.m_PrincipalPTSlvY = result_buffer.get<float>();
    cameraIntrinsics.m_DistortionSlv = result_buffer.get<float*>();

    return cameraIntrinsics;
}

int SLAMAlgorithmProxyWrapper::getTrackingState() {
    if (m_obj == nullptr)
        return -1;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getTrackingState_method);
}

std::string SLAMAlgorithmProxyWrapper::getSLAMInfo() {
    if (m_obj == nullptr)
        return "";
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jstring info = (jstring)env->CallObjectMethod(m_obj, s_getSLAMInfo_method);
    jboolean is_copy;
    const char* info_ptr = env->GetStringUTFChars(info, &is_copy);
    std::string slam_info = info_ptr;
    env->ReleaseStringUTFChars(info, info_ptr);
    env->DeleteLocalRef(info);
    return slam_info;
}

int SLAMAlgorithmProxyWrapper::getCloudResult() {
    if (m_obj == nullptr)
        return -1;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getCloudResult_method);
}

std::string SLAMAlgorithmProxyWrapper::getMapId() {
    if (m_obj == nullptr)
        return "";
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jstring jmapId = (jstring)env->CallObjectMethod(m_obj, s_getMapId_method);
    const char* mapId = env->GetStringUTFChars(jmapId, JNI_FALSE);
    std::string result = mapId;
    env->ReleaseStringUTFChars(jmapId, mapId);
    return result;
}

std::string SLAMAlgorithmProxyWrapper::getAnchorId() {
    if (m_obj == nullptr)
        return "";
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jstring janchorId = (jstring)env->CallObjectMethod(m_obj, s_getAnchorId_method);
    const char* anchorId = env->GetStringUTFChars(janchorId, JNI_FALSE);
    std::string result = anchorId;
    env->ReleaseStringUTFChars(janchorId, anchorId);
    return result;
}

void SLAMAlgorithmProxyWrapper::getCloudAnchorPose(float* pose_out) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray jpose = (jfloatArray)env->CallObjectMethod(m_obj, s_getCloudAnchorPose_method);
    jfloat* pose = env->GetFloatArrayElements(jpose, JNI_FALSE);
    memcpy(pose_out, pose, sizeof(float) * 7);
    env->ReleaseFloatArrayElements(jpose, pose, JNI_FALSE);
    return;
}

void SLAMAlgorithmProxyWrapper::getDenseMeshIndex(unsigned short *index_array) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getDenseMeshIndex_method, reinterpret_cast<jlong>(index_array));
}

int32_t SLAMAlgorithmProxyWrapper::getDenseMeshIndexCount() {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getDenseMeshIndexCount_method);
}

int32_t SLAMAlgorithmProxyWrapper::getSLAMPlaneIndex(int32_t planeid) {
    if (m_obj == nullptr)
        return -1;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getSLAMPlaneIndex_method, planeid);
}

void SLAMAlgorithmProxyWrapper::getViewMatrix(float *view, int screen_rotate) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getViewMatrix_method, reinterpret_cast<jlong>(view),screen_rotate);
}

float SLAMAlgorithmProxyWrapper::getSLAMLightIntensity() {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallFloatMethod(m_obj, s_getSLAMLightIntensity_method);
}

vec3f SLAMAlgorithmProxyWrapper::insectPlaneWithSlamResult(standardar::vec3f ray_origin,
                                                           standardar::vec3f ray_direction,
                                                           int32_t &plane_id,
                                                           standardar::vec3f &quat) {
    if (m_obj == nullptr)
        return vec3f(0,0,0);
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray ray_origin_arr = vec3fToFloatArray(env.get(), ray_origin);
    jfloatArray ray_direction_arr = vec3fToFloatArray(env.get(), ray_direction);
    jintArray plane_id_arr = env->NewIntArray(1);
    jfloatArray quat_arr = env->NewFloatArray(3);
    jfloatArray res = (jfloatArray)env->CallObjectMethod(m_obj, s_insectPlaneWithSlamResult_method,
                                            ray_origin_arr, ray_direction_arr, plane_id_arr, quat_arr);
    vec3f insect;
    copyFloatArrayToVec3f(env.get(), res, insect);
    copyFloatArrayToVec3f(env.get(), quat_arr, quat);
    env->DeleteLocalRef(ray_origin_arr);
    env->DeleteLocalRef(ray_direction_arr);
    env->DeleteLocalRef(quat_arr);
    int plane_id_data[1];
    env->GetIntArrayRegion(plane_id_arr, 0, 1, plane_id_data);
    plane_id = plane_id_data[0];
    env->DeleteLocalRef(plane_id_arr);
    env->DeleteLocalRef(res);
    return insect;
}

vec3f SLAMAlgorithmProxyWrapper::getPlaneCenter(int plane_id) {
    if (m_obj == nullptr)
        return vec3f(0,0,0);
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray res = (jfloatArray)env->CallObjectMethod(m_obj, s_getPlaneCenter_method, plane_id);
    vec3f res_vec = floatArrayToVec3f(env.get(), res);
    env->DeleteLocalRef(res);
    return res_vec;
}

void SLAMAlgorithmProxyWrapper::getDenseMeshVertex(float *vertex_array) {
    if (m_obj == nullptr)
        return ;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getDenseMeshVertex_method, reinterpret_cast<jlong>(vertex_array));
}

int SLAMAlgorithmProxyWrapper::getDenseMeshVertexCount() {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getDenseMeshVertexCount_method);
}

void SLAMAlgorithmProxyWrapper::getDenseMeshNormal(float *vertex_normal) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getDenseMeshNormal_method, reinterpret_cast<jlong>(vertex_normal));
}

int SLAMAlgorithmProxyWrapper::getDenseMeshFormat() {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getDenseMeshFormat_method);
}

vec3f SLAMAlgorithmProxyWrapper::getCenterPose(int plane_id) {
    if (m_obj == nullptr)
        return vec3f(0,0,0);
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray pose_arr = (jfloatArray)env->CallObjectMethod(m_obj, s_getCenterPose_method, plane_id);
    vec3f pose = floatArrayToVec3f(env.get(), pose_arr);
    env->DeleteLocalRef(pose_arr);
    return pose;
}

vec3f SLAMAlgorithmProxyWrapper::insectSurfaceMesh(float x, float y, standardar::vec3f &normal, float screen_aspect, int screen_rotate) {
    if (m_obj == nullptr)
        return vec3f(0,0,0);
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray normal_arr = env->NewFloatArray(3);
    jfloatArray insect_arr = (jfloatArray)env->CallObjectMethod(m_obj, s_insectSurfaceMesh_method, x, y, normal_arr, screen_aspect, screen_rotate);
    copyFloatArrayToVec3f(env.get(), normal_arr, normal);
    vec3f insect;
    copyFloatArrayToVec3f(env.get(), insect_arr, insect);
    env->DeleteLocalRef(insect_arr);
    env->DeleteLocalRef(normal_arr);
    return insect;
}

int SLAMAlgorithmProxyWrapper::getNumberLandMarks() {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getNumberLandMarks_method);
}

float* SLAMAlgorithmProxyWrapper::getLandMarks() {
    if (m_obj == nullptr)
        return nullptr;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jlong ptr = env->CallLongMethod(m_obj, s_getLandMarks_method);
    return reinterpret_cast<float*>(ptr);
}

float SLAMAlgorithmProxyWrapper::getDistanceFromCamera(standardar::vec3f hit_pose) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray hit_pose_arr = vec3fToFloatArray(env.get(), hit_pose);
    float dist = env->CallFloatMethod(m_obj, s_getDistanceFromCamera_method, hit_pose_arr);
    env->DeleteLocalRef(hit_pose_arr);
    return dist;
}

int SLAMAlgorithmProxyWrapper::getPlaneVertexCount() {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getPlaneVertexCount_method);
}


void SLAMAlgorithmProxyWrapper::getPlaneVertexArray(float *vertext_array) {
    if (m_obj == nullptr)
        return ;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getPlaneVertexArray_method, reinterpret_cast<jlong>(vertext_array));
}

int SLAMAlgorithmProxyWrapper::getPlaneIndexCount() {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getPlaneIndexCount_method);
}

void SLAMAlgorithmProxyWrapper::getPlaneIndexArray(unsigned short *index_array) {
    if (m_obj == nullptr)
        return ;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getPlaneIndexArray_method, reinterpret_cast<jlong>(index_array));
}

int SLAMAlgorithmProxyWrapper::getPlaneType(int plane_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getPlaneType_method, plane_id);
}

void SLAMAlgorithmProxyWrapper::getPlaneNormal(float *out_plane_normal, int plane_id) {
    if (m_obj == nullptr)
        return ;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getPlaneNormal_method, reinterpret_cast<jlong>(out_plane_normal), plane_id);
}

int SLAMAlgorithmProxyWrapper::getPolygonSize(int plane_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getPolygonSize_method, plane_id);
}

void SLAMAlgorithmProxyWrapper::getPolygon(float *out_data, int plane_id) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getPolygon_method, reinterpret_cast<jlong>(out_data), plane_id);
}

float SLAMAlgorithmProxyWrapper::getExtent(int axis, int plane_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallFloatMethod(m_obj, s_getExtent_method, axis, plane_id);
}

int SLAMAlgorithmProxyWrapper::getPolygon3DSize(int plane_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getPolygon3DSize_method, plane_id);
}

void SLAMAlgorithmProxyWrapper::getPolygon3D(float *out_data, int plane_id) {
    if (m_obj == nullptr)
        return ;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getPolygon3D_method, reinterpret_cast<jlong>(out_data), plane_id);
}

int SLAMAlgorithmProxyWrapper::isPoseInPolygon(standardar::vec3f pose, int plane_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray pose_arr = vec3fToFloatArray(env.get(), pose);
    int res = env->CallIntMethod(m_obj, s_isPoseInPolygon_method, pose_arr, plane_id);
    env->DeleteLocalRef(pose_arr);
    return res;
}

vec3f SLAMAlgorithmProxyWrapper::getPlaneOriginPoint(int plane_id) {
    if (m_obj == nullptr)
        return vec3f(0,0,0);
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray res = (jfloatArray)env->CallObjectMethod(m_obj, s_getPlaneOriginPoint_method, plane_id);
    vec3f res_vec = floatArrayToVec3f(env.get(), res);
    env->DeleteLocalRef(res);
    return res_vec;
}

int SLAMAlgorithmProxyWrapper::isPoseInExtents(standardar::vec3f &p, int plane_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray p_arr = vec3fToFloatArray(env.get(), p);
    int res = env->CallIntMethod(m_obj, s_isPoseInExtents_method, p_arr, plane_id);
    env->DeleteLocalRef(p_arr);
    return res;
}

std::vector<int> SLAMAlgorithmProxyWrapper::getAllPlaneId() {
    if (m_obj == nullptr)
        return std::vector<int>();
    std::vector<int> res;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jintArray plane_ids = (jintArray)env->CallObjectMethod(m_obj, s_getAllPlaneId_method);
    if (plane_ids == nullptr)
        return res;
    int length = env->GetArrayLength(plane_ids);
    jboolean isCopy;
    jint* plane_id_ptr = env->GetIntArrayElements(plane_ids, &isCopy);
    for (int i = 0; i < length; ++i) {
        res.push_back(plane_id_ptr[i]);
    }
    env->ReleaseIntArrayElements(plane_ids, plane_id_ptr, 0);
    env->DeleteLocalRef(plane_ids);
    return res;
}

bool SLAMAlgorithmProxyWrapper::isDepthImageAvaliable()
{
    if (m_obj == nullptr)
        return false;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallBooleanMethod(m_obj, s_isDepthImageAvaliable_method);
}

bool SLAMAlgorithmProxyWrapper::getDepthImage(unsigned char* depth)
{
    if (m_obj == nullptr)
        return false;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallBooleanMethod(m_obj, s_getDepthImage_method, reinterpret_cast<jlong>(depth));
}

int SLAMAlgorithmProxyWrapper::getDepthImageWidth()
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallIntMethod(m_obj, s_getDepthImageWidth_method);
}

int SLAMAlgorithmProxyWrapper::getDepthImageHeight()
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallIntMethod(m_obj, s_getDepthImageHeight_method);
}

void SLAMAlgorithmProxyWrapper::setBoundingBox(const float* vertex, const float* matrix)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    env->CallVoidMethod(m_obj, s_setBoundingBox_method, reinterpret_cast<jlong>(vertex), reinterpret_cast<jlong>(matrix));
}

void SLAMAlgorithmProxyWrapper::getBoundingBoxPointCloud(float** points, int* points_num)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    env->CallVoidMethod(m_obj, s_getBoundingBoxPointCloud_method, reinterpret_cast<jlong>(points), reinterpret_cast<jlong>(points_num));
}

void SLAMAlgorithmProxyWrapper::getScanningResult(uint8_t **out_raw_bytes, int64_t *out_raw_bytes_size)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    env->CallVoidMethod(m_obj, s_getScanningResult_method, reinterpret_cast<jlong>(out_raw_bytes), reinterpret_cast<jlong>(out_raw_bytes_size));
}

ARIlluminationEstimateState SLAMAlgorithmProxyWrapper::getLightEstimateState() {
    if (m_obj == nullptr)
        return (ARIlluminationEstimateState)0;

    JNIEnvProxy env(DynamicLoader::s_JVM);
    jint state = env->CallIntMethod(m_obj, s_getLightEstimateState_method);
    return (ARIlluminationEstimateState)state;
}

ARIlluminationEstimateMode SLAMAlgorithmProxyWrapper::getLightEstimateMode() {
    if (m_obj == nullptr)
        return (ARIlluminationEstimateMode)1;

    JNIEnvProxy env(DynamicLoader::s_JVM);
    jint mode = env->CallIntMethod(m_obj, s_getLightEstimateMode_method);
    return (ARIlluminationEstimateMode)mode;
}

float SLAMAlgorithmProxyWrapper::getPixelIntensity() {
    if (m_obj == nullptr)
        return -1;

    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloat color = env->CallFloatMethod(m_obj, s_getPixelIntensity_method);
    return (float)color;
}

void SLAMAlgorithmProxyWrapper::getSphericalHarmonicLighting(float* out) {
    if (m_obj == nullptr)
        return;

    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray jspherical = (jfloatArray)env->CallObjectMethod(m_obj, s_getSphericalHarmonicLighting_method);
    if(jspherical == nullptr)
        return;
    jfloat* spherical = env->GetFloatArrayElements(jspherical, JNI_FALSE);
    memcpy(out, spherical, sizeof(float) * 27);
    env->ReleaseFloatArrayElements(jspherical, spherical, JNI_FALSE);
    env->DeleteLocalRef(jspherical);
    return;
}

void SLAMAlgorithmProxyWrapper::getEnvironmentTextureSize(int* out) {
    if (m_obj == nullptr)
        return;

    JNIEnvProxy env(DynamicLoader::s_JVM);
    jintArray jsize = (jintArray)env->CallObjectMethod(m_obj, s_getEnvironmentTextureSize_method);
    jint* size = env->GetIntArrayElements(jsize, JNI_FALSE);
    memcpy(out, size, sizeof(int) * 2);
    env->ReleaseIntArrayElements(jsize, size, JNI_FALSE);
    env->DeleteLocalRef(jsize);
    return;
}

void SLAMAlgorithmProxyWrapper::getColorCorrection(float* out) {
    if (m_obj == nullptr)
        return;

    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray jcolor = (jfloatArray)env->CallObjectMethod(m_obj, s_getColorCorrection_method);
    if(jcolor == nullptr)
        return;
    jfloat* color = env->GetFloatArrayElements(jcolor, JNI_FALSE);
    memcpy(out, color, sizeof(float) * 4);
    env->ReleaseFloatArrayElements(jcolor, color, JNI_FALSE);
    env->DeleteLocalRef(jcolor);
    return;
}

void SLAMAlgorithmProxyWrapper::getEnvironmentalHdrMainLightDirection(float* out) {
    if (m_obj == nullptr)
        return;

    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray jdirection = (jfloatArray)env->CallObjectMethod(m_obj, s_getEnvironmentalHdrMainLightDirection_method);
    if(jdirection == nullptr)
        return;
    jfloat* direction = env->GetFloatArrayElements(jdirection, JNI_FALSE);
    memcpy(out, direction, sizeof(float) * 3);
    env->ReleaseFloatArrayElements(jdirection, direction, JNI_FALSE);
    env->DeleteLocalRef(jdirection);
    return;
}

void SLAMAlgorithmProxyWrapper::getEnvironmentalHdrMainLightIntensity(float* out) {
    if (m_obj == nullptr)
        return;

    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray jintensity = (jfloatArray)env->CallObjectMethod(m_obj, s_getEnvironmentalHdrMainLightIntensity_method);
    if(jintensity == nullptr)
        return;
    jfloat* intensity = env->GetFloatArrayElements(jintensity, JNI_FALSE);
    memcpy(out, intensity, sizeof(float) * 3);
    env->ReleaseFloatArrayElements(jintensity, intensity, JNI_FALSE);
    env->DeleteLocalRef(jintensity);
}

void SLAMAlgorithmProxyWrapper::getEnvironmentalHdrCubemap(float* buffer) {
    if (m_obj == nullptr)
        return;

    JNIEnvProxy env(DynamicLoader::s_JVM);
    jintArray jsize = (jintArray)env->CallObjectMethod(m_obj, s_getEnvironmentTextureSize_method);
    jfloatArray jcubemap = (jfloatArray)env->CallObjectMethod(m_obj, s_getEnvironmentalHdrCubemap_method);
    if(jcubemap == nullptr)
        return;
    jfloat* cubemap = env->GetFloatArrayElements(jcubemap, JNI_FALSE);
    jint* size = env->GetIntArrayElements(jsize, JNI_FALSE);
    memcpy(buffer, cubemap, sizeof(float) * size[0] * size[1] * 6 * 3);
    env->ReleaseFloatArrayElements(jcubemap, cubemap, JNI_FALSE);
    env->ReleaseIntArrayElements(jsize, size, JNI_FALSE);
    env->DeleteLocalRef(jcubemap);
}

int64_t SLAMAlgorithmProxyWrapper::getLightTimeStamp() {
    if (m_obj == nullptr)
        return 0;

    JNIEnvProxy env(DynamicLoader::s_JVM);
    return (int64_t)env->CallLongMethod(m_obj, s_getLightTimeStamp_method);
}

void SLAMAlgorithmProxyWrapper::getPredictedViewMatrix(float *view_mat) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray view_res = (jfloatArray)env->CallObjectMethod(m_obj, s_getPredictedViewMatrix_method);
    jboolean isCopy;
    jfloat *view_arr = env->GetFloatArrayElements(view_res, &isCopy);
    memcpy(view_mat, view_arr, 16 * sizeof(float));
    env->ReleaseFloatArrayElements(view_res, view_arr, isCopy);
    env->DeleteLocalRef(view_res);
}

void SLAMAlgorithmProxyWrapper::getProjectionMatrixSeeThrough(float near, float far,
                                                              float *proj_mat) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jfloatArray proj_res = (jfloatArray)env->CallObjectMethod(m_obj,
        s_getProjectionMatrixSeeThrough_method, (jfloat)near, (jfloat)far);
    jboolean isCopy;
    jfloat *proj_arr = env->GetFloatArrayElements(proj_res, &isCopy);
    memcpy(proj_mat, proj_arr, 16 * sizeof(float));
    env->ReleaseFloatArrayElements(proj_res, proj_arr, isCopy);
}

void SLAMAlgorithmProxyWrapper::setWindow(void *native_window, void *app_context, bool newPbuffer) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_setWindow_method, (jlong)native_window, (jlong)app_context,
        (jboolean)newPbuffer);
}

void SLAMAlgorithmProxyWrapper::beginRenderEye(int eye_side) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_beginRenderEye_method, eye_side);
}

void SLAMAlgorithmProxyWrapper::endRenderEye(int eye_side) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_endRenderEye_method, eye_side);
}

void SLAMAlgorithmProxyWrapper::submitFrame(int left_tex_id, int right_tex_id) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_submitFrame_method, left_tex_id, right_tex_id);
}

void SLAMAlgorithmProxyWrapper::setReferenceObjectDatabase(CReferenceObjectDatabase& database)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    int size = sizeof(int) * 3 + sizeof(int) * 2 * database.getUpdateObjectNum() + database.getObjectDataSize();
    jbyte* buffer = new jbyte[size];
    database.getObjectData(buffer, size, BUFFER_LITTLER_ENDIAN);
    jobject byteBuffer = env->NewDirectByteBuffer(buffer, size);

    env->CallVoidMethod(m_obj, s_setReferenceObjectDatabase_method, byteBuffer);
}

std::string SLAMAlgorithmProxyWrapper::getObjectTrackingInfo()
{
    if (m_obj == nullptr)
        return "";
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jstring info = (jstring)env->CallObjectMethod(m_obj, s_getObjectTrackingInfo_method);
    jboolean is_copy;
    const char* info_ptr = env->GetStringUTFChars(info, &is_copy);
    std::string object_info = info_ptr;
    env->ReleaseStringUTFChars(info, info_ptr);
    env->DeleteLocalRef(info);
    return object_info;
}

void SLAMAlgorithmProxyWrapper::getCameraViewMatrix(float* view, int screen_rotate)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getCameraViewMatrix_method, reinterpret_cast<jlong>(view),screen_rotate);
}

void SLAMAlgorithmProxyWrapper::getObjectCenterPose(int object_id, float* pose_ptr)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    env->CallVoidMethod(m_obj, s_getObjectCenterPose_method, object_id, reinterpret_cast<jlong>(pose_ptr));
}

float SLAMAlgorithmProxyWrapper::getExtentX(int object_id)
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallFloatMethod(m_obj, s_getExtentX_method, object_id);
}

float SLAMAlgorithmProxyWrapper::getExtentY(int object_id)
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallFloatMethod(m_obj, s_getExtentY_method, object_id);
}

float SLAMAlgorithmProxyWrapper::getExtentZ(int object_id)
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallFloatMethod(m_obj, s_getExtentZ_method, object_id);
}

void SLAMAlgorithmProxyWrapper::getBoundingBox(int object_id, float* boundingBox)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallVoidMethod(m_obj, s_getBoundingBox_method, object_id, reinterpret_cast<jlong>(boundingBox));
}

std::vector<int> SLAMAlgorithmProxyWrapper::getAllObjectId()
{
    if (m_obj == nullptr)
        return std::vector<int>();
    std::vector<int> res;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jintArray object_ids = (jintArray)env->CallObjectMethod(m_obj, s_getAllObjectId_method);
    if (object_ids == nullptr)
        return res;
    int length = env->GetArrayLength(object_ids);
    jboolean isCopy;
    jint* object_id_ptr = env->GetIntArrayElements(object_ids, &isCopy);
    for (int i = 0; i < length; ++i) {
        res.push_back(object_id_ptr[i]);
    }
    env->ReleaseIntArrayElements(object_ids, object_id_ptr, 0);
    env->DeleteLocalRef(object_ids);
    return res;
}

std::string SLAMAlgorithmProxyWrapper::getRegionInfo() {
    JNIEnvProxy env = JNIEnvProxy(DynamicLoader::s_JVM);
    jstring info = (jstring)env->CallObjectMethod(m_obj, s_getRegionInfo_method);
    jboolean is_copy;
    const char* info_ptr = env->GetStringUTFChars(info, &is_copy);
    std::string object_info = info_ptr;
    env->ReleaseStringUTFChars(info, info_ptr);
    env->DeleteLocalRef(info);
    return object_info;
}

std::string MarkerAlgorithmProxyWrapper::s_class_name = "com/standardar/service/algorithm/marker/MarkerLibLoadAlgorithmProxy";
jclass MarkerAlgorithmProxyWrapper::s_class = nullptr;
MarkerAlgorithmProxyWrapper::MarkerAlgorithmProxyWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global){}
jmethodID MarkerAlgorithmProxyWrapper::s_setReferenceImageDatabase_method = nullptr;
jmethodID MarkerAlgorithmProxyWrapper::s_getCenterPose_method = nullptr;
jmethodID MarkerAlgorithmProxyWrapper::s_get2DCorners_method = nullptr;
jmethodID MarkerAlgorithmProxyWrapper::s_getExtentX_method = nullptr;
jmethodID MarkerAlgorithmProxyWrapper::s_getExtentY_method = nullptr;
jmethodID MarkerAlgorithmProxyWrapper::s_getExtentZ_method = nullptr;
jmethodID MarkerAlgorithmProxyWrapper::s_getAllMarkerId_method = nullptr;

void MarkerAlgorithmProxyWrapper::init() {
    DexClassLoaderWrapper* dexClassLoaderWrapper = DynamicLoader::s_DexClassLoader;
    s_class = dexClassLoaderWrapper->loadClassGlobalRef(s_class_name);
    s_setReferenceImageDatabase_method = DynamicLoader::getMethodId(s_class, "setReferenceImageDatabase", "(Ljava/nio/ByteBuffer;)V");
    s_getCenterPose_method = DynamicLoader::getMethodId(s_class, "getCenterPose", "(IJIJ)V");
    s_get2DCorners_method = DynamicLoader::getMethodId(s_class, "get2DCorners", "(IJ)V");
    s_getExtentX_method = DynamicLoader::getMethodId(s_class, "getExtentX", "(I)F");
    s_getExtentY_method = DynamicLoader::getMethodId(s_class, "getExtentY", "(I)F");
    s_getExtentZ_method = DynamicLoader::getMethodId(s_class, "getExtentZ", "(I)F");
    s_getAllMarkerId_method = DynamicLoader::getMethodId(s_class, "getAllMarkerId", "()[I");
}

void MarkerAlgorithmProxyWrapper::setReferenceImageDatabase(CImageNodeMgr& database)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    int size = sizeof(int) * 3 + (sizeof(int) * 6 + sizeof(float)) * database.getUpdateImageNum() + database.getImageDataSize();
    jbyte* buffer = new jbyte[size];
    database.getImageData(buffer, size, BUFFER_LITTLER_ENDIAN);
    jobject byteBuffer = env->NewDirectByteBuffer(buffer, size);

    env->CallVoidMethod(m_obj, s_setReferenceImageDatabase_method, byteBuffer);
}

void MarkerAlgorithmProxyWrapper::getCenterPose(int marker_id, float* view_matrix, int screenRotate, float* pose_ptr)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    env->CallVoidMethod(m_obj, s_getCenterPose_method, marker_id, reinterpret_cast<jlong>(view_matrix), screenRotate, reinterpret_cast<jlong>(pose_ptr));
}

void MarkerAlgorithmProxyWrapper::get2DCorners(int marker_id, float* corners)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    env->CallVoidMethod(m_obj, s_get2DCorners_method, marker_id, reinterpret_cast<jlong>(corners));
}

float MarkerAlgorithmProxyWrapper::getExtentX(int marker_id)
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallFloatMethod(m_obj, s_getExtentX_method, marker_id);
}

float MarkerAlgorithmProxyWrapper::getExtentY(int marker_id)
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallFloatMethod(m_obj, s_getExtentY_method, marker_id);
}

float MarkerAlgorithmProxyWrapper::getExtentZ(int marker_id)
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallFloatMethod(m_obj, s_getExtentZ_method, marker_id);
}

std::vector<int> MarkerAlgorithmProxyWrapper::getAllMarkerId()
{
    if (m_obj == nullptr)
        return std::vector<int>();
    std::vector<int> res;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    jintArray marker_ids = (jintArray)env->CallObjectMethod(m_obj, s_getAllMarkerId_method);
    if (marker_ids == nullptr)
        return res;
    int length = env->GetArrayLength(marker_ids);
    jboolean isCopy;
    jint* marker_id_ptr = env->GetIntArrayElements(marker_ids, &isCopy);
    for (int i = 0; i < length; ++i) {
        res.push_back(marker_id_ptr[i]);
    }
    env->ReleaseIntArrayElements(marker_ids, marker_id_ptr, 0);
    env->DeleteLocalRef(marker_ids);
    return res;
}

std::string HandGestureAlgorithmProxyWrapper::s_class_name = "com/standardar/service/handgesture/HandGestureLibLoadAlgorithmProxy";
jclass HandGestureAlgorithmProxyWrapper::s_class = nullptr;
HandGestureAlgorithmProxyWrapper::HandGestureAlgorithmProxyWrapper(jobject obj, bool is_global):JavaClassWrapper(obj,is_global) {}
jmethodID HandGestureAlgorithmProxyWrapper::s_getAllHandGestureId_method = nullptr;
jmethodID HandGestureAlgorithmProxyWrapper::s_getHandSide_method = nullptr;
jmethodID HandGestureAlgorithmProxyWrapper::s_getHandType_method = nullptr;
jmethodID HandGestureAlgorithmProxyWrapper::s_getHandToward_method = nullptr;
jmethodID HandGestureAlgorithmProxyWrapper::s_getHandTypeConfidence_method = nullptr;
jmethodID HandGestureAlgorithmProxyWrapper::s_getLandMark2DCount_method = nullptr;
jmethodID HandGestureAlgorithmProxyWrapper::s_getLandMark2DArray_method = nullptr;

void HandGestureAlgorithmProxyWrapper::init() {
    DexClassLoaderWrapper* dexClassLoaderWrapper = DynamicLoader::s_DexClassLoader;
    s_class = dexClassLoaderWrapper->loadClassGlobalRef(s_class_name);
    s_getAllHandGestureId_method = DynamicLoader::getMethodId(s_class, "getAllHandGestureId", "()[I");
    s_getHandSide_method = DynamicLoader::getMethodId(s_class, "getHandSide", "(I)I");
    s_getHandType_method = DynamicLoader::getMethodId(s_class, "getHandType", "(I)I");
    s_getHandToward_method =DynamicLoader::getMethodId(s_class, "getHandToward", "(I)I");
    s_getHandTypeConfidence_method = DynamicLoader::getMethodId(s_class, "getHandTypeConfidence", "(I)F");
    s_getLandMark2DCount_method = DynamicLoader::getMethodId(s_class, "getLandMark2DCount", "(I)I");
    s_getLandMark2DArray_method = DynamicLoader::getMethodId(s_class, "getLandMark2DArray", "(IJ)V");

}

std::vector<int> HandGestureAlgorithmProxyWrapper::getAllHandGestureId() {
    if (m_obj == nullptr)
        return std::vector<int>();
    std::vector<int> res;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jintArray hand_ids = (jintArray)env->CallObjectMethod(m_obj, s_getAllHandGestureId_method);
    if (hand_ids == nullptr)
        return res;
    int length = env->GetArrayLength(hand_ids);
    jboolean isCopy;
    jint* hand_id_ptr = env->GetIntArrayElements(hand_ids, &isCopy);
    for (int i = 0; i < length; ++i) {
        res.push_back(hand_id_ptr[i]);
    }
    env->ReleaseIntArrayElements(hand_ids, hand_id_ptr, 0);
    env->DeleteLocalRef(hand_ids);
    return res;
}

int32_t HandGestureAlgorithmProxyWrapper::getHandSide(int32_t hand_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getHandSide_method, hand_id);
}

int32_t HandGestureAlgorithmProxyWrapper::getHandGestureType(int32_t hand_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getHandType_method, hand_id);
}

int32_t HandGestureAlgorithmProxyWrapper::getHandToward(int32_t hand_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getHandToward_method, hand_id);
}



float_t HandGestureAlgorithmProxyWrapper::getHandTypeConfidence(int32_t hand_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallFloatMethod(m_obj, s_getHandTypeConfidence_method, hand_id);
}


int32_t HandGestureAlgorithmProxyWrapper::getLandMark2DCount(int32_t hand_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getLandMark2DCount_method, hand_id);
}


void HandGestureAlgorithmProxyWrapper::getLandMark2DArray(int32_t handId,
                                                          float *out_landMark2DArray) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getLandMark2DArray_method,handId, reinterpret_cast<jlong>(out_landMark2DArray));
}

std::string FaceMeshAlgorithmProxyWrapper::s_class_name = "com/standardar/service/facemesh/FaceMeshLibLoadAlgorithmProxy";
jclass FaceMeshAlgorithmProxyWrapper::s_class = nullptr;
FaceMeshAlgorithmProxyWrapper::FaceMeshAlgorithmProxyWrapper(jobject obj, bool is_global):JavaClassWrapper(obj,is_global) {}
jmethodID FaceMeshAlgorithmProxyWrapper::s_getAllFaceMeshId_method = nullptr;
jmethodID FaceMeshAlgorithmProxyWrapper::s_getVertexCount_method = nullptr;
jmethodID FaceMeshAlgorithmProxyWrapper::s_getIndexCount_method = nullptr;
jmethodID FaceMeshAlgorithmProxyWrapper::s_getTextureCoordinateCount_method = nullptr;
jmethodID FaceMeshAlgorithmProxyWrapper::s_getNormalCount_method = nullptr;
jmethodID FaceMeshAlgorithmProxyWrapper::s_getVertices_method = nullptr;
jmethodID FaceMeshAlgorithmProxyWrapper::s_getIndices_method = nullptr;
jmethodID FaceMeshAlgorithmProxyWrapper::s_getTextureCoordinates_method = nullptr;
jmethodID FaceMeshAlgorithmProxyWrapper::s_getNormals_method = nullptr;
jmethodID FaceMeshAlgorithmProxyWrapper::s_getPose_method = nullptr;

void FaceMeshAlgorithmProxyWrapper::init() {
    DexClassLoaderWrapper* dexClassLoaderWrapper = DynamicLoader::s_DexClassLoader;
    s_class = dexClassLoaderWrapper->loadClassGlobalRef(s_class_name);
    s_getAllFaceMeshId_method = DynamicLoader::getMethodId(s_class, "getAllFaceMeshId", "()[I");
    s_getVertexCount_method = DynamicLoader::getMethodId(s_class, "getVertexCount", "(I)I");
    s_getIndexCount_method = DynamicLoader::getMethodId(s_class, "getIndexCount", "(I)I");
    s_getTextureCoordinateCount_method = DynamicLoader::getMethodId(s_class, "getTextureCoordinateCount", "(I)I");
    s_getNormalCount_method =DynamicLoader::getMethodId(s_class, "getNormalCount", "(I)I");
    s_getVertices_method = DynamicLoader::getMethodId(s_class, "getVertices", "(I)J");
    s_getIndices_method = DynamicLoader::getMethodId(s_class, "getIndices", "(I)J");
    s_getTextureCoordinates_method = DynamicLoader::getMethodId(s_class, "getTextureCoordinates", "(I)J");
    s_getNormals_method = DynamicLoader::getMethodId(s_class, "getNormals", "(I)J");
    s_getPose_method = DynamicLoader::getMethodId(s_class, "getPose", "(IJ)V");

}

std::vector<int> FaceMeshAlgorithmProxyWrapper::getAllFaceMeshId() {
    if (m_obj == nullptr)
        return std::vector<int>();
    std::vector<int> res;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jintArray face_ids = (jintArray)env->CallObjectMethod(m_obj, s_getAllFaceMeshId_method);
    if (face_ids == nullptr)
        return res;
    int length = env->GetArrayLength(face_ids);
    jboolean isCopy;
    jint* face_id_ptr = env->GetIntArrayElements(face_ids, &isCopy);
    for (int i = 0; i < length; ++i) {
        res.push_back(face_id_ptr[i]);
    }
    env->ReleaseIntArrayElements(face_ids, face_id_ptr, 0);
    env->DeleteLocalRef(face_ids);
    return res;
}


int32_t FaceMeshAlgorithmProxyWrapper::getVertexCount(int32_t face_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getVertexCount_method, face_id);
}


int32_t FaceMeshAlgorithmProxyWrapper::getIndexCount(int32_t face_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getIndexCount_method, face_id);
}


int32_t FaceMeshAlgorithmProxyWrapper::getTextureCoordinateCount(int32_t face_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getTextureCoordinateCount_method, face_id);
}


int32_t FaceMeshAlgorithmProxyWrapper::getNormalCount(int32_t face_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getNormalCount_method, face_id);
}


float* FaceMeshAlgorithmProxyWrapper::getVertices(int32_t face_id) {
    if (m_obj == nullptr)
        return nullptr;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    long verticePtr = env->CallLongMethod(m_obj, s_getVertices_method,face_id);

    return reinterpret_cast<float *>(verticePtr);
}


int32_t* FaceMeshAlgorithmProxyWrapper::getIndices(int32_t face_id) {
    if (m_obj == nullptr)
        return nullptr;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    long indicesPtr =env->CallLongMethod(m_obj, s_getIndices_method,face_id);

    return reinterpret_cast<int32_t *>(indicesPtr);

}


float* FaceMeshAlgorithmProxyWrapper::getTextureCoordinates(int32_t face_id) {
    if (m_obj == nullptr)
        return nullptr;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    long textureCoordinatesPtr = env->CallLongMethod(m_obj, s_getTextureCoordinates_method,face_id);
    return reinterpret_cast<float *>(textureCoordinatesPtr);

}


float* FaceMeshAlgorithmProxyWrapper::getNormals(int32_t face_id) {
    if (m_obj == nullptr)
        return nullptr;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    long normalsPtr = env->CallLongMethod(m_obj, s_getNormals_method,face_id);
    return reinterpret_cast<float *>(normalsPtr);

}

void FaceMeshAlgorithmProxyWrapper::getPose(int32_t face_id, float *out_pose_ptr) {
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getPose_method,face_id, reinterpret_cast<jlong>(out_pose_ptr));
}

std::string ObjectTrackingAlgorithmProxyWrapper::s_class_name = "com/standardar/service/objecttracking/algorithm/ObjectTrackingLibLoadAlgorithmProxy";
jclass ObjectTrackingAlgorithmProxyWrapper::s_class = nullptr;
ObjectTrackingAlgorithmProxyWrapper::ObjectTrackingAlgorithmProxyWrapper(jobject obj, bool is_global) : JavaClassWrapper(obj, is_global){}
jmethodID ObjectTrackingAlgorithmProxyWrapper::s_setReferenceObjectDatabase_method = nullptr;
jmethodID ObjectTrackingAlgorithmProxyWrapper::s_getObjectTrackingInfo_method = nullptr;
jmethodID ObjectTrackingAlgorithmProxyWrapper::s_getCameraViewMatrix_method = nullptr;
jmethodID ObjectTrackingAlgorithmProxyWrapper::s_getCenterPose_method = nullptr;
jmethodID ObjectTrackingAlgorithmProxyWrapper::s_getExtentX_method = nullptr;
jmethodID ObjectTrackingAlgorithmProxyWrapper::s_getExtentY_method = nullptr;
jmethodID ObjectTrackingAlgorithmProxyWrapper::s_getExtentZ_method = nullptr;
jmethodID ObjectTrackingAlgorithmProxyWrapper::s_getBoundingBox_method = nullptr;
jmethodID ObjectTrackingAlgorithmProxyWrapper::s_getAllObjectId_method = nullptr;

void ObjectTrackingAlgorithmProxyWrapper::init() {
    DexClassLoaderWrapper* dexClassLoaderWrapper = DynamicLoader::s_DexClassLoader;
    s_class = dexClassLoaderWrapper->loadClassGlobalRef(s_class_name);
    s_setReferenceObjectDatabase_method = DynamicLoader::getMethodId(s_class, "setReferenceObjectDatabase", "(Ljava/nio/ByteBuffer;)V");
    s_getObjectTrackingInfo_method = DynamicLoader::getMethodId(s_class, "getObjectTrackingInfo", "()Ljava/lang/String;");
    s_getCameraViewMatrix_method = DynamicLoader::getMethodId(s_class, "getViewMatrix","(JI)V");
    s_getCenterPose_method = DynamicLoader::getMethodId(s_class, "getCenterPose", "(IJ)V");
    s_getExtentX_method = DynamicLoader::getMethodId(s_class, "getExtentX", "(I)F");
    s_getExtentY_method = DynamicLoader::getMethodId(s_class, "getExtentY", "(I)F");
    s_getExtentZ_method = DynamicLoader::getMethodId(s_class, "getExtentZ", "(I)F");
    s_getBoundingBox_method = DynamicLoader::getMethodId(s_class, "getBoundingBox", "(IJ)V");
    s_getAllObjectId_method = DynamicLoader::getMethodId(s_class, "getAllObjectId", "()[I");
}

void ObjectTrackingAlgorithmProxyWrapper::setReferenceObjectDatabase(CReferenceObjectDatabase& database)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    int size = sizeof(int) * 3 + sizeof(int) * 2 * database.getUpdateObjectNum() + database.getObjectDataSize();
    jbyte* buffer = new jbyte[size];
    database.getObjectData(buffer, size, BUFFER_LITTLER_ENDIAN);
    jobject byteBuffer = env->NewDirectByteBuffer(buffer, size);

    env->CallVoidMethod(m_obj, s_setReferenceObjectDatabase_method, byteBuffer);
}

std::string ObjectTrackingAlgorithmProxyWrapper::getObjectTrackingInfo()
{
    if (m_obj == nullptr)
        return "";
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jstring info = (jstring)env->CallObjectMethod(m_obj, s_getObjectTrackingInfo_method);
    jboolean is_copy;
    const char* info_ptr = env->GetStringUTFChars(info, &is_copy);
    std::string object_info = info_ptr;
    env->ReleaseStringUTFChars(info, info_ptr);
    env->DeleteLocalRef(info);
    return object_info;
}

void ObjectTrackingAlgorithmProxyWrapper::getCameraViewMatrix(float* view, int screen_rotate)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getCameraViewMatrix_method, reinterpret_cast<jlong>(view),screen_rotate);
}

void ObjectTrackingAlgorithmProxyWrapper::getCenterPose(int object_id, float* pose_ptr)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    env->CallVoidMethod(m_obj, s_getCenterPose_method, object_id, reinterpret_cast<jlong>(pose_ptr));
}

float ObjectTrackingAlgorithmProxyWrapper::getExtentX(int object_id)
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallFloatMethod(m_obj, s_getExtentX_method, object_id);
}

float ObjectTrackingAlgorithmProxyWrapper::getExtentY(int object_id)
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallFloatMethod(m_obj, s_getExtentY_method, object_id);
}

float ObjectTrackingAlgorithmProxyWrapper::getExtentZ(int object_id)
{
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallFloatMethod(m_obj, s_getExtentZ_method, object_id);
}

void ObjectTrackingAlgorithmProxyWrapper::getBoundingBox(int object_id, float* boundingBox)
{
    if (m_obj == nullptr)
        return;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    return env->CallVoidMethod(m_obj, s_getBoundingBox_method, object_id, reinterpret_cast<jlong>(boundingBox));
}

std::vector<int> ObjectTrackingAlgorithmProxyWrapper::getAllObjectId()
{
    if (m_obj == nullptr)
        return std::vector<int>();
    std::vector<int> res;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    jintArray object_ids = (jintArray)env->CallObjectMethod(m_obj, s_getAllObjectId_method);
    if (object_ids == nullptr)
        return res;
    int length = env->GetArrayLength(object_ids);
    jboolean isCopy;
    jint* object_id_ptr = env->GetIntArrayElements(object_ids, &isCopy);
    for (int i = 0; i < length; ++i) {
        res.push_back(object_id_ptr[i]);
    }
    env->ReleaseIntArrayElements(object_ids, object_id_ptr, 0);
    env->DeleteLocalRef(object_ids);
    return res;
}

std::string BodyAlgorithmProxyWrapper::s_class_name = "com/standardar/service/bodydetect/BodyDetectLibLoadAlgorithmProxy";
jclass BodyAlgorithmProxyWrapper::s_class = nullptr;
BodyAlgorithmProxyWrapper::BodyAlgorithmProxyWrapper(jobject obj, bool is_global):JavaClassWrapper(obj,is_global) {}
jmethodID BodyAlgorithmProxyWrapper::s_getAllBodyId_method = nullptr;
jmethodID BodyAlgorithmProxyWrapper::s_getSkeletonPoint2dCount_method = nullptr;
jmethodID BodyAlgorithmProxyWrapper::s_getSkeletonPoint2d_method = nullptr;
jmethodID BodyAlgorithmProxyWrapper::s_getSkeletonPoint2dConfidence_method = nullptr;


//jmethodID BodyAlgorithmProxyWrapper::s_getIndexCount_method = nullptr;
//jmethodID BodyAlgorithmProxyWrapper::s_getTextureCoordinateCount_method = nullptr;
//jmethodID BodyAlgorithmProxyWrapper::s_getNormalCount_method = nullptr;
//jmethodID BodyAlgorithmProxyWrapper::s_getIndices_method = nullptr;
//jmethodID BodyAlgorithmProxyWrapper::s_getTextureCoordinates_method = nullptr;
//jmethodID BodyAlgorithmProxyWrapper::s_getNormals_method = nullptr;
//jmethodID BodyAlgorithmProxyWrapper::s_getPose_method = nullptr;

void BodyAlgorithmProxyWrapper::init() {
    DexClassLoaderWrapper* dexClassLoaderWrapper = DynamicLoader::s_DexClassLoader;
    s_class = dexClassLoaderWrapper->loadClassGlobalRef(s_class_name);
    s_getAllBodyId_method = DynamicLoader::getMethodId(s_class, "getAllBodyId", "()[I");
    s_getSkeletonPoint2dCount_method = DynamicLoader::getMethodId(s_class, "getSkeletonPoint2dCount", "(I)I");
    s_getSkeletonPoint2d_method = DynamicLoader::getMethodId(s_class, "getSkeletonPoint2d", "(IJ)V");
    s_getSkeletonPoint2dConfidence_method = DynamicLoader::getMethodId(s_class, "getSkeletonPoint2dConfidence", "(IJ)V");

//    s_getTextureCoordinateCount_method = DynamicLoader::getMethodId(s_class, "getTextureCoordinateCount", "(I)I");
//    s_getNormalCount_method =DynamicLoader::getMethodId(s_class, "getNormalCount", "(I)I");
//    s_getIndices_method = DynamicLoader::getMethodId(s_class, "getIndices", "(IJ)V");
//    s_getTextureCoordinates_method = DynamicLoader::getMethodId(s_class, "getTextureCoordinates", "(IJ)V");
//    s_getNormals_method = DynamicLoader::getMethodId(s_class, "getNormals", "(IJ)V");
//    s_getPose_method = DynamicLoader::getMethodId(s_class, "getPose", "(IJ)V");

}

std::vector<int> BodyAlgorithmProxyWrapper::getAllBodyId() {
    if (m_obj == nullptr)
        return std::vector<int>();
    std::vector<int> res;
    JNIEnvProxy env(DynamicLoader::s_JVM);

    jintArray body_ids = (jintArray)env->CallObjectMethod(m_obj, s_getAllBodyId_method);
    if (body_ids == nullptr)
        return res;
    int length = env->GetArrayLength(body_ids);
    jboolean isCopy;
    jint* body_id_ptr = env->GetIntArrayElements(body_ids, &isCopy);
    for (int i = 0; i < length; ++i) {
        res.push_back(body_id_ptr[i]);
    }
    env->ReleaseIntArrayElements(body_ids, body_id_ptr, 0);
    env->DeleteLocalRef(body_ids);
    return res;
}

int32_t BodyAlgorithmProxyWrapper::getSkeletonPoint2dCount(int32_t body_id) {
    if (m_obj == nullptr)
        return 0;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    return env->CallIntMethod(m_obj, s_getSkeletonPoint2dCount_method, body_id);
}


void BodyAlgorithmProxyWrapper::getSkeletonPoint2d(int32_t body_id, float *out_point2d) {
    if (m_obj == nullptr)
        return ;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getSkeletonPoint2d_method,body_id, reinterpret_cast<jlong>(out_point2d));
}


void BodyAlgorithmProxyWrapper::getSkeletonPoint2dConfidence(int32_t body_id,
                                                             float *out_point2d_confidence) {
    if (m_obj == nullptr)
        return ;
    JNIEnvProxy env(DynamicLoader::s_JVM);
    env->CallVoidMethod(m_obj, s_getSkeletonPoint2dConfidence_method,body_id, reinterpret_cast<jlong>(out_point2d_confidence));
}

