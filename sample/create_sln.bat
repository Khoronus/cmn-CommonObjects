mkdir %build_folder%
cd %build_folder%
"C:\Program Files\CMake\bin\cmake.exe" -G "%compiler_version%" -DCMAKE_SYSTEM_VERSION=10 ..
