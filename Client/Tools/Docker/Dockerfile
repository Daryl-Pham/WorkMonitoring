# escape=`

# Use the latest Windows Server Core image with .NET Framework 3.5.
FROM mcr.microsoft.com/dotnet/framework/sdk:3.5-20191008-windowsservercore-ltsc2019

# Restore the default Windows shell for correct batch processing.
SHELL ["cmd", "/S", "/C"]

# .\vs_BuildTools.exe export --config "C:\BuildTools\vsconfig" で得た vsconfig に記載されているオプションを --add で付加。
# Reference https://docs.microsoft.com/ja-jp/visualstudio/install/command-line-parameter-examples?view=vs-2019
# Install Build Tools
RUN mkdir C:TEMP && `
    powershell -Command "Invoke-WebRequest -Uri https://aka.ms/vs/16/release/vs_buildtools.exe -outfile C:\TEMP\vs_buildtools.exe"
RUN C:\TEMP\vs_buildtools.exe --quiet --wait --norestart --nocache `
    --installPath C:\BuildTools `
    --add Microsoft.VisualStudio.Component.Roslyn.Compiler `
    --add Microsoft.Component.MSBuild `
    --add Microsoft.VisualStudio.Component.CoreBuildTools `
    --add Microsoft.VisualStudio.Workload.MSBuildTools `
    --add Microsoft.VisualStudio.Component.Windows10SDK `
    --add Microsoft.VisualStudio.Component.VC.CoreBuildTools `
    --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    --add Microsoft.VisualStudio.Component.VC.Redist.14.Latest `
    --add Microsoft.VisualStudio.Component.Windows10SDK.18362 `
    --add Microsoft.VisualStudio.Component.VC.ATL `
    --add Microsoft.VisualStudio.Component.VC.ATLMFC `
    --add Microsoft.VisualStudio.Component.VC.ASAN `
    --add Microsoft.VisualStudio.Workload.VCTools `
 || IF "%ERRORLEVEL%"=="3010" EXIT 0

# Install Git
ADD https://github.com/git-for-windows/git/releases/download/v2.29.2.windows.2/PortableGit-2.29.2.2-64-bit.7z.exe C:\TEMP\PortableGit.7z.exe
RUN C:\TEMP\PortableGit.7z.exe -o"C:\Git" -y && `
    setx /M PATH "%PATH%;C:\Git\bin"

# Install vcpkg
RUN git clone https://github.com/microsoft/vcpkg C:\vcpkg && `
    git -C C:\vcpkg switch -c d0268cf16649ffb8b847b719a9cab224623291f && `
    C:\vcpkg\bootstrap-vcpkg.bat && `
    C:\vcpkg\vcpkg.exe integrate install && `
    setx /M PATH "%PATH%;C:\vcpkg"

# Install vcpkg packages
RUN vcpkg install boost-asio:x86-windows-static `
                  boost-asio:x64-windows-static `
                  boost-chrono:x86-windows-static `
                  boost-chrono:x64-windows-static `
                  boost-compute:x86-windows-static `
                  boost-compute:x64-windows-static `
                  boost-dll:x86-windows-static `
                  boost-dll:x64-windows-static `
                  boost-filesystem:x86-windows-static `
                  boost-filesystem:x64-windows-static `
                  boost-format:x86-windows-static `
                  boost-format:x64-windows-static `
                  boost-serialization:x86-windows-static `
                  boost-serialization:x64-windows-static `
                  boost-timer:x86-windows-static `
                  boost-timer:x64-windows-static `
                  boost-uuid:x86-windows-static `
                  boost-uuid:x64-windows-static `
                  cpprestsdk:x86-windows-static `
                  cpprestsdk:x64-windows-static `
                  gtest:x86-windows-static `
                  gtest:x64-windows-static `
                  sentry-native:x86-windows-static `
                  sentry-native:x64-windows-static && `
    rmdir /s /q C:\vcpkg\downloads && `
    rmdir /s /q C:\vcpkg\packages

# Install WiX Toolset
ADD https://github.com/wixtoolset/wix3/releases/download/wix3112rtm/wix311-binaries.zip C:\TEMP\wix311-binaries.zip
RUN powershell -Command "Expand-Archive -Path C:\TEMP\wix311-binaries.zip -Destination C:\wix311" && `
    setx /M PATH "%PATH%;C:\wix311"

# Install sentry-cli
ADD https://github.com/getsentry/sentry-cli/releases/download/1.58.0/sentry-cli-Windows-x86_64.exe C:\sentry-cli\sentry-cli.exe
RUN setx /M PATH "%PATH%;C:\sentry-cli"

# Install Java
## Make sure of the URL of https://www.java.com/ja/download/manual.jsp
## Reference https://alexandrnikitin.github.io/blog/running-java-inside-windows-container-on-windows-server/
ADD https://javadl.oracle.com/webapps/download/AutoDL?BundleId=243737_61ae65e088624f5aaa0b1d2d801acb16 C:\TEMP\jre-windows-x64.exe
RUN powershell start-process -filepath C:\TEMP\jre-windows-x64.exe  -passthru -wait -argumentlist "/s,INSTALLDIR=c:\jre" && `
    setx /M JAVA_HOME "C:\jre" && `
    setx /M PATH "%PATH%;C:\jre\bin"

# Install sonar-scanner
## You can get the build-wrapper in OPTiM internal network.
ADD https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/sonar-scanner-cli-4.5.0.2216-windows.zip C:\TEMP\sonar-scanner-cli.zip
RUN powershell -Command "Expand-Archive -Path C:\TEMP\sonar-scanner-cli.zip -Destination C:\sonar-scanner-cli" && `
    setx /M PATH "%PATH%;C:\sonar-scanner-cli\sonar-scanner-4.5.0.2216-windows\bin"

# Install build-wrapper
## You can get the build-wrapper in OPTiM internal network.
ADD https://sonarqube.bizweb.optim-test.com/static/cpp/build-wrapper-win-x86.zip C:\TEMP\build-wrapper-win-x86.zip
RUN powershell -Command "Expand-Archive -Path C:\TEMP\build-wrapper-win-x86.zip -Destination C:\build-wrapper" && `
    setx /M PATH "%PATH%;C:\build-wrapper\build-wrapper-win-x86"

# Define the entry point for the docker container.
# This entry point starts the developer command prompt and launches the PowerShell shell.
ENTRYPOINT ["C:\\BuildTools\\Common7\\Tools\\VsDevCmd.bat", "&&", "powershell.exe", "-NoLogo", "-ExecutionPolicy", "Bypass"]