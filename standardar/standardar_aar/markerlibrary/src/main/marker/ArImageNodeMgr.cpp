#include <log.h>
#include <ArCommon.h>
#include "ArImageNodeMgr.h"
#include "miniz.h"
#include "picojson.h"

namespace standardar
{
    //stAugmentedImageDatabase
    CImageNodeMgr::CImageNodeMgr()
    {
        m_ImageIndexBeforeUpdate = 0;
    }

    CImageNodeMgr::~CImageNodeMgr()
    {
        for(CImageNode* pMarker:m_AugmentedImageList)
        {
            if(pMarker) {
                delete pMarker;
                pMarker = nullptr;
            }
        }
        m_AugmentedImageList.clear();
    }

    CImageNode* CImageNodeMgr::getReferenceImageById(int32_t id)
    {
//        for (auto&& [first,second] : augmentedImageList) //c++17
        for (CImageNode* pMarker:m_AugmentedImageList) //c++11
        {
            if (pMarker->m_MarkerId == id)
                return pMarker;
        }
        return nullptr;
    }

    void CImageNodeMgr::deserialize(const uint8_t *bytes, int64_t size)
    {
        mz_zip_archive zip_archive;
        memset(&zip_archive, 0, sizeof(zip_archive));

        mz_bool status = mz_zip_reader_init_mem(&zip_archive, bytes, size, 0);
        if (!status)
        {
            LOGW("mz_zip_reader_init_file() failed!\n");
        }

        for (int i = 0; i < (int)mz_zip_reader_get_num_files(&zip_archive); i++)
        {
            mz_zip_archive_file_stat file_stat;
            if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
            {
                LOGW("mz_zip_reader_file_stat() failed!\n");
                mz_zip_reader_end(&zip_archive);
            }

            std::string fileName = file_stat.m_filename;
            if(fileName == "all-markers.conf"){
                int configBufferSize;
                char* configBuffer = (char *)mz_zip_reader_extract_to_heap(&zip_archive, file_stat.m_file_index, (size_t *)&configBufferSize, 0);
                char* string_configBuffer = new char[configBufferSize + 1];
                memcpy(string_configBuffer, configBuffer, configBufferSize);
                string_configBuffer[configBufferSize] = '\0';
                loadConfigure(string_configBuffer);
                delete[] string_configBuffer;
                free(configBuffer);
                break;
            }

        }

        for (int i = 0; i < (int)mz_zip_reader_get_num_files(&zip_archive); i++)
        {
            mz_zip_archive_file_stat file_stat;
            if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat))
            {
                LOGW("mz_zip_reader_file_stat() failed!\n");
                mz_zip_reader_end(&zip_archive);
            }

