# Overview

Daryl Work Monitoring for Windows.

# Development

## Development Environment
* Windows 10
* Visual Studio 2022 Community
  * Desktop development with C++
  * Windows 10 SDK (The latest version and 10.0.18362.0)
  * MFC
  * English Language Package (The vcpkg needs this)
* [.NET Framework 3.5 Service Pack 1](https://www.microsoft.com/download/details.aspx?id=22)
  * This is required for installing the WiX Toolset.
* [WiX Toolset v3.11.2](https://github.com/wixtoolset/wix3/releases/tag/wix3112rtm)
  * Click `wix311.exe` and execute it.

Guideline install vcpkg, boost, cpprestapi, sentry
(Make sure installed English Language Package for Visual Studio 2019)

## Setup vcpkg
0.  Before build project, please install vcpkg package, and install

1.  Get vcpkg package and install
*  Use power-shell with adminstrator permission:
*  PS> git clone https://github.com/Microsoft/vcpkg.git
*  PS> cd vcpkg
*  PS> git checkout <>
*  PS> .\bootstrap-vcpkg.bat
*  PS> .\vcpkg.exe integrate install

2.  Install boost, cpprestapi, sentry, gtest with vcpkg 
*  PS> .\vcpkg.exe install sentry-native:x86-windows-static sentry-native:x64-windows-static
*  PS> .\vcpkg.exe install boost:x86-windows-static boost:x64-windows-static
*  PS> .\vcpkg.exe install cpprestsdk:x86-windows-static cpprestsdk:x64-windows-static
*  PS> .\vcpkg.exe install gtest:x86-windows-static gtest:x64-windows-static

3. For those cases that have been installed and need to upgrade packages to the latest version
*  Before update, we need to backup vcpkg packages (I think copy all folder vcpkg).
*  Use power-shell with adminstrator permission:
*  PS> git checkout master
*  PS> git restore .
*  PS> git pull
*  PS> git checkout <> 
*  PS> .\bootstrap-vcpkg.bat
*  PS> .\vcpkg.exe update
*  PS> .\vcpkg.exe upgrade --no-dry-run
