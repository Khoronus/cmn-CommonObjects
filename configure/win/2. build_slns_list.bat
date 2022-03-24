REM Create a log build file
@echo "Projects build result [==0 success, >0 error]" > "build_result.txt"
call :build_function "%build%" "D:\workspace\programs\ThirdPartyLib\cmn-CommonObjects\sample\build\CommonObjectSample.sln" "%params_debug%" "build_result.txt"
call :build_function "%build%" "D:\workspace\programs\ThirdPartyLib\cmn-CommonObjects\sample\build\CommonObjectSample.sln" "%params_release%" "build_result.txt"
REM Skip the function
goto:eof
REM function to build a process
:build_function
"%~1" %~2 %~3
echo "%~2" Exit Code is %errorlevel% >> %~4
goto:eof
