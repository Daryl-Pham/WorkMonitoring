# This script for developing BizTeleworkUpdater.exe.
# You need administrator permission to run this script.

Param([parameter(mandatory=$true)][ValidateSet("debug", "release")]$conf,
      [parameter(mandatory=$true)][ValidateSet("x86", "x64")]$arch)
$ErrorActionPreference = "stop"

$src = "$conf\BizTeleworkUpdater.exe"
$dst = "C:\Program Files (x86)\Optimal Biz Telework\BizTeleworkUpdater.exe"
if ($arch -eq "x64")
{
    $src = "x64/$src"
    $dst = "C:\Program Files\Optimal Biz Telework\BizTeleworkUpdater.exe"
}

if (!(Test-Path $src))
{
    Write-Output "Error: You need to build it."
    exit 1
}

if (!(Test-Path $dst))
{
    Write-Output "Error: You need to install BizTelework.msi. "
    exit 1
}

Stop-Service OptimalBizTeleworkUpdater
Copy-Item -Path $src -Destination $dst
Start-Service OptimalBizTeleworkUpdater
