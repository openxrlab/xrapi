@echo off
echo ar_c
echo %cd%
set path=%cd%

cd %path%\..\released\samples\hello_densemesh_c
echo %cd%
gradlew clean && gradlew makeApk && cd %path% && echo %cd%