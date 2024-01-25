#include "com_standardar_common_Util.h"

#include <string.h>
#include "ArCommon.h"
#include "log.h"
#include "NativeClient.h"
#include <unistd.h>
#include <sys/syscall.h>

//#include "arm_neon.h"
//JNIEXPORT void JNICALL Java_com_standardar_common_Util_cropImageNv12(JNIEnv *env,
//                                                                     jobject self,
//                                                                     jobject buf,
//                                                                     jint width,
//                                                                     jint height,
//                                                                     jint stride,
//                                                                     jint scanline,
//                                                                     jbyteArray out_)
//{
//    void *image=env->GetDirectBufferAddress(buf);
//    jbyte *out = env->GetByteArrayElements(out_, NULL);
//
//    int i,j;
//    unsigned char* image_y = (unsigned char*)image;
//    unsigned char* image_uv = image_y + stride * scanline;
//    unsigned char *out_image_y = (unsigned char *)out;
//    unsigned char *out_image_uv = out_image_y + width * height;
//
//    for (i = 0; i < height; i++) {
//
//        int offsetSrc = i * stride + 68;
//        int offsetDst = i * width;
//        int uv_offsetSrc = (i >> 1) * stride + 68;
//        int uv_offsetDst = (i >> 1) * width;
//
//        unsigned char *out_uv = out_image_uv + uv_offsetDst;
//        unsigned char *in_uv = image_uv + uv_offsetSrc;
//        unsigned char *out_y = out_image_y + offsetDst;
//        unsigned char *in_y = image_y + offsetSrc;
//
//        for(j =0 ; j<= (width - 16); j+=16) {
//
//            uint8x16_t src_8x16 = vld1q_u8(in_y + j);
//            vst1q_u8(out_y + j , src_8x16);
//
//            if (i % 2 == 0) {
//                uint8x16_t src_uv_8x16 = vld1q_u8(in_uv + j);
//                uint8x16_t dst_uv_8x16 = vrev16q_u8 (src_uv_8x16);
//                vst1q_u8(out_uv + j,dst_uv_8x16);
//            }
//        }
//        for(;j < width; j++)
//        {
//            out_y[j] = in_y[j];
//            if (i % 2 == 0 && j % 2 == 0) {
//                out_uv[j] = in_uv[j + 1];
//                out_uv[j + 1] = in_uv[j];
//            }
//        }
//    }
//
//    env->ReleaseByteArrayElements(out_, out, 0);
//}


JNIEXPORT void JNICALL Java_com_standardar_common_Util_copyImage(JNIEnv* env, jobject obj,
                                                                 jobject dst, jint order, jint pos,
                                                                 jobject src, jint rgb_size, jint depth_size)
{
    jbyte* dst_ptr = (jbyte*)env->GetDirectBufferAddress(dst);
    DirectByteBufferWrapper dst_buffer(dst_ptr, env->GetDirectBufferCapacity(dst), (BUFFER_ORDER)order);
    dst_buffer.position(pos);
    jbyte* src_ptr = (jbyte*)env->GetDirectBufferAddress(src);
    dst_buffer.put(src_ptr, rgb_size);
    if (depth_size != 0) {
        dst_buffer.put<int>(depth_size);
        dst_buffer.put(src_ptr + rgb_size, depth_size);
    }
}

JNIEXPORT void JNICALL Java_com_standardar_common_Util_copyBuffer(JNIEnv* env, jobject obj,
                                                                  jobject dst_buffer, jint dst_pos,
                                                                  jobject src_buffer, jint src_pos, jint length)
{

    jbyte* dst_ptr = (jbyte*)env->GetDirectBufferAddress(dst_buffer);
    jbyte* src_ptr = (jbyte*)env->GetDirectBufferAddress(src_buffer);
    memcpy(dst_ptr + dst_pos, src_ptr + src_pos, length);
}

JNIEXPORT jstring JNICALL Java_com_standardar_common_Util_getXRServerPkg(JNIEnv* env, jobject obj)
{
    std::string brand;
    getPhoneBrand(brand);
    return env->NewStringUTF("com.standardar.xrserver");
}

JNIEXPORT void JNICALL Java_com_standardar_common_Util_bindCore(JNIEnv* env, jobject obj)
{
    int standard = 1;
    int num = sysconf(_SC_NPROCESSORS_CONF);
    for(int i=1; i<num; ++i){
        standard = (standard << 1) | 1;
    }

    int mask;
    syscall(__NR_sched_getaffinity, 0, sizeof(int), &mask);

    if(standard != mask){
        syscall(__NR_sched_setaffinity, 0, sizeof(int), &standard);
    }
}


