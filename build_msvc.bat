@echo off
SETLOCAL EnableDelayedExpansion

rem BuildServer.bat
rem
rem This is the build script used for building SWGANH and it's dependencies
rem using Microsoft tools.

rem ----------------------------------------------------------------------------
rem ---Start of Main Execution -------------------------------------------------

rem Initialize environment variable defaults
call :SET_DEFAULTS

rem Process command line arguments first
goto :PROCESS_ARGUMENTS
:CONTINUE_FROM_PROCESS_ARGUMENTS

rem Build the environment and bail out if it fails
call :BUILD_ENVIRONMENT
if x%environment_built% == x goto :eof

cd %PROJECT_BASE%

call :BUILD_DEPENDENCIES

if %DEPENDENCIESONLY% == true goto :eof

call :BUILD_PROJECT

if not %ERRORLEVEL% == 0 (
if %HALT_ON_ERROR% == true (set /p halt=*** BUILD FAILED... PRESS ENTER TO CONTINUE ***)
exit /b %ERRORLEVEL%
)

echo.
echo Server Successfully Built^^!
echo This window will close shortly.
call :SLEEP 10

goto :eof
rem --- End of Main Execution --------------------------------------------------
rem ----------------------------------------------------------------------------

rem ----------------------------------------------------------------------------
rem --- Start of SET_DEFAULTS --------------------------------------------------
:SET_DEFAULTS

set VERSION=0.1.0
set DEPENDENCIES_FILE=libanh-deps-%VERSION%-win.7z
set DEPENDENCIES_URL=http://github.com/downloads/anhstudios/libanh/%DEPENDENCIES_FILE%
set "PROJECT_BASE=%~dp0"
set BUILD_TYPE=debug
set REBUILD=build
set DEPENDENCIESONLY=false
set BUILDNUMBER=0
set BUILD_ERROR=false
set HALT_ON_ERROR=true
set halt=

goto :eof
rem --- End of SET_DEFAULTS ----------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of PROCESS_ARGUMENTS ---------------------------------------------
:PROCESS_ARGUMENTS

if "%~0" == "" goto :CONTINUE_FROM_PROCESS_ARGUMENTS


if "%~0" == "-h" (
    echo msvc_build.cmd Help
    echo.
    echo "    /builddeps                     Builds only the project dependencies"
    echo "    /nohaltonerror                 Skips halting on errors"
    echo "    /rebuild                       Rebuilds the projects instead of incremental build"
    echo "    /clean                         Cleans the generated files"
    echo "    /build [debug-release-all]     Specifies the build type, defaults to debug"
    echo "    /buildnumber [num]             Specifies a build number to be set rather than commit hash"
)

if "%~0" == "/clean" (
	call :CLEAN_BUILD
	goto :eof
)

if "%~0" == "/builddeps" (
	set DEPENDENCIESONLY=true
)

if "%~0" == "/nohaltonerror" (
	set HALT_ON_ERROR=false
)

rem Check for /rebuild then set REBUILD
if "%~0" == "/rebuild" (
	set REBUILD=rebuild
)


rem Check for /buildnumber x format and then set BUILDNUMBER
if "%~0" == "/buildnumber" (
	set BUILDNUMBER=%~1
	shift
)

rem Check for /build:x format and then set BUILD_TYPE
if "%~0" == "/build" (
	set BUILD_TYPE=%~1
	shift
)

shift

goto :PROCESS_ARGUMENTS
rem --- End of PROCESS_ARGUMENTS -----------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of CLEAN_BUILD ---------------------------------------------------
rem --- Cleans all output created by the build process, restoring the        ---
rem --- project to it's original state like a fresh checkout.                ---
:CLEAN_BUILD

echo Cleaning the build environment
if exist "msvc\build-aux" rmdir /S /Q "msvc\build-aux"
if exist "msvc\Debug" rmdir /S /Q "msvc\Debug"
if exist "msvc\Release" rmdir /S /Q "msvc\Release"
if exist "msvc\DEPS" del /Q "msvc\DEPS"
if exist "boost" rmdir /S /Q "boost"
if exist "glog" rmdir /S /Q "glog"
if exist "gtest" rmdir /S /Q "gtest"
if exist "tbb" rmdir /S /Q "tbb"

goto :eof
rem --- End of CLEAN_BUILD -----------------------------------------------------
rem ----------------------------------------------------------------------------

rem ----------------------------------------------------------------------------
rem --- Start of BUILD_ENVIRONMENT ---------------------------------------------
:BUILD_ENVIRONMENT

if not exist "%VS100COMNTOOLS%" (
  set "VS100COMNTOOLS=%PROGRAMFILES(X86)%\Microsoft Visual Studio 10.0\Common7\Tools"
  if not exist "!VS100COMNTOOLS!" (
  	  set "VS100COMNTOOLS=%PROGRAMFILES%\Microsoft Visual Studio 10.0\Common7\Tools"
  	  if not exist "!VS100COMNTOOLS!" (          
  		    rem TODO: Allow user to enter a path to their base visual Studio directory.
         
    	    echo ***** Microsoft Visual Studio 10.0 required *****
    	    exit /b 1
  	  )
  )
)

