@echo off
echo body_detect_c
echo %cd%
set path=%cd%

cd %path%\..\released\samples\body_detect_c
echo %cd%
gradlew clean && gradlew makeApk && cd %path% && echo %cd%
