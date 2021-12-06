# Copy from https://gitlab.tokyo.optim.co.jp/bizwindows/optimal_biz_windows_agent/-/blob/master/Build/PostBuild.ps1
# Save as Shift-JIS to avoid post-build error.

Param(
    [string]$file,
    [switch]$upx)
$ErrorActionPreference = "stop"
function ExecuteWithRetry($cmdline)
{
    Set-Variable -name RETRY -value 10 -option constant
    Write-Output $cmdline
    # 成功した回か、最後の回の実行結果を採用する。
    foreach($i in (1..$RETRY))
    {
        Write-Output "Trial #$i"
        iex $cmdline
        if ($lastexitcode -eq 0)
        {
            break
        }
        sleep -s 1
    }
    if ($lastexitcode -ne 0)
    {
        $msg = "$((-split $cmdline)[0])がステータスコード $lastexitcode で終了しました"
        throw $msg
    }
}
function getProductName
{
    $target = $env:PRODUCT_NAME
    if ($target)
    {
        echo $target
    }
    else
    {
        echo "Optimal Biz Telework"
    }
}
try
{
    if (!(Test-Path $file))
    {
        exit 1
    }
    Write-Debug $file
    $pfxFile = $env:BIZ_PFX_FILE
    if ($pfxFile -ne $null) { Write-Debug $pfxFile }
    if ([bool]$pfxFile -and (Test-Path $pfxFile))
    {
        $productName = getProductName
        Write-Debug $productName
        $targetExt = (ls $file).Extension.ToUpperInvariant()
        Write-Debug $targetExt
        if (".EXE", ".DLL", ".MSI" -contains $targetExt)
        {
            if ($upx)
            {
                upx --best --lzma $file
            }
            $password = $env:CERT_PASSWORD
            if ($password)
            {
                $passwordArg = "/p ""$password"""
            }
            ExecuteWithRetry -cmdline "signtool sign /f $pfxFile $passwordArg /d ""$productName"" /fd SHA256 /td SHA256 /tr $env:BIZ_TIMESTAMP_SERVER_TR /ph $file"
        }
    }
    else
    {
        echo "The pfxFile is not set."
        exit 1
    }
    exit 0
}
catch
{
    echo $Error
    exit 1
}
