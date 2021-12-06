Param(
    [string]$version)
$ErrorActionPreference = "stop"

$versionPattern = "(\d+\.\d+\.\d+\.\d+)"
$quotedVersionPattern = "`"$versionPattern`""
$commaSeparatedVersionPattern = ($versionPattern -replace "\.", ",")

function ReplaceString
{
    param(
        [string]$file,
        [string]$encoding,
        [string]$old,
        [string]$new)
    ((Get-Content -Path $file -Raw) -replace $old, $new) | Set-Content -Encoding $encoding -Path $file -NoNewline
}


function Main
{
    param ([ValidateScript({ $_ -match "^$versionPattern$" })]$version)

    $quotedVersion = "`"$version`""
    $commaSeparatedVersion = ($version -replace "\.", ",")

    ReplaceString 'BizTelework\BizTelework.rc'     "UNICODE" $quotedVersionPattern $quotedVersion
    ReplaceString 'BizTelework\BizTelework.rc'     "UNICODE" "FILEVERSION $commaSeparatedVersionPattern" "FILEVERSION $commaSeparatedVersion"
    ReplaceString 'BizTelework\BizTelework.rc'     "UNICODE" "PRODUCTVERSION $commaSeparatedVersionPattern" "PRODUCTVERSION $commaSeparatedVersion"
    ReplaceString 'BizTeleworkUpdater\BizTeleworkUpdater.rc'     "UNICODE" $quotedVersionPattern $quotedVersion
    ReplaceString 'BizTeleworkUpdater\BizTeleworkUpdater.rc'     "UNICODE" "FILEVERSION $commaSeparatedVersionPattern" "FILEVERSION $commaSeparatedVersion"
    ReplaceString 'BizTeleworkUpdater\BizTeleworkUpdater.rc'     "UNICODE" "PRODUCTVERSION $commaSeparatedVersionPattern" "PRODUCTVERSION $commaSeparatedVersion"
    ReplaceString 'BizTeleworkMsi\common.wxi' "UTF8"    " Version = $quotedVersionPattern " " Version = $quotedVersion "
    Out-File -InputObject "$version" -Encoding ASCII -NoNewLine -FilePath version.txt
}

Main $version