            for(int j = 0; j<m_AugmentedImageList.size(); ++j){
                std::string fileName = file_stat.m_filename;
                if(fileName == m_AugmentedImageList[j]->m_Name + ".patt"){
                    m_AugmentedImageList[j]->m_MarkerId = j;
                    m_AugmentedImageList[j]->m_buffer = (unsigned char*)mz_zip_reader_extract_to_heap(&zip_archive, file_stat.m_file_index, (size_t *)&m_AugmentedImageList[j]->m_bufferSize, 0);
                    break;
                }
            }
        }
        mz_zip_reader_end(&zip_archive);
    }

    void CImageNodeMgr::loadConfigure(const char* config)
    {
        std::string jsonStr(config);
        picojson::value v;
        std::string err = picojson::parse(v, jsonStr);
        if (!err.empty()) {
            return;
        }

        const auto& array = v.get<picojson::array>();
        for (picojson::array::const_iterator it = array.begin(); it != array.end(); it++)
        {
            picojson::object obj = it->get<picojson::object>();
            std::string global_id = obj["markerId"].get<std::string>();
            std::string marker_name = obj["markerName"].get<std::string>();
            int score = (int) obj["score"].get<double>();
            float meters_in_width = (float) obj["width"].get<double>();

            CImageNode* imageNode = new CImageNode(0, marker_name.c_str(), ImageNodeType::Patt);
            imageNode->m_GlobalId = global_id;
            imageNode->m_Score = score;
            imageNode->m_physicalSize = meters_in_width;

            LOGI("load name:%s", marker_name.c_str());
            LOGI("load global id:%s", global_id.c_str());
            LOGI("load score:%d", score);
            LOGI("load physicalSize:%f", meters_in_width);

            m_AugmentedImageList.push_back(imageNode);
        }
    }

    void CImageNodeMgr::addPatt(const char* patt_name, const uint8_t *bytes, int64_t size)
    {
        for(int j = 0; j<m_AugmentedImageList.size(); ++j){
            std::string fileName = patt_name;
            if(fileName == m_AugmentedImageList[j]->m_Name + ".patt"){
                m_AugmentedImageList[j]->m_MarkerId = j;
                m_AugmentedImageList[j]->m_bufferSize = (int) size;
                m_AugmentedImageList[j]->m_buffer = (unsigned char*)malloc(size);
                memcpy(m_AugmentedImageList[j]->m_buffer, bytes, size);
                break;
            }
        }
    }

    void CImageNodeMgr::addImage(const char* image_name,
                  const uint8_t *image_grayscale_pixels,
                  int32_t image_width_in_pixels,
                  int32_t image_height_in_pixels,
                  int32_t image_stride_in_pixels,
                  float image_width_in_meters,
                  int32_t *out_index)
    {
        int index = m_AugmentedImageList.size();
        CImageNode* imageNode = new CImageNode(index, image_name, ImageNodeType::Image);
        imageNode->m_width = image_width_in_pixels;
        imageNode->m_height = image_height_in_pixels;
        imageNode->m_stride = image_stride_in_pixels;
        imageNode->m_physicalSize = image_width_in_meters;
        imageNode->m_bufferSize = image_height_in_pixels * image_width_in_pixels;
        imageNode->m_buffer = (unsigned char*)malloc(image_height_in_pixels * image_width_in_pixels);
        memcpy(imageNode->m_buffer, image_grayscale_pixels, image_height_in_pixels * image_width_in_pixels);
        *out_index = index;

        m_AugmentedImageList.push_back(imageNode);
    }

    void CImageNodeMgr::setReferenceImageDatabase(CImageNodeMgr* database)
    {
        m_ImageIndexBeforeUpdate = m_AugmentedImageList.size();
        for (CImageNode* pMarker:database->m_AugmentedImageList)
        {
            CImageNode* tmpMarker = new CImageNode(m_ImageIndexBeforeUpdate + pMarker->m_MarkerId, pMarker->m_Name.c_str(), pMarker->m_InputType);
            pMarker->copyImageNode(tmpMarker);
            m_AugmentedImageList.push_back(tmpMarker);
        }
    }

    void CImageNodeMgr::getReferenceImageDatabase(CImageNodeMgr* database)
    {
        database->m_AugmentedImageList.clear();
        for (CImageNode* pMarker:m_AugmentedImageList)
        {
            CImageNode* tmpMarker = new CImageNode(pMarker->m_MarkerId, pMarker->m_Name.c_str(), pMarker->m_InputType);
            pMarker->copyImageNode(tmpMarker);
            database->m_AugmentedImageList.push_back(tmpMarker);
        }
    }

    void CImageNodeMgr::getImageData(jbyte* data_ptr, int length, int order) {
        DirectByteBufferWrapper data_buffer(data_ptr, length, (BUFFER_ORDER) order);
        data_buffer.put<int>(SEND_MAGIC_WORD);
        data_buffer.put<int>(IMAGE_TRACKINNG_VERSION);
        int num = m_AugmentedImageList.size() - m_ImageIndexBeforeUpdate;
        data_buffer.put<int>(num);
        for (int i=m_ImageIndexBeforeUpdate; i<m_AugmentedImageList.size(); ++i) {
            data_buffer.put<int>(m_AugmentedImageList[i]->m_InputType);
            data_buffer.put<int>(m_AugmentedImageList[i]->m_MarkerId);
            data_buffer.put<int>(m_AugmentedImageList[i]->m_width);
            data_buffer.put<int>(m_AugmentedImageList[i]->m_height);
            data_buffer.put<int>(m_AugmentedImageList[i]->m_stride);
            data_buffer.put<float>(m_AugmentedImageList[i]->m_physicalSize);
            data_buffer.put<int>(m_AugmentedImageList[i]->m_bufferSize);
            data_buffer.put(m_AugmentedImageList[i]->m_buffer, m_AugmentedImageList[i]->m_bufferSize);
        }
    }

    int CImageNodeMgr::getImageDataSize()
    {
        int size = 0;
        for (int i=m_ImageIndexBeforeUpdate; i<m_AugmentedImageList.size(); ++i) {
            size += m_AugmentedImageList[i]->m_bufferSize;
        }

        return size;
    }

    int CImageNodeMgr::getUpdateImageNum(){
        return m_AugmentedImageList.size() - m_ImageIndexBeforeUpdate;
    }

    int CImageNodeMgr::getImageNum(){
        return m_AugmentedImageList.size();
    }
}
