<br/>

<div align="center">
    <img src="docs/en/assets/XRAPI.png" width="600"/>
</div>
<br/>

<div align="left">
<div align="left">


[![LICENSE](https://img.shields.io/github/license/openxrlab/xrapi)](https://github.com/openxrlab/xrapi/blob/main/LICENSE)

</div>

## Introduction

OpenXRLab XRAPI is an open source implementation of the 《信息技术 移动设备增强现实系统应用接口》(Information technology - Application interfaces of augmented reality system for mobile devices). This standard specifies the data types and interface definitions for mobile device augmented reality system application development, and be used to guide the design, development and implementation of mobile device augmented reality systems. XIAPI is part of the OpenXRLab project. We hope that XIAPI can connect industry and academia, give full play to their respective advantages, and form a closed loop to promote ecological development.

https://user-images.githubusercontent.com/44204704/187863580-e3a1869e-0ff2-4319-8e86-9c669f2b15ef.mp4

https://user-images.githubusercontent.com/44204704/187863786-efbc3804-a4d8-4727-a7e2-ea45744330e4.mp4

### **Major Features**

The augmented reality system on mobile devices consists of modules such as real-time tracking and positioning, scale estimation, landmark recognition and tracking, 3D reconstruction, illumination estimation, face alignment, gesture recognition etc. 

XRAPI includes the following interface modules: 
* Real-time tracking and positioning: Use input data from mobile devices to calculate 6DoF pose.
* Illumination estimation: Use video streams to calculate illumination information.
* Scale estimation: Use input data to calculate scale information about the physical world.
* Marker recognition and tracking: Marker recognition and tracking based on video streams.
* 3D reconstruction: Use 6DoF pose, scale information and depth data to reconstruct the 3D model of the scene.
* Face alignment: Predicting a set of facial keypoints from a video stream.
* Gesture recognition: Detect hand bounding boxes and classify gestures based on video streams.

## Introduction to standards

### Information technology - Application interfaces of augmented reality system for mobile devices

* Scope of application: The standard specifies the data types and interfaces for mobile device augmented reality system application development.

* Main issues to be solved: The application interface of the mobile device augmented reality system is a bridge that realizes the interaction between the device operating system, the augmented reality engine and the application program. The application interface is an indispensable basic link to realize the augmented reality function on the mobile device.In order to further improve the compatibility of mobile device augmented reality systems and guide the design, development and application of mobile device augmented reality systems, this standard specifies the data types and interface definitions for mobile device augmented reality system application development.

## Installation

We provide detailed [installation tutorial](./docs/en/installation.md) for XRAPI.

## Getting Started

Please refer to [quick start](docs/en/xrapi_c_tutorial.md) for the basic usage of XRAPI.

## License

The license of our codebase is [Apache-2.0](LICENSE). Note that this license only applies to code in our library, the dependencies of which are separate and individually licensed. We would like to pay tribute to open-source implementations to which we rely on. Please be aware that using the content of dependencies may affect the license of our codebase. Some supported methods may carry [additional licenses](docs/en/additional_licenses.md).


## Citation

If you use this toolbox or benchmark in your research, please cite this project.

```bibtex
@misc{xrapi,
    title={OpenXRLab application interfaces of augmented reality system for mobile devices},
    author={XRAPI Contributors},
    howpublished = {\url{https://github.com/openxrlab/xrapi}},
    year={2024}
}
```

## Contributing

We appreciate all contributions to improve XRAPI.
Please refer to [CONTRIBUTING.md](.github/CONTRIBUTING.md) for the contributing guideline.

## Acknowledgement

XRAPI is an open source project that is contributed by researchers and
engineers from both the academia and the industry.
We appreciate all the contributors who implement their methods or add new features,
as well as users who give valuable feedbacks.
We hope that XIAPI can connect industry and academia, give full play to their respective advantages, and form a closed loop to promote ecological development.


## Projects in OpenXRLab

- [XRAPI](https://github.com/openxrlab/xrapi): OpenXRLab application interfaces of augmented reality system for mobile devices.
- [XRPrimer](https://github.com/openxrlab/xrprimer): OpenXRLab foundational library for XR-related algorithms.
- [XRSLAM](https://github.com/openxrlab/xrslam): OpenXRLab Visual-inertial SLAM Toolbox and Benchmark.
- [XRSfM](https://github.com/openxrlab/xrsfm): OpenXRLab Structure-from-Motion Toolbox and Benchmark.
- [XRLocalization](https://github.com/openxrlab/xrlocalization): OpenXRLab Visual Localization Toolbox and Server.
- [XRMoCap](https://github.com/openxrlab/xrmocap): OpenXRLab Multi-view Motion Capture Toolbox and Benchmark.
- [XRMoGen](https://github.com/openxrlab/xrmogen): OpenXRLab Human Motion Generation Toolbox and Benchmark.
- [XRNeRF](https://github.com/openxrlab/xrnerf): OpenXRLab Neural Radiance Field (NeRF) Toolbox and Benchmark.
