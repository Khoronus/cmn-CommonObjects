REM Set the variable values with the global projects default values
set path_global_tmp=D:/workspace/programs/MyLib/global-Configuration/to_compile/configure

@echo off
ECHO set global project path (i.e. %path_global_tmp%)
set /p path_global_tmp="Enter path: "

REM Saved in global_path_configuration.txt
@echo path_global %path_global_tmp:\=/%> ../global_path_configuration.txt
