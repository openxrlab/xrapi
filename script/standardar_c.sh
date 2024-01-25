cd ../released/samples/hello_standardar_c
echo "start make standardar c"

./gradlew clean && ./gradlew makeApk
git checkout local.properties
cd -
