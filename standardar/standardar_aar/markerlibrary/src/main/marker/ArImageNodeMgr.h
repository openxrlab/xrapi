
#ifndef STANDARD_AR_ARIMAGENODEMGR_C_API_H_
#define STANDARD_AR_ARIMAGENODEMGR_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <string.h>
#include "ArImageNode.h"
namespace standardar
{
    class CImageNodeMgr {
    public:
        CImageNodeMgr();
        ~CImageNodeMgr();
        CImageNode* getReferenceImageById(int32_t id);
        void serialize(uint8_t **bytes, int64_t *size);
        void deserialize(const uint8_t *bytes, int64_t size);
        void loadConfigure(const char* config);
        void addPatt(const char* patt_name, const uint8_t *bytes, int64_t size);
        void addImage(const char* image_name,
                      const uint8_t *image_grayscale_pixels,
                      int32_t image_width_in_pixels,
                      int32_t image_height_in_pixels,
                      int32_t image_stride_in_pixels,
                      float image_width_in_meters,
                      int32_t *out_index);
        void setReferenceImageDatabase(CImageNodeMgr* database);
        void getReferenceImageDatabase(CImageNodeMgr* database);
        void getImageData(jbyte* data_ptr, int length, int order);
        int getImageDataSize();
        int getImageNum();
        int getUpdateImageNum();

    private:
        std::vector<CImageNode*> m_AugmentedImageList;
        int m_ImageIndexBeforeUpdate;
    };

}


#endif  // STANDARD_AR_ARIMAGENODEMGR_C_API_H_
