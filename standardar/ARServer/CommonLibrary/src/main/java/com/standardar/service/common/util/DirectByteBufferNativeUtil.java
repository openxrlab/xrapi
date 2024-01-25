package com.standardar.service.common.util;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class DirectByteBufferNativeUtil {
    static {
        System.loadLibrary("common_lib");
    }

    public static native void copyBuffer(ByteBuffer dst, int dstPos, ByteBuffer src, int srcPos, int length);

    public static native void startCollect(ByteBuffer buffer, int order,
                                           int[] tags, long[] callbackHandler);

    public static int order(ByteBuffer buffer) {
        if (buffer == null) {
            return 0;
        }
        return buffer.order() == ByteOrder.BIG_ENDIAN ? 0 : 1;
    }
}
