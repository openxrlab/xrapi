#include <log.h>
#include <ArCommon.h>
#include "ArReferenceObjectDatabase.h"
#include "miniz.h"
#include "picojson.h"

namespace standardar
{
    CReferenceObjectDatabase::CReferenceObjectDatabase()
    {
        m_ObjectIndexBeforeUpdate = 0;
    }

    CReferenceObjectDatabase::~CReferenceObjectDatabase()
    {
        for(CObjectNode* pObject:m_ReferenceObjectDatabaseList)
        {
            if(pObject) {
                delete pObject;
                pObject = nullptr;
            }
        }
        m_ReferenceObjectDatabaseList.clear();
    }

    CObjectNode* CReferenceObjectDatabase::getReferenceObjectById(int32_t id)
    {
        for (CObjectNode* pObject:m_ReferenceObjectDatabaseList) //c++11
        {
            if (pObject->getIndex() == id)
                return pObject;
        }
        return nullptr;
    }

    void CReferenceObjectDatabase::deserialize(const uint8_t *bytes, int64_t size)
    {

    }

    void CReferenceObjectDatabase::loadConfigure(const char* config)
    {

    }

    void CReferenceObjectDatabase::addReferenceObject(const char *object_name, const uint8_t *object_raw_bytes, int64_t object_raw_bytes_size, int32_t *out_index)
    {
        int index = m_ReferenceObjectDatabaseList.size();
        *out_index = index;
        CObjectNode* object = new CObjectNode(index, object_name);
        object->m_bufferSize = (int)object_raw_bytes_size;
        object->m_buffer = (unsigned char*)malloc(object_raw_bytes_size);
        memcpy(object->m_buffer, object_raw_bytes, object_raw_bytes_size);
        m_ReferenceObjectDatabaseList.push_back(object);
    }

    void CReferenceObjectDatabase::getObjectData(jbyte* data_ptr, int length, int order)
    {
        DirectByteBufferWrapper data_buffer(data_ptr, length, (BUFFER_ORDER) order);
        data_buffer.put<int>(SEND_MAGIC_WORD);
        data_buffer.put<int>(OBJECT_TRACKING_VERSION);
        int num = m_ReferenceObjectDatabaseList.size() - m_ObjectIndexBeforeUpdate;
        data_buffer.put<int>(num);
        for (int i=m_ObjectIndexBeforeUpdate; i<m_ReferenceObjectDatabaseList.size(); ++i) {
            data_buffer.put<int>(m_ReferenceObjectDatabaseList[i]->m_ObjectId);
            data_buffer.put<int>(m_ReferenceObjectDatabaseList[i]->m_bufferSize);
            data_buffer.put(m_ReferenceObjectDatabaseList[i]->m_buffer, m_ReferenceObjectDatabaseList[i]->m_bufferSize);
        }
    }

    int CReferenceObjectDatabase::getObjectDataSize()
    {
        int size = 0;
        for (int i=m_ObjectIndexBeforeUpdate; i<m_ReferenceObjectDatabaseList.size(); ++i) {
            size += m_ReferenceObjectDatabaseList[i]->m_bufferSize;
        }

        return size;
    }

    int CReferenceObjectDatabase::getObjectNum()
    {
        return m_ReferenceObjectDatabaseList.size();
    }

    int CReferenceObjectDatabase::getUpdateObjectNum()
    {
        return m_ReferenceObjectDatabaseList.size() - m_ObjectIndexBeforeUpdate;
    }

    void CReferenceObjectDatabase::setObjectDatabase(CReferenceObjectDatabase* database)
    {
        m_ReferenceObjectDatabaseList.clear();
        for (CObjectNode* pObject:database->m_ReferenceObjectDatabaseList)
        {
            CObjectNode* tmpObject = new CObjectNode(pObject);
            m_ReferenceObjectDatabaseList.push_back(tmpObject);
        }
    }

    void CReferenceObjectDatabase::getObjectDatabase(CReferenceObjectDatabase* database)
    {
        database->m_ReferenceObjectDatabaseList.clear();
        for (CObjectNode* pObject:m_ReferenceObjectDatabaseList)
        {
            CObjectNode* tmpObject = new CObjectNode(pObject);
            database->m_ReferenceObjectDatabaseList.push_back(tmpObject);
        }
    }
}
