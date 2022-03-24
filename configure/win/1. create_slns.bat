REM location is set with the current called batch location. If different, please change
SET "location=%cd%"
SET "build_folder=build"
REM SET "compiler_version=Visual Studio 14 2015 Win64"
SET "compiler_version=Visual Studio 15 2017"
SET "compiler_option=x64"

REM Create the C++ solutions
cd "%location%\..\.."
call create_sln.bat