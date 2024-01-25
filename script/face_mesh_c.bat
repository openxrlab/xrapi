@echo off
echo face_mesh_c
echo %cd%
set path=%cd%

cd %path%\..\released\samples\face_mesh_c
echo %cd%
gradlew clean && gradlew makeApk && cd %path% && echo %cd%
