REM location is set with the current called batch location. If different, please change
SET "location=%cd%\..\.."
REM SET "build=C:\Program Files (x86)\MSBuild\14.0\Bin\msbuild.exe"
SET "build=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\msbuild.exe"
SET "params_debug=/t:build /m:8 /p:RunCodeAnalysis=false;BuildInParallel=True /v:n /property:Configuration=Debug /property:Platform=x64"
SET "params_release=/t:build /m:8 /p:RunCodeAnalysis=false;BuildInParallel=True /v:n /property:Configuration=Release /property:Platform=x64"

REM call the build function
call "2. build_slns_list.bat"
