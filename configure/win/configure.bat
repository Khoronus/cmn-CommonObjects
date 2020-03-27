REM Set the variable values with the default values
set path_opencv_tmp=D:/src/opencv_3.4/build
set path_boost_tmp=C:/local/boost
set path_boost_lib_tmp=C:/local/boost/lib/x64/lib
set path_realsense_tmp=D:/workspace/programs/ThirdPartyLib/librealsense
set path_eigen_tmp=D:/workspace/programs/ThirdPartyLib/eigen
set path_dlib_tmp=D:/workspace/programs/ThirdPartyLib/dlib/dlib
set path_yolo_tmp=D:/workspace/programs/ThirdPartyLib/darknet
set path_yolo_lib_tmp=D:/workspace/programs/ThirdPartyLib/darknet/build/release
set path_cuda_tmp=C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v10.0
set path_openpose_tmp=D:/workspace/programs/ThirdPartyLib/openpose
set path_concurrentqueue_tmp=D:/workspace/programs/ThirdPartyLib/concurrentqueue
set path_storedata_tmp=D:/workspace/programs/MyLib/StoreData
set path_cmnuniverse_tmp=D:/workspace/programs/MyLib/CmnUniverse
set path_common_tmp=D:/workspace/programs/MyLib/CommonObjects
set path_cvplot_tmp=D:/workspace/programs/ThirdPartyLib/cvplot
set path_freeglut_tmp=D:/workspace/programs/ThirdPartyLib/freeglut-3.0.0
set path_skeleton_tmp=D:/workspace/programs/MyProg/HumanBodyAnalyzer/src/HumanBodyAnalyzer

@echo off
ECHO set opencv path (i.e. %path_opencv_tmp%)
set /p path_opencv_tmp="Enter path: "
ECHO set boost path (i.e. %path_boost_tmp%)
set /p path_boost_tmp="Enter path: "
ECHO set boost_lib path (i.e. %path_boost_lib_tmp%)
set /p path_boost_lib_tmp="Enter path: "
ECHO set realsense path (i.e. %path_realsense_tmp%)
set /p path_realsense_tmp="Enter path: "
ECHO set eigen path (i.e. %path_eigen_tmp%)
set /p path_eigen_tmp="Enter path: "
ECHO set dlib path (i.e. %path_dlib_tmp%)
set /p path_dlib_tmp="Enter path: "
ECHO set yolo path (i.e. %path_yolo_tmp%)
ECHO link https://github.com/AlexeyAB/darknet
set /p path_yolo_tmp="Enter path: "
ECHO set yolo lib path (i.e. %path_yolo_lib_tmp%)
set /p path_yolo_lib_tmp="Enter path: "
ECHO set cuda path (i.e. %path_cuda_tmp%)
set /p path_cuda_tmp="Enter path: "
ECHO set openpose path (i.e. %path_openpose_tmp%)
ECHO link https://github.com/CMU-Perceptual-Computing-Lab/openpose
set /p path_openpose_tmp="Enter path: "
ECHO set councurrentqueue path (i.e. %path_concurrentqueue_tmp%)
ECHO link https://github.com/cameron314/concurrentqueue
set /p path_concurrentqueue_tmp="Enter path: "
ECHO set storedata path (i.e. %path_storedata_tmp%)
set /p path_storedata_tmp="Enter path: "
ECHO set CmnUniverse path (i.e. %path_cmnuniverse_tmp%)
set /p path_cmnuniverse_tmp="Enter path: "
ECHO set common path (i.e. %path_common_tmp%)
set /p path_common_tmp="Enter path: "
ECHO set cvplot path (i.e. %path_cvplot_tmp%)
set /p path_cvplot_tmp="Enter path: "
ECHO link http://freeglut.sourceforge.net/index.php#download
ECHO set freeglut path (i.e. %path_freeglut_tmp%)
set /p path_freeglut_tmp="Enter path: "
ECHO set skeleton path
set /p path_skeleton_tmp="Enter path: "

REM Saved in path_configuration.txt
@echo path_opencv %path_opencv_tmp:\=/%> ../path_configuration.txt
@echo path_boost %path_boost_tmp:\=/%>> ../path_configuration.txt
@echo path_boost_lib %path_boost_lib_tmp:\=/%>> ../path_configuration.txt
@echo path_realsense %path_realsense_tmp:\=/%>> ../path_configuration.txt
@echo path_eigen %path_eigen_tmp:\=/%>> ../path_configuration.txt
@echo path_dlib %path_dlib_tmp:\=/%>> ../path_configuration.txt
@echo path_yolo %path_yolo_tmp:\=/%>> ../path_configuration.txt
@echo path_yolo_lib %path_yolo_lib_tmp:\=/%>> ../path_configuration.txt
@echo path_cuda %path_cuda_tmp:\=/%>> ../path_configuration.txt
@echo path_openpose %path_openpose_tmp:\=/%>> ../path_configuration.txt
@echo path_concurrentqueue %path_concurrentqueue_tmp:\=/%>> ../path_configuration.txt
@echo path_storedata %path_storedata_tmp:\=/%>> ../path_configuration.txt
@echo path_cmnuniverse %path_cmnuniverse_tmp:\=/%>> ../path_configuration.txt
@echo path_common %path_common_tmp:\=/%>> ../path_configuration.txt
@echo path_cvplot %path_cvplot_tmp:\=/%>> ../path_configuration.txt
@echo path_freeglut %path_freeglut_tmp:\=/%>> ../path_configuration.txt
@echo path_skeleton %path_skeleton_tmp:\=/%>> ../path_configuration.txt