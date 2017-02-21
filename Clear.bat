@echo off

set current_dir=%~dp0
set sln_dir=%current_dir%

::删除Build目录
rmdir /S /Q %sln_dir%\Build

::删除Debug目录
del %sln_dir%\Bin\Debug\*.exe
del %sln_dir%\Bin\Debug\*.dll
del %sln_dir%\Bin\Debug\*.ilk

::提交
::git commit -m "temp"

::打包
cd %sln_dir%
::git archive -o %current_dir%\UI2017.zip HEAD
"C:\Program Files (x86)\WinRAR\RAR.exe" a -k -r -s -m1 -x.git -x.vs %sln_dir%UI2017.zip 


:END

pause