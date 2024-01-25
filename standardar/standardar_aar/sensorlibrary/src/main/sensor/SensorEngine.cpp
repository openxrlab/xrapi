#include <string>
#include <picojson.h>
#include <ArCommon.h>
#include "SensorEngine.h"
#include "log.h"
#include "ArJniHelpler.h"
#include "StandardARCommon.h"
#include "DynamicLoader.h"
#define DEVICE_PARAM_NAME           "device_params.yaml"
namespace standardar
{
    SensorEngine::SensorEngine(int source_type)
    {
        m_Source_Tyep = source_type;
        m_Callback = NULL;
        m_IMUReader_obj = NULL;
        m_Context = NULL;
        m_Sensor_Open = false;
    }

    SensorEngine::~SensorEngine()
    {
        m_Context = nullptr;
    }

    void SensorEngine::InitFromJava()
    {
        if (m_Sensor_Open)
        {
            LOGW("sensor is opened");
            return;
        }
        if (!m_Context)
        {
            LOGW("sensor engine context is null");
            return;
        }

//        JNIEnvProxy jni_env = ArJniHelpler::getJavaEnv();
//        if (jni_env == nullptr || m_IMUReaderClass == nullptr)
//        {
//            LOGE("sensor engine can not get jni env or class not found %p %p", jni_env.get(), m_IMUReaderClass);
//            return;
//        }
//
//        jmethodID imu_getInstanceId = jni_env->GetStaticMethodID(m_IMUReaderClass, "getInstance",
//                                                                 "(Landroid/content/Context;JJ)Lcom/standardar/sensor/imu/IMUReader;");
//        if (imu_getInstanceId == 0)
//        {
//            LOGE("sensor engine function getInstance is not found");
//            return;
//        }
//
//        jobject imureader_obj = jni_env->CallStaticObjectMethod(m_IMUReaderClass, imu_getInstanceId,
//                                                                reinterpret_cast<jobject>(m_Context),
//                                                                reinterpret_cast<jlong>((void*)NULL),
//                                                                reinterpret_cast<jlong>(this));
        JNIEnvProxy jni_env(DynamicLoader::s_JVM);
        jobject imureader_obj = IMUReaderWrapper::construct(m_Context);
        if (imureader_obj == NULL)
        {
            LOGE("sensor engine can not get imu reader object");
            return;
        }

        m_IMUReader_obj = jni_env->NewGlobalRef(imureader_obj);

//        jmethodID imureader_openSensorId = jni_env->GetMethodID(m_IMUReaderClass, "openSensor", "()V");
//        if (imureader_openSensorId == 0)
//        {
//            LOGE("openSensor method is not found");
//            return;
//        }
//        jni_env->CallVoidMethod(m_IMUReader_obj, imureader_openSensorId);

        IMUReaderWrapper imuReaderWrapper(m_IMUReader_obj, true);
        imuReaderWrapper.openSensor();
        m_Sensor_Open = true;
    }

    void SensorEngine::Init()
    {
        switch (m_Source_Tyep)
        {
            case SENSOR_SOURCE_JAVA:
            {
                InitFromJava();
                break;
            }
            default:
                break;
        }
    }

    void SensorEngine::Start()
    {
        switch (m_Source_Tyep)
        {
            case SENSOR_SOURCE_JAVA:
            {
                StartFromJava();
                break;
            }
            default:
                break;
        }
    }

    void SensorEngine::StartFromJava()
    {
        if (!m_Sensor_Open)
            return;
//        JNIEnvProxy jni_env = ArJniHelpler::getJavaEnv();
//        jmethodID imureader_startId = jni_env->GetMethodID(m_IMUReaderClass, "start", "(I)V");
//        if (imureader_startId == 0)
//        {
//            LOGE("start method is not found");
//            return;
//        }
//        jni_env->CallVoidMethod(m_IMUReader_obj, imureader_startId, GetIMUHz());
        IMUReaderWrapper imuReaderWrapper(m_IMUReader_obj, true);
        imuReaderWrapper.start();
    }

    void SensorEngine::Stop()
    {
        switch (m_Source_Tyep)
        {
            case SENSOR_SOURCE_JAVA:
            {
                StopFromJava();
                break;
            }
            default:
                break;
        }
    }

    void SensorEngine::StopFromJava()
    {
        if (!m_Sensor_Open)
            return;
//        JNIEnvProxy jni_env = ArJniHelpler::getJavaEnv();
//        jmethodID imureader_stopId = jni_env->GetMethodID(m_IMUReaderClass, "stop", "()V");
//        if (imureader_stopId == 0)
//        {
//            LOGE("start method is not found");
//            return;
//        }
//        jni_env->CallVoidMethod(m_IMUReader_obj, imureader_stopId);
        IMUReaderWrapper imuReaderWrapper(m_IMUReader_obj, true);
        imuReaderWrapper.stop();
    }

    void SensorEngine::DeInitFromeJava()
    {
        if (!m_Sensor_Open || m_IMUReader_obj == NULL)
        {
            LOGW("sensor is not opened %d %p", m_Sensor_Open, m_IMUReader_obj);
            return;
        }

        JNIEnvProxy jni_env(DynamicLoader::s_JVM);
//        if (jni_env == NULL)
//        {
//            return;
//        }
//
//        jmethodID imureader_closeSensorid = jni_env->GetMethodID(m_IMUReaderClass, "closeSensor", "()V");
//        if (imureader_closeSensorid == 0)
//        {
//            LOGE("imureader stop method is not found");
//            return;
//        }
//        jni_env->CallVoidMethod(m_IMUReader_obj, imureader_closeSensorid);
        IMUReaderWrapper imuReaderWrapper(m_IMUReader_obj, true);
        imuReaderWrapper.closeSensor();
        jni_env->DeleteGlobalRef(m_IMUReader_obj);
        m_IMUReader_obj = NULL;
        m_Context = NULL;
        m_Sensor_Open = false;
    }

    void SensorEngine::DeInit()
    {
        switch (m_Source_Tyep)
        {
            case SENSOR_SOURCE_JAVA:
            {
                DeInitFromeJava();
                break;
            }
            default:
                break;
        }
    }

    void SensorEngine::SetContext(jobject context)
    {
        m_Context = context;
    }

    void SensorEngine::SetNotify(jobject object_client)
    {
        if (object_client == nullptr)
            return;
        if (m_IMUReader_obj == nullptr)
            return;

//        JNIEnvProxy env = ArJniHelpler::getJavaEnv();
//        jmethodID register_id = env->GetMethodID(m_IMUReaderClass, "setSensorNotify", "(Lcom/standardar/sensor/imu/IMUReader$ISensorNotifyCallback;)V");
//        env->CallVoidMethod(m_IMUReader_obj, register_id, object_client);

        IMUReaderWrapper imuReaderWrapper(m_IMUReader_obj, true);
        imuReaderWrapper.setSensorNotify(object_client);
    }

}
