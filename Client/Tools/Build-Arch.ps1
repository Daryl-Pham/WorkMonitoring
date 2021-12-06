Param([parameter(mandatory=$true)][ValidateSet("x86", "x64")]$arch)
$ErrorActionPreference = "stop"

Set-PSDebug -Trace 1  # To show lines of executing.

# build-wrapper-win-x86-64.exe does not impact the msbuild. See https://docs.sonarqube.org/latest/analysis/languages/cfamily/
$MsbuildCommand="build-wrapper-win-x86-64.exe --out-dir artifact\x86\build_wrapper msbuild"
$MsiBinPath="BizTeleworkMsi\bin"
$ReleasePath=".\Release"
if ($arch -eq "x64")
{
    $MsbuildCommand="msbuild"
    $MsiBinPath="BizTeleworkMsi\bin\x64"
    $ReleasePath=".\x64\Release"
}
$ScriptUploadPath=".\Tools\Upload-DebugInfor.ps1"

# BIZ_PFX_FILE is used by .\Build\sing.ps1.
$env:BIZ_PFX_FILE="$CI_PROJECT_DIR\optim_authcode_nopassword.pfx"
certutil -decode $SIGNING_CERTIFICATE_PEM $env:BIZ_PFX_FILE

mkdir -p artifact\$arch

chcp 65001
$Customize="testing"
if ("$CI_COMMIT_TAG" -match "^release/(.*)-.*$") { $Customize=$Matches[1]; }
Write-Output (Resolve-Path -Path BizTeleworkMsi\customize\$Customize.wxi).Path
Invoke-Expression "$MsbuildCommand .\BizTelework.sln /m /t:rebuild /p:Configuration=Release /p:Platform=$arch /p:Customize=$Customize /p:EnableSigning=true /p:WixInstallPath=$WIX_INSTALL_PATH /p:WixTargetsPath=$WIX_INSTALL_PATH\wix.targets; $LASTEXITCODE"

$IsUploadDebugFile="false"
if ("$CI_COMMIT_TAG" -match "^release/(.*)_sentry$") { $IsUploadDebugFile="true"; }
if($IsUploadDebugFile -eq "true" ) { Invoke-Expression "$ScriptUploadPath $arch $ReleasePath"; }

# Make artifact
Move-Item -Path $MsiBinPath\Release\ja-JP\BizTelework.msi -Destination .\artifact\$arch
Move-Item -Path $ReleasePath\MsiForTesting.msi -Destination .\artifact\$arch
Move-Item -Path $ReleasePath\[LB]*.pdb -Destination .\artifact\$arch
Move-Item -Path $ReleasePath\UnitTest*.exe -Destination .\artifact\$arch
Move-Item -Path $ReleasePath\*.dll -Destination .\artifact\$arch
(Get-FileHash -Algorithm MD5 .\artifact\$arch\BizTelework.msi).Hash > .\artifact\$arch\BizTelework.msi.md5.txt
Copy-Item version.txt .\artifact\$arch
