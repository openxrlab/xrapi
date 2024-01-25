
#include <sys/system_properties.h>
#include "FileUtils.h"
#include "log.h"

std::string FileUtils::getPhoneBrand() {
    char brand[PROP_VALUE_MAX];
    __system_property_get("ro.product.brand", brand);
    return brand;
}

std::string FileUtils::getPhoneModel() {
    char model[PROP_VALUE_MAX];
    __system_property_get("ro.product.model", model);
    return model;
}

bool FileUtils::extractAsset(AAssetManager *apkAssetManager, std::string assetName,
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

bool FileUtils::readFromAsset(AAssetManager *apkAssetManager, std::string assetName,
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

bool FileUtils::readFile(std::string filepath, char **file_buf,
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

