REM IMPORTANT: The build order is important to prevent errors

REM location is set with the current called batch location. If different, please change
SET "location=%cd%"
SET "build_folder=build"
REM SET "compiler_version=Visual Studio 14 2015 Win64"
REM SET "compiler_version=Visual Studio 15 2017 Win64"
SET "compiler_version=Visual Studio 16 2019"
SET "compiler_option=x64"

REM SET "build=C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild.exe"
REM SET "build=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\msbuild.exe"
SET "build=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\msbuild.exe"
SET "params_debug=/t:build /m:8 /p:RunCodeAnalysis=false;BuildInParallel=True /v:n /property:Configuration=Debug /property:Platform=x64"
SET "params_release=/t:build /m:8 /p:RunCodeAnalysis=false;BuildInParallel=True /v:n /property:Configuration=Release /property:Platform=x64"


REM Create a log build file
@echo "Projects build result [==0 success, >0 error]" > "build_result.txt"

REM Create CmnLib
cd "%location%\..\..\sample"
call create_sln.bat
call :build_function "%build%" "%location%\..\..\sample\build\CommonObjectSample.sln" "%params_debug%" "build_result.txt"
call :build_function "%build%" "%location%\..\..\sample\build\CommonObjectSample.sln" "%params_release%" "build_result.txt"

REM Skip the function
goto:eof

REM function to build a process
:build_function
"%~1" %~2 %~3
cd "%location%
echo "%~2" Exit Code is %errorlevel% >> %~4
goto:eof


