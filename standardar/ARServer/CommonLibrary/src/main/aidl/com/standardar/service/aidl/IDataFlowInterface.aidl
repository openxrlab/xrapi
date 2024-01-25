package com.standardar.service.aidl;

import com.standardar.service.aidl.IDataFlowListener;
import android.os.SharedMemory;

interface IDataFlowInterface {
    byte[] processFrame(in byte[] frameData);
    byte[] processFrameShareMemory(in ParcelFileDescriptor frameDataDescriptor, int length);
    void setupSharedMemory(in SharedMemory sharedMemory, int prot);
    byte[] processFrameShareMemoryV27();
    int sendCommand(in int commandId, in byte[] message);
    void processFrameShareMemoryV27Callback(IDataFlowListener callback);
    void sendCommandSharedMemoryV27(in int cmdid, in SharedMemory sharedMemory, IDataFlowListener callback);
    oneway void sendCommandSharedMemoryV27Async(in int cmdid, in SharedMemory sharedMemory, IDataFlowListener callback);
}
