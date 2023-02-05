1. Install develop environment
# Get vcpkg package and install
*  Use power-shell with adminstrator permission:
*  PS> git clone https://github.com/Microsoft/vcpkg.git
*  PS> cd vcpkg
*  PS> git checkout d0268cf16649ffb8b847b719a9cab224623291f
*  PS> .\bootstrap-vcpkg.bat
*  PS> .\vcpkg.exe integrate install

# Install boost, cpprestsdk, sentry, gtest with vcpkg 
*  PS> .\vcpkg.exe install sentry-native:x86-windows-static sentry-native:x64-windows-static
*  PS> .\vcpkg.exe install boost:x86-windows-static boost:x64-windows-static
*  PS> .\vcpkg.exe install cpprestsdk:x86-windows-static cpprestsdk:x64-windows-static
*  PS> .\vcpkg.exe install gtest:x86-windows-static gtest:x64-windows-static