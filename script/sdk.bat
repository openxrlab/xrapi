@echo off
echo sdk
echo %cd%
set path=%cd%


cd ..\standardar\standardar_aar\
gradlew clean && gradlew assembleRelease makeSDK && cd %path% && echo %cd%
