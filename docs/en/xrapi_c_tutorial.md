# 		StandardAR C API Tutorial

# 1 Overview
Hello StandardAR C using the StandardAR C API shows the effect of SLAM Algorithm。

# 2 FileList
| Class              | Function                                                     |
| ------------------ | ------------------------------------------------------------ |
| BackgroundRenderer | Used for drawing the AR background。                         |
| HelloArApplication | Used for handling application logic, including startup, update, pause, etc. |
| ObjRenderer        | Used for drawing virtual object。                            |
| PlaneRenderer3D    | Used for drawing planes detected by algorithm。              |
| PointCloudRenderer | Used for drawing the point cloud。                           |

# 3 Project setting
There are two ways to use the SDK: 1) refer to the libstandardar.so and standardar.jar packages; 2) refer to the libstandardar.aar packages. The following shows the first configuration method.

Create the libs directory under the app directory, put libstandardar.so and standardar.jar into the libs directory, and add the following config into Gradle file.

```c++
sourceSets {
    main {
        // let gradle pack the shared library into apk
        jniLibs.srcDirs = ['libs']
    }
}
```
Add includes file in sdk directory into CMakeLists.txt.
```c++
target_include_directories(hello_ar_native PRIVATE
            ${SRCDIR}/../../../sdk/includes)

```
# 4 Project runtime
## **4.1 Java runtime environment**
Use `arJavaEnvOnLoad` function to initialize the java running environment when the `JNI_Onload`function is called in Android.

## **4.2 Initialization**
Create `ARSession`、`ARMap`和`ARFrame` during Initialization.

`ARSession`: the management object of the entire augmented reality session. User can get `ARFrame`object use the `ARSession`.

`ARFrame`:the management object of current frame. User can get the camera's pose of current frame.

```c++
CHECK(arSessionCreate(env, context, &ar_session_) == ARRESULT_SUCCESS);
CHECK(ar_session_);

arSessionAcquireWorldMap(ar_session_, &ar_map_);

ARConfig *ar_config = nullptr;
arConfigCreate(&ar_config);

CHECK(arSessionConfigure(ar_session_, ar_config) == ARRESULT_SUCCESS);

arConfigDestroy(ar_config);

arFrameCreate(ar_session_, &ar_frame_);
CHECK(ar_frame_);

```
## **4.3 Window setting**
Window settings are mainly used to set the size of the window display area to prevent the acquired texture from being stretched. On different mobile phones, the resolution of the picture frame may not necessarily be the same as the full screen resolution of the mobile phone. In order to achieve a better drawing effect, the example will calculate a middle area to display the unstretched picture (by leaving up and down or left and right area).

`arFrameGetImageResolution`: get the image resolution of the picture frame.

`arSessionSetDisplayGeometry`: get the size of the display area.

```c++
// get frame video image width and height
int w = 0;
int h = 0;
arFrameGetImageResolution(ar_session_, ar_frame_, &w, &h);
float ratio = (float)w / h;

// Calculate the appropriate size of the window display area
if (m_displayRotation == 0 || m_displayRotation == 2)
{
    m_ViewportWidth = width;
    m_ViewportHeight = width * ratio;
    glViewport(0, (height - m_ViewportHeight) / 2, m_ViewportWidth, m_ViewportHeight);
}
else if (m_displayRotation == 1 || m_displayRotation == 3)
{
    m_ViewportHeight = height;
    m_ViewportWidth = height * ratio;
    glViewport( (width - m_ViewportWidth) / 2 , 0, m_ViewportWidth, m_ViewportHeight);
}

m_ScreenAspect = ratio;

// set ar display size
LOGI("OnSurfaceChanged(%d, %d)", width, height);
LOGI("viewport size(%d, %d)", m_ScreenWidth, m_ScreenHeight);
arSessionSetDisplayGeometry(ar_session_, display_rotation, m_ViewportWidth, m_ViewportHeight);

```

