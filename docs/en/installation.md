# Installation

## Build from source

Only specific version of Android Studio and NDK can compile the source.

* Android Studio Arctic Fox | 2020.3.1 Patch 2
* NDK 17.2.4988734
* CMake 3.6.4111459

To run slam demo. Please open the following three projects with Android Studio to make the compilation environment be configured correctly.

* standardar/standardar_aar
* ARServer
* released/hello_standardar_c

Every project has a file named local.properties after opening the project. you should configure the sdk and ndk manually.

```
sdk.dir=D\:\\Library\\Android\\sdk
ndk.dir=D\:\\Library\\Android\\sdk\\ndk\\17.2.4988734
```

If you encounter network problems, configure a vpn.

Then you can use the script to build two apk.

* script/sdk.bat: build `standardar.aar`. this aar will be used in hello_standardar_c.
* script/serverfull.bat: build ARServer apk. This apk is generate at released/sdk/apk/Standardar_Server.apk
* script/hello_standardar_c.bat: build SLAM apk. This apk is generate at released/sdk/apk/hello_standardar_c.apk

Then installl `Standardar_Server.apk` and `hello_standardar_c.apk`. Start `hello_standardar_c` to run SLAM.