set "MSBUILD=%WINDIR%\Microsoft.NET\Framework\v4.0.30319\msbuild.exe"

call "%VS100COMNTOOLS%\vsvars32.bat" >NUL

set environment_built=yes

goto :eof
rem --- End of BUILD_ENVIRONMENT -----------------------------------------------
rem ----------------------------------------------------------------------------

rem ----------------------------------------------------------------------------
rem --- Start of BUILD_DEPENDENCIES --------------------------------------------
rem --- Builds all external dependencies needed by the project.              ---
:BUILD_DEPENDENCIES

echo ** Building dependencies necessary for this project **
echo.

if not exist "boost" (
	echo ** Dependencies out of date -- Updating now **

	rem Need to do a full rebuild after updating dependenceies
	set REBUILD=rebuild
	
	call :CLEAN_BUILD
	call :DOWNLOAD_DEPENDENCIES
	echo ** Dependencies updated **
)

if not exist "msvc\DEPS" (
	echo ** Dependencies out of date -- Updating now **

	rem Need to do a full rebuild after updating dependenceies
	set REBUILD=rebuild
	
	call :CLEAN_BUILD
	call :DOWNLOAD_DEPENDENCIES
	echo ** Dependencies updated **
)

set /p dependencies_version=<"msvc\DEPS"

if not %dependencies_version% == %VERSION% (
	echo ** Dependencies out of date -- Updating now **

	rem Need to do a full rebuild after updating dependenceies
	set REBUILD=rebuild

	call :CLEAN_BUILD
	call :DOWNLOAD_DEPENDENCIES
	echo ** Dependencies updated **
)

if exist "boost" call :BUILD_BOOST
if exist "glog" call :BUILD_GLOG
if exist "gtest" call :BUILD_GTEST

echo ** Building dependencies complete **

goto :eof
rem --- End of BUILD_DEPENDENCIES ----------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of DOWNLOAD_DEPENDENCIES -----------------------------------------
rem --- Downloads the dependency package for the current version of the source -
:DOWNLOAD_DEPENDENCIES

if not exist "%DEPENDENCIES_FILE%" (
	"msvc\tools\wget.exe" !DEPENDENCIES_URL! -O "%DEPENDENCIES_FILE%"
)

if exist "%DEPENDENCIES_FILE%" (
	echo Extracting dependencies ...

	"msvc\tools\7z.exe" x -y "%DEPENDENCIES_FILE%"
	echo %VERSION% >"msvc\DEPS"
	echo Complete!
	echo.
)

goto :eof
rem --- End of DOWNLOAD_DEPENDENCIES -------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of BUILD_BOOST ---------------------------------------------------
rem --- Builds the boost library for use with this project.                  ---
:BUILD_BOOST

echo BUILDING: Boost - http://www.boost.org/

cd "%PROJECT_BASE%boost"