## **4.4 Texture**
Setting the texture target is an internal method to quickly update the video image data to the texture. After setting, every time the update function is called, the opengl drawing method will be used to update the video image data to the texture target.
Users can also obtain video image data and convert it into texture data externally. However, the video image data defaults to YUV format and the conversion process is complicated. The method of setting the target texture provides a quick update method.
`arSessionSetCameraTextureName` is used to set the target texture, and `arFramePointQueryMode` is used to set the query mode of the click operation.
*Function of setting the target texture must be called in an environment with an opengl context.*

```c++
background_renderer_.InitializeGlContent();
arSessionSetCameraTextureName(ar_session_, background_renderer_.GetTextureId());

arFramePointQueryMode(ar_session_, ar_frame_, ARQUERY_MODE_POLYGON_PERSISTENCE);
```

## **4.5 Start**
Generally called in `resume` of Android activity

```c++
const ARResult status = arSessionResume(ar_session_);
CHECK(status == ARRESULT_SUCCESS);
```
## **4.6 Update**
Calling the function arSessionUpdate every frame to update the frame and various algorithm running information corresponding to the frame, such as: camera pose, recognized in-plane array, point cloud and other information. This information is obtained through ARFrame. If the target texture is set, this function will update the video image data into the target texture, so in this case, this function needs to be called in the opengl context.
```c++
// Update session to get current frame and render camera background.
if (arSessionUpdate(ar_session_, ar_frame_) != ARRESULT_SUCCESS) {
    LOGE("HelloArApplication::OnDrawFrame ArSession_update error");
}
```
## **4.7 Camera pose**
After updating, user can obtain camera pose information through `ARFrame`.

`arFrameAcquireCamera`: get `ARCamera` object.

`arCameraGetViewMatrix`: get the view matrix.

``arCameraGetProjectionMatrix`: get the projection matrix.

```c++
ARCamera *ar_camera;
arFrameAcquireCamera(ar_session_, ar_frame_, &ar_camera);

float znear = 0.1f;
float zfar = 100.0f;
glm::mat4 view_mat;
glm::mat4 projection_mat;
arCameraGetViewMatrix(ar_session_, ar_camera, glm::value_ptr(view_mat));
arCameraGetProjectionMatrix(ar_session_, ar_camera, znear, zfar,
                            glm::value_ptr(projection_mat));

ARTrackingState trackState;
arCameraGetTrackingState(ar_session_, ar_camera, &trackState);
```
## **4.8 Illuminance estimation**（Not ready）
Get the lighting information of real world。
```c++
// Get light estimation value.
ARIlluminationEstimate *ar_light_estimate;
ARIlluminationEstimateState ar_light_estimate_state;
arIlluminationEstimateCreate(ar_session_, &ar_light_estimate);

arFrameGetIlluminationEstimate(ar_session_, ar_frame_, ar_light_estimate);
arIlluminationEstimateGetState(ar_session_, ar_light_estimate,
                               &ar_light_estimate_state);

// Set light intensity to default. Intensity value ranges from 0.0f to 1.0f.
float light_intensity = 0.8f;
if (ar_light_estimate_state == ARILLUMINATION_ESTIMATE_STATE_VALID) {
    arIlluminationEstimateGetPixelIntensity(ar_session_, ar_light_estimate,
                                            &light_intensity);
}

arIlluminationEstimateDestroy(ar_light_estimate);
```
## **4.9 Anchor**
`ARAnchor` is a tracking point set in the map. The pose of the tracking point can be obtained through `ARAnchor`.

```c++
util::ScopedArPose pose(ar_session);
arAnchorGetPose(ar_session, ar_anchor, pose.GetArPose());
arPoseGetMatrix(pose.GetArPose(),  glm::value_ptr(*out_model_mat));
```
## **4.10 Nodes**（Not ready）
During the operation of the `SLAM` algorithm, various types of nodes will be identified, the most common of which are plane nodes. The plane node array can be obtained through the `arMapGetAllNodes` function. 
```c++
ARNodeList *plane_list = nullptr;
arNodeListCreate(ar_session_, &plane_list);
CHECK(plane_list != nullptr);

