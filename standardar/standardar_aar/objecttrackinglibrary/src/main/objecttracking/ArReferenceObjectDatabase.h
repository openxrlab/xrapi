
#ifndef STANDARD_AR_ARREFERENCEOBJECTDATABASE_C_API_H_
#define STANDARD_AR_ARREFERENCEOBJECTDATABASE_C_API_H_

#include <stddef.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <string.h>

#include "ArObjectNode.h"
#include "jni.h"

namespace standardar
{
    class CReferenceObjectDatabase {
    public:
        CReferenceObjectDatabase();
        ~CReferenceObjectDatabase();
        CObjectNode* getReferenceObjectById(int32_t id);
        void deserialize(const uint8_t *bytes, int64_t size);
        void loadConfigure(const char* config);
        void addReferenceObject(const char *object_name, const uint8_t *object_raw_bytes, int64_t object_raw_bytes_size, int32_t *out_index);
        void getObjectData(jbyte* data_ptr, int length, int order);
        int getObjectDataSize();
        int getObjectNum();
        int getUpdateObjectNum();
        void setObjectDatabase(CReferenceObjectDatabase* database);
        void getObjectDatabase(CReferenceObjectDatabase* database);

    private:
        std::vector<CObjectNode*> m_ReferenceObjectDatabaseList;
        int m_ObjectIndexBeforeUpdate;
    };

}


#endif  // STANDARD_AR_ARREFERENCEOBJECTDATABASE_C_API_H_