rem Only build boost if it hasn't been built already.
if "%BUILD_TYPE%" == "debug" (
	if exist "stage\lib\libboost_*-mt-gd.lib" (
		echo Boost libraries already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)
if "%BUILD_TYPE%" == "release" (
	if exist "stage\lib\libboost_*-mt.lib" (
		echo Boost libraries already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)
if "%BUILD_TYPE%" == "all" (
	if exist "stage\lib\libboost_*-mt-gd.lib" (
		if exist "stage\lib\libboost_*-mt.lib" (
			echo Boost libraries already built ... skipping
			echo.
			cd "%PROJECT_BASE%"
			goto :eof
		)
	)
)

rem Build BJAM which is needed to build boost.
if not exist "tools\jam\src\bin.ntx86\bjam.exe" (
	cd "tools\jam\src"
	cmd /q /c build.bat >NUL 2>&1
	cd "%PROJECT_BASE%deps\boost"
)

rem Build the boost libraries we need.

if "%BUILD_TYPE%" == "debug" (
	cmd /c "tools\jam\src\bin.ntx86\bjam.exe" --toolset=msvc-10.0 --with-date_time --with-thread variant=debug link=static runtime-link=shared threading=multi define=_SCL_SECURE_NO_WARNINGS=0
)

if "%BUILD_TYPE%" == "release" (
	cmd /c "tools\jam\src\bin.ntx86\bjam.exe" --toolset=msvc-10.0 --with-date_time --with-thread variant=release link=static runtime-link=shared threading=multi define=_SCL_SECURE_NO_WARNINGS=0
)

if "%BUILD_TYPE%" == "all" (
	cmd /c "tools\jam\src\bin.ntx86\bjam.exe" --toolset=msvc-10.0 --with-date_time --with-thread variant=debug,release link=static runtime-link=shared threading=multi define=_SCL_SECURE_NO_WARNINGS=0
)

cd "%PROJECT_BASE%"

goto :eof
rem --- End of BUILD_BOOST -----------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of BUILD_GTEST ---------------------------------------------------
rem --- Builds all googletest library used for unit testing.                 ---
:BUILD_GTEST

echo BUILDING: Google Test - http://code.google.com/p/googletest/

cd "%PROJECT_BASE%gtest\msvc"

rem Only build gtest if it hasn't been built already.
if "%BUILD_TYPE%" == "debug" (
	if exist "gtest-md\debug\gtest_main-md.lib" (
		echo Google Test library already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)
if "%BUILD_TYPE%" == "release" (
	if exist "gtest-md\release\gtest_main-md.lib" (
		echo Google Test library already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)

if "%BUILD_TYPE%" == "all" (
	if exist "gtest-md\debug\gtest_main-md.lib" (
		if exist "gtest-md\release\gtest_main-md.lib" (
			echo Google Test library already built ... skipping
			echo.
			cd "%PROJECT_BASE%"
			goto :eof
		)
	)
)

if exist "*.cache" del /S /Q "*.cache" >NUL

if "%BUILD_TYPE%" == "debug" (
	"%MSBUILD%" "gtest-md.sln" /t:build /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

if "%BUILD_TYPE%" == "release" (
	"%MSBUILD%" "gtest-md.sln" /t:build /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

if "%BUILD_TYPE%" == "all" (
	"%MSBUILD%" "gtest-md.sln" /t:build /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	if exist "*.cache" del /S /Q "*.cache" >NUL

	"%MSBUILD%" "gtest-md.sln" /t:build /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

cd "%PROJECT_BASE%"

goto :eof
rem --- End of BUILD_GTEST -----------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of BUILD_GLOG ----------------------------------------------------
rem --- Builds glog library used for logging.                                ---
:BUILD_GLOG

echo BUILDING: Google glog - http://code.google.com/p/google-glog/

cd "%PROJECT_BASE%glog"

rem Only build gtest if it hasn't been built already.
if "%BUILD_TYPE%" == "debug" (
	if exist "debug\libglog_static.lib" (
		echo Google glog library already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)
if "%BUILD_TYPE%" == "release" (
	if exist "release\libglog_static.lib" (
		echo Google glog library already built ... skipping
		echo.
		cd "%PROJECT_BASE%"
		goto :eof
	)
)

if "%BUILD_TYPE%" == "all" (
	if exist "debug\libglog_static.lib" (
		if exist "release\libglog_static.lib" (
			echo Google glog library already built ... skipping
			echo.
			cd "%PROJECT_BASE%"
			goto :eof
		)
	)
)

if exist "*.cache" del /S /Q "*.cache" >NUL

if "%BUILD_TYPE%" == "debug" (
	"%MSBUILD%" "google-glog.sln" /t:build /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

if "%BUILD_TYPE%" == "release" (
	"%MSBUILD%" "google-glog.sln" /t:build /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

if "%BUILD_TYPE%" == "all" (
	"%MSBUILD%" "google-glog.sln" /t:build /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	if exist "*.cache" del /S /Q "*.cache" >NUL

	"%MSBUILD%" "google-glog.sln" /t:build /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

cd "%PROJECT_BASE%"

goto :eof
rem --- End of BUILD_GTEST -----------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of BUILD_PROJECT -------------------------------------------------
rem --- Builds the actual project.                                           ---
:BUILD_PROJECT

cd "%PROJECT_BASE%msvc"

if exist "*.cache" del /S /Q "*.cache" >NUL
if exist "build-aux\*.xml" del /S /Q "build-aux\*.xml" >NUL

if "%BUILD_TYPE%" == "debug" (
	"%MSBUILD%" "libanh.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

if "%BUILD_TYPE%" == "release" (
	"%MSBUILD%" "libanh.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

if "%BUILD_TYPE%" == "all" (
	"%MSBUILD%" "libanh.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Debug,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1
	if exist "*.cache" del /S /Q "*.cache" >NUL

	"%MSBUILD%" "libanh.sln" /t:%REBUILD% /p:Platform=Win32,Configuration=Release,VCBuildAdditionalOptions="/useenv"
	if errorlevel 1 exit /b 1
	if exist "*.cache" del /S /Q "*.cache" >NUL
)

goto :eof
rem --- End of BUILD_PROJECT ---------------------------------------------------
rem ----------------------------------------------------------------------------


rem ----------------------------------------------------------------------------
rem --- Start of SLEEP ---------------------------------------------------------
rem --- Waits some seconds before returning.                                 ---
:SLEEP
ping -n %1 -w 1 127.0.0.1>NUL
goto :eof
rem --- End of SLEEP -----------------------------------------------------------
rem ----------------------------------------------------------------------------

ENDLOCAL