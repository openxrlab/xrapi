package com.standardar.service.aidl;

import android.os.SharedMemory;

interface IDataFlowListener {
    void onResponse(in SharedMemory sharedMemory);
}
