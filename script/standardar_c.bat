@echo off
echo standardar_c
echo %cd%
set path=%cd%

cd %path%\..\released\samples\hello_standardar_c
echo %cd%
gradlew clean && gradlew makeApk && cd %path% && echo %cd%
