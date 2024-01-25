
#ifndef ARSERVER_COMMONLIB_H
#define ARSERVER_COMMONLIB_H

#include <functional>
#include <assert.h>
#include <jni.h>
#include <string.h>
enum BUFFER_ORDER {
    BUFFER_BIG_ENDIAN = 0,
    BUFFER_LITTLER_ENDIAN = 1
};

class DirectByteBufferWrapper{
public:
    DirectByteBufferWrapper(void* ptr, int cap, BUFFER_ORDER order) :
            m_Pos(0),
            m_Order(order){
        m_Buffer_Ptr = (jbyte*)ptr;
        m_Capcity = cap;
        m_Native_Order = checkNativeOrder();
    }
    template <typename T>
    T get() {
        T* tPtr = (T*)(offset());
        m_Pos += sizeof(T);
        return toNativeOrder(*tPtr);
    }
    template <typename T>
    void put(T value) {
        int size = sizeof(T);
        jbyte* iPtr = offset();
        m_Pos += sizeof(T);
        memset(iPtr, 0, sizeof(T));
        int64_t v = 0;
        memcpy(&v, &value, size);
        int offset = size * 8 - 8;
        if (m_Order == BUFFER_BIG_ENDIAN) {
            for (int i = 0; i < size; ++i) {
                *iPtr |= ((v >> (offset - i * 8)) & 0xff);
                iPtr++;
            }
        } else if (m_Order == BUFFER_LITTLER_ENDIAN) {
            for (int i = 0; i < size; ++i) {
                *iPtr |= ((v >> (i * 8)) & 0xff);
                iPtr++;
            }
        }

    }

    template <typename T>
    T toNativeOrder(T value) {
        int64_t v = 0;
        memcpy(&v, &value, sizeof(T));
        int64_t* iPtr = &v;
        if (m_Native_Order == m_Order) {
            return value;
        } else if (m_Native_Order != m_Order) {
            int size = sizeof(T);
            int offset = size * 8 - 8;
            int64_t res = 0;
            for (int i = 0; i < size; ++i) {
                res |= ((*iPtr >> (offset - i * 8)) & 0xFF) << (i * 8);
            }
            return *((T*)&res);
        }
        return 0;
    }

    void get(void* dst, size_t size) {
        memcpy(dst, offset(), size);
        m_Pos += size;
    }

    void put(void* src, size_t size) {
        if (src == nullptr)
            return;
        memcpy(offset(), src, size);
        m_Pos += size;
    }
    void rewind() {
        m_Pos = 0;
    }
    int capacity() {
        return m_Capcity;
    }
    int remaining() {
        return m_Capcity - m_Pos;
    }
    int position() {
        return m_Pos;
    }
    void position(int pos) {
        if (pos < m_Capcity) {
            m_Pos = pos;
        }
    }
    jbyte* pointerOffset() {
        return offset();
    }
private:
    int m_Pos;
    int m_Native_Order;
    jbyte* m_Buffer_Ptr;
    BUFFER_ORDER m_Order;
    int m_Capcity;
    jbyte* offset() {
        assert(m_Pos < m_Capcity);
        return m_Buffer_Ptr + m_Pos;
    }
    BUFFER_ORDER checkNativeOrder() {
        int a = 0x12345678;
        jbyte* aPtr = (jbyte*)&a;
        if (*aPtr == 0x78)
            return BUFFER_LITTLER_ENDIAN;
        else
            return BUFFER_BIG_ENDIAN;
    }
};

typedef void(*resutl_callback)(DirectByteBufferWrapper* buffer);
typedef resutl_callback  ALGORITHM_RESULT_FILL_FUNC;
void collectResult(JNIEnv* env, jobject buffer, int order, jintArray tags, jlongArray callbackHandler);

#endif //ARSERVER_COMMONLIB_H
