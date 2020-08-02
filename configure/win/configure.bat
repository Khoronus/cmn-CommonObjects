REM Set the variable values with the default values
set path_opencv_tmp=C:/src/opencv/build
set path_boost_tmp=C:/local/boost
set path_boost_lib_tmp=C:/local/boost/lib/x64/lib
set path_storedata_tmp=D:/workspace/programs/MyLib/cmn-StoreData
set path_cmnuniverse_tmp=D:/workspace/programs/MyLib/cmn-CmnUniverse
set path_common_tmp=D:/workspace/programs/MyLib/cmn-CommonObjects

@echo off
ECHO set opencv path (i.e. %path_opencv_tmp%)
set /p path_opencv_tmp="Enter path: "
ECHO set boost path (i.e. %path_boost_tmp%)
set /p path_boost_tmp="Enter path: "
ECHO set boost_lib path (i.e. %path_boost_lib_tmp%)
set /p path_concurrentqueue_tmp="Enter path: "
ECHO set storedata path (i.e. %path_storedata_tmp%)
set /p path_storedata_tmp="Enter path: "
ECHO set CmnUniverse path (i.e. %path_cmnuniverse_tmp%)
set /p path_cmnuniverse_tmp="Enter path: "
ECHO set common path (i.e. %path_common_tmp%)

REM Saved in path_configuration.txt
@echo path_opencv %path_opencv_tmp:\=/%> ../path_configuration.txt
@echo path_boost %path_boost_tmp:\=/%>> ../path_configuration.txt
@echo path_boost_lib %path_boost_lib_tmp:\=/%>> ../path_configuration.txt
@echo path_storedata %path_storedata_tmp:\=/%>> ../path_configuration.txt
@echo path_cmnuniverse %path_cmnuniverse_tmp:\=/%>> ../path_configuration.txt
@echo path_common %path_common_tmp:\=/%>> ../path_configuration.txt
