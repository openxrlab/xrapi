#ifndef STANDARD_AR_SENSORENGINE_API_H
#define STANDARD_AR_SENSORENGINE_API_H

#include <vector>
#include <android/sensor.h>
#include <functional>
#include <jni.h>



namespace standardar
{



    typedef enum SensorEngineSource {
        SENSOR_SOURCE_JAVA,
        SENSOR_SOURCE_C
    }SensorEngineSource;

    class SensorEngine {
    public:
        SensorEngine(int source_type);
        ~SensorEngine();

        void Init();
        void Start();
        void Stop();
        void DeInit();
        void SetContext(jobject context);
        void SetNotify(jobject object_client);
    private:
        void InitFromJava();
        void StartFromJava();
        void StopFromJava();
        void DeInitFromeJava();
    private:
        int m_Source_Tyep;
        bool m_Sensor_Open;
        std::function<void(void *ctx, double *values, int length,
                           double timestamp, int tag)> m_Callback;
        jobject m_IMUReader_obj;
        jobject m_Context;
    };
}

#endif //STANDARD_AR_SENSORENGINE_API_H
