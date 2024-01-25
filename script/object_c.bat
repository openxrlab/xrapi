@echo off
echo %cd%
set path=%cd%

cd %path%\..\released\samples\hello_object_c
echo %cd%
gradlew clean && gradlew makeApk && cd %path% && echo %cd%
