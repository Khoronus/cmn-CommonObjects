REM location is set with the current called batch location. If different, please change
SET "location=%cd%\..\.."
SET "build_folder=build"
SET "compiler_version=Visual Studio 17 2022"
SET "compiler_option=x64"

REM run the list of batch files
call "1. create_slns_list.bat"
