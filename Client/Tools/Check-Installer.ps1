Param(
    [string]$msi)
$ErrorActionPreference = "stop"

$msiFullPath = Resolve-Path -Path $msi
echo "Checking $msiFullPath"

$targetDirectory = (Get-Location).Path + "\tmp_installer_check"
if (Test-Path $targetDirectory)
{
    Remove-Item $targetDirectory -Recurse
}

Start-Process -Wait -FilePath msiexec -ArgumentList "/a $msiFullPath /qb TARGETDIR=$targetDirectory"

$expanededDirectory = $targetDirectory + "\BizTelework\Optimal Biz Telework"
$fileList = @(
    "BizTelework.exe",
    "BizTeleworkUpdater.exe",
    "BizTeleworkHook.dll",
    "LanguageEn.dll",
    "LanguageJa.dll",
    "crashpad_handler.exe",
    "wadding.txt",
    "BizTelework.ico")

if (((dir $expanededDirectory).count) -ne (($fileList).count))
{
    echo "Error: There are some unexpected files."
    echo (dir $expanededDirectory).count
    echo ($fileList).count
    exit 1
}

foreach ($file in $fileList)
{
    Resolve-Path -Path "$expanededDirectory\$file"
}

# Testing for https://biztelework.atlassian.net/browse/BTC-770
# Remove this test if 1.3.0 disappears from the market. (https://biztelework.atlassian.net/browse/BTC-771)
$fileSize = (Get-ChildItem $msiFullPath).Length
$oldVersionFileSize = 5861376 # File size of https://gitlab.tokyo.optim.co.jp/bizwindows/optimal_biz_windows_telework/-/releases/release%252Fbiztw-1.3.0.0_ver2_sentry
if ($fileSize -lt $oldVersionFileSize)
{
    echo "The new installer must be larger than old installer."
    echo $fileSize
    echo $oldVersionFileSize
    exit 1
}
