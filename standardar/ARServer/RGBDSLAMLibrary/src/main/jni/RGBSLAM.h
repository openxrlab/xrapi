#ifndef ARSERVER_RGBSLAM_H
#define ARSERVER_RGBSLAM_H


#include "SLAMBase.h"

class RGBSLAM : public SLAMBase{
public:
    RGBSLAM();
    int initSLAM(AAssetManager* assetManager, string pathToInternalPath, int w, int h, float fovh);
    virtual void setFrame(unsigned char* image_ptr, int format, int width, int height, int stride,
                          int64_t exposure_time, int64_t timestamp);
    virtual void startProcessFrame();
protected:
    void getCameraIntrinsics(string config);
    XRSLAMImage m_SLAMImageData;
    XRSLAMImageExtension m_SLAMImageDataExtension;
};


#endif //ARSERVER_RGBSLAM_H
