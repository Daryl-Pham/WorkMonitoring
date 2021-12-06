Param([parameter(mandatory=$true)][ValidateSet("x86", "x64")]$arch,
[parameter(mandatory=$true)][ValidateSet(".\Release", ".\x64\Release")]$releasepath)

$AuthToken="49b1d8ca98e04336a59c3b9940c8cc0bdb858acd4f3b47d29e7ff87e5b1cbc2c"
$ProjectSentry="biz-telework-windows-$arch"
$Organization="optimalbiz"

sentry-cli.exe --auth-token=$AuthToken  upload-dif -o $Organization -p $ProjectSentry $releasepath\