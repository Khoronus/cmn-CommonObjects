REM location is set with the current called batch location. If different, please change
SET "location=%cd%\..\.."
SET "build_folder=build"
REM SET "compiler_version=Visual Studio 14 2015 Win64"
REM SET "compiler_version=Visual Studio 15 2017 Win64"
SET "compiler_version=Visual Studio 16 2019"
SET "compiler_option=x64"

REM run the list of batch files
call "1. create_slns_list.bat"
