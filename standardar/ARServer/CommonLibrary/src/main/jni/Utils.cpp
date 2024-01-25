#include <sys/system_properties.h>
#include <malloc.h>
#include <string.h>
#include <fstream>

#include "Utils.h"
#include "log.h"

GlassConfig g_GlassConfig;

std::string Utils::getPhoneBrand() {
    char brand[PROP_VALUE_MAX];
    __system_property_get("ro.product.brand", brand);
    return brand;
}

std::string Utils::getPhoneModel() {
    char model[PROP_VALUE_MAX];
    __system_property_get("ro.product.model", model);
    return model;
}

std::string Utils::getProperty(std::string property)
{
    char value[PROP_VALUE_MAX];
    __system_property_get(property.c_str(), value);
    return value;
}

bool Utils::extractAsset(AAssetManager *apkAssetManager, std::string assetName,
                             std::string outputPath) {
    AAsset* asset = AAssetManager_open(apkAssetManager, assetName.c_str(), AASSET_MODE_STREAMING);
    char buf[BUFSIZ] = {'\0'};
    int nb_read = 0;
    if (asset != NULL)
    {
        FILE* out = fopen(outputPath.c_str(), "w");
        if (out == nullptr)
        {
            LOGW("open file %s failed!", outputPath.c_str());
            AAsset_close(asset);
            return false;
        }
        while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
        {
            fwrite(buf, nb_read, 1, out);
        }
        fclose(out);
        AAsset_close(asset);
        return true;
    }
    else
    {
        LOGE("Asset not found: %s", assetName.c_str());
        return false;
    }
}


bool Utils::readFromAsset(AAssetManager *apkAssetManager, std::string assetName,
                                char** file_buf, int* file_length)
{
    AAsset* asset = AAssetManager_open(apkAssetManager, assetName.c_str(), AASSET_MODE_STREAMING);
    if (asset != nullptr)
    {
        char buf[BUFSIZ] = {'\0'};
        int nb_read = 0;
        int offset = 0;
        int length = AAsset_getLength(asset);
        *file_length = length;
        *file_buf = (char*) malloc(length);
        while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
        {
            memcpy(*file_buf + offset, buf, nb_read);
            offset += nb_read;
        }
        *file_length = offset;
        AAsset_close(asset);
        return true;
    }
    else
    {
        LOGE("Asset not found: %s", assetName.c_str());
        return false;
    }
}

bool Utils::readFile(std::string filepath, char **file_buf,
                         int *file_length) {
    FILE* license_file = fopen(filepath.c_str(), "rb");
    if (license_file != NULL)
    {
        fseek(license_file, 0, SEEK_END);
        int file_size = (int)ftell(license_file);
        rewind(license_file);
        *file_buf = (char*) malloc(sizeof(char) * file_size);
        int size = (int)fread(*file_buf, sizeof(char), file_size, license_file);
        if (size != file_size)
        {
            LOGE("read file error:%d %d", size, file_size);
            free(*file_buf);
            fclose(license_file);
            return false;
        }
        fclose(license_file);
        *file_length = file_size;
        return true;
    }
    LOGE("no file:%s", filepath.c_str());
    return false;
}

bool Utils::readGlassConfig(const char *filePath, GlassConfig *glassConfig) {
    if (filePath == nullptr || glassConfig == nullptr) {
        LOGE("fail to read glass config: %d %d", filePath == nullptr, glassConfig == nullptr);
        return false;
    }

    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        LOGE("fail to open file %s", filePath);
        return false;
    }

    std::string line;
    while (getline(ifs, line)) {
        line.erase(std::remove_if(line.begin(), line.end(), std::isblank), line.end());
        std::string::size_type sepPos = line.find("=");
        if (sepPos == line.npos) {
            continue;
        }
        std::string elem = line.substr(0, sepPos),
                val = line.substr(sepPos + 1);
        if (elem == "eye_width")            glassConfig->m_eyeWidth = std::stoi(val);
        else if (elem == "eye_height")      glassConfig->m_eyeHeight = std::stoi(val);
        else if (elem == "refresh_rate")    glassConfig->m_refreshRate = std::stoi(val);
        else if (elem == "left_fx")         glassConfig->m_leftFx = std::stof(val);
        else if (elem == "left_fy")         glassConfig->m_leftFy = std::stof(val);
        else if (elem == "left_cx")         glassConfig->m_leftCx = std::stof(val);
        else if (elem == "left_cy")         glassConfig->m_leftCy = std::stof(val);
        else if (elem == "left_rot_x")      glassConfig->m_leftRotX = std::stof(val);
        else if (elem == "left_rot_y")      glassConfig->m_leftRotY = std::stof(val);
        else if (elem == "left_rot_z")      glassConfig->m_leftRotZ = std::stof(val);
        else if (elem == "left_pos_x")      glassConfig->m_leftPosX = std::stof(val);
        else if (elem == "left_pos_y")      glassConfig->m_leftPosY = std::stof(val);
        else if (elem == "left_pos_z")      glassConfig->m_leftPosZ = std::stof(val);
        else if (elem == "right_fx")        glassConfig->m_rightFx = std::stof(val);
        else if (elem == "right_fy")        glassConfig->m_rightFy = std::stof(val);
        else if (elem == "right_cx")        glassConfig->m_rightCx = std::stof(val);
        else if (elem == "right_cy")        glassConfig->m_rightCy = std::stof(val);
        else if (elem == "right_rot_x")     glassConfig->m_rightRotX = std::stof(val);
        else if (elem == "right_rot_y")     glassConfig->m_rightRotY = std::stof(val);
        else if (elem == "right_rot_z")     glassConfig->m_rightRotZ = std::stof(val);
        else if (elem == "right_pos_x")     glassConfig->m_rightPosX = std::stof(val);
        else if (elem == "right_pos_y")     glassConfig->m_rightPosY = std::stof(val);
        else if (elem == "right_pos_z")     glassConfig->m_rightPosZ = std::stof(val);
    }

    ifs.close();
    LOGI("read glass config finished");
    return true;
}

bool Utils::isFileExist(std::string filepath)
{
    FILE *f = fopen(filepath.c_str(),"r");
    if(f != NULL){
        fclose(f);
        return true;
    }

    return false;
}


