@echo off
echo server
echo %cd%
set path=%cd%


cd ..\standardar\ARServer\
gradlew clean && gradlew makeFullServer && cd %path% && echo %cd%