ARNodeType plane_tracked_type = ARNODE_TYPE_PLANE;
arWorldMapGetAllNodes(ar_session_, ar_map_, plane_tracked_type, plane_list);

int32_t plane_list_size = 0;
arNodeListGetSize(ar_session_, plane_list, &plane_list_size);

for (int i = 0; i < plane_list_size; ++i) {
    ARNode *ar_node = nullptr;
    arNodeListAcquireItem(ar_session_, plane_list, i, &ar_node);
    ARPlaneNode *ar_plane = ARNodeAsARPlaneNode(ar_node);

    const auto iter = plane_color_map_.find(i);
    glm::vec3 color;
    if (iter != plane_color_map_.end()) {
        color = iter->second;
    } else {
        color = GetRandomPlaneColor();
        plane_color_map_.insert({i, color});
    }
    plane_renderer_3d_.Draw(projection_mat, view_mat, ar_session_, ar_plane, color);

    arNodeRelease(ar_node);
}

arNodeListDestroy(plane_list);
```
## **4.11 PlaneNodes**（Not ready）
The plane node contains the normal information of the polygon mesh and the plane. The plane polygon is provided in the form of three-dimensional coordinates. At the same time, the normal of the plane node can be obtained to determine the orientation of the model. 
```c++
int32_t polygon_length;
arPlaneNodeGetPolygon3DSize(ar_session, ar_plane, &polygon_length);

if (polygon_length == 0) {
    //LOGE("PlaneRenderer3D::UpdatePlane, no valid plane polygon is found");
    return;
}

const int32_t vertices_size = polygon_length / 3;
std::vector<glm::vec3> raw_vertices(vertices_size);
arPlaneNodeGetPolygon3D(ar_session, ar_plane, glm::value_ptr(raw_vertices.front()));

// Fill vertex 0 to 3. Note that the vertex.xy are used for x and z
// position. vertex.z is used for alpha. The outter polygon's alpha
// is 0.
for (int32_t i = 0; i < vertices_size; ++i) {
vertices_.push_back(glm::vec4(raw_vertices[i].x, 
		     raw_vertices[i].y, raw_vertices[i].z, 0.0f));
}


util::ScopedArPose scopedArPose(ar_session);
arPlaneNodeGetCenterPose(ar_session, ar_plane, scopedArPose.GetArPose());
arPoseGetMatrix(scopedArPose.GetArPose(), glm::value_ptr(model_mat_));
arPlaneNodeGetNormal(ar_session, ar_plane, glm::value_ptr(normal_vec_));
```
## **4.12 point cloud**
For the current frame, there will be corresponding tracking and recognition point cloud data. `ARPointCloud` is obtained through the `arFrameAcquirePointCloud` function, and point cloud data is obtained through the `arPointCloudGetData` function. 

*Not thread safe. You need to ensure that it runs on the same thread as the update function.*

```c++
// Update and render point cloud.
ARPointCloud *ar_point_cloud = nullptr;
ARResult point_cloud_status = arFrameAcquirePointCloud(ar_session_, ar_frame_,
                                                       &ar_point_cloud);
if (point_cloud_status == ARRESULT_SUCCESS) {
    point_cloud_renderer_.Draw(projection_mat * view_mat, ar_session_, ar_point_cloud);
    arPointCloudRelease(ar_point_cloud);
}

int32_t number_of_points = 0;

// get point cloud data
arPointCloudGetNumberOfPoints(ar_session, ar_point_cloud, &number_of_points);
if (number_of_points <= 0) {
    return;
}

const float* point_cloud_data;
arPointCloudGetData(ar_session, ar_point_cloud, &point_cloud_data);
```
## **4.13 Pause**
After the system is paused, related data flows and related algorithms will be stopped. It is generally recommended to call it in `onPause` of the activity.

```c++
arSessionPause(ar_session_);
```
## **4.14 Exit**
When the system exits, the relevant memory wiil be released. 

```c++
arFrameDestroy(ar_frame_);
arWorldMapRelease(ar_map_);
arSessionDestroy(ar_session_);
```