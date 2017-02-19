@echo off

set current_dir=%~dp0
set sln_dir=%current_dir%

::删除Build目录
::rmdir /S /Q %sln_dir%\Build

::删除Debug目录
::del %sln_dir%\Bin\Debug\*.exe
::del %sln_dir%\Bin\Debug\*.dll
::del %sln_dir%\Bin\Debug\*.ilk

::打包
cd %sln_dir%
git archive -o %current_dir%\UI2017.zip HEAD


:END

pause