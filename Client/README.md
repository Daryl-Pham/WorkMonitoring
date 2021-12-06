# Overview

Optimal Biz Telework for Windows.

# Development

## Development Environment
* Windows 10
* Visual Studio 2019 Professional
  * Desktop development with C++
  * Windows 10 SDK (The latest version and 10.0.18362.0)
  * MFC
  * English Language Package (The vcpkg needs this)
* [.NET Framework 3.5 Service Pack 1](https://www.microsoft.com/download/details.aspx?id=22)
  * This is required for installing the WiX Toolset.
* [WiX Toolset v3.11.2](https://github.com/wixtoolset/wix3/releases/tag/wix3112rtm)
  * Click `wix311.exe` and execute it.
* [Wix Toolset Visual Studio 2019 Extension](https://marketplace.visualstudio.com/items?itemName=WixToolset.WixToolsetVisualStudio2019Extension)
  * Click Download button and execute `Votive2019.vsix`.
* vcpkg
* [Gitlab](https://gitlab.tokyo.optim.co.jp/bizios/biztelework/optimal_biz_ios_telework) for source code version controll, code view, and CI build.
* [Jira](https://biztelework.atlassian.net/jira/software/projects/BTC/boards/3) for backlogs.
* [Confluence](https://biztelework.atlassian.net/wiki/spaces/BT/overview) for documents about BizTelework.
* [Slack](https://optim-biz.slack.com/) for communicate with BizTelwork team on those channels. You need an invitation to join this slack.
  * [#team_dev_b_client](https://optim-biz.slack.com/archives/C0129U7GD7U) are joined by OPTiM members who are relating Biz Telework client.
  * [#team_dev_b_with_vti](https://optim-biz.slack.com/archives/C0130Q2V8HM) are joined by OPTiM members and VTI members who are relating Biz Telework client. For talking about Biz Telework client in English or Japanse. @an.trantuan can help your communication in English or Japanse.
  * [#team_telework](https://optim-biz.slack.com/archives/C01CMHD6WR0) are joined by all BizTelerok memebers.
  * [#biz3](https://optim-biz.slack.com/archives/C0PDTQZ16) are joined by all Optimal Biz members. You can request information about Optimal Biz to them.

Guideline install vcpkg, boost, cpprestapi, sentry
(Make sure installed English Language Package for Visual Studio 2019)

## Setup vcpkg
0.  Before build project, please install vcpkg package, and install

1.  Get vcpkg package and install
*  Use power-shell with adminstrator permission:
*  PS> git clone https://github.com/Microsoft/vcpkg.git
*  PS> cd vcpkg
*  PS> git checkout d0268cf16649ffb8b847b719a9cab224623291f
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
*  PS> git checkout d0268cf16649ffb8b847b719a9cab224623291f
*  PS> .\bootstrap-vcpkg.bat
*  PS> .\vcpkg.exe update
*  PS> .\vcpkg.exe upgrade --no-dry-run

## Execute BizTelework.exe from Visual Studio

You need to install `BizTeleworkMsi\bin\Debug\ja-JP\BizTelework.msi` before execute from Visual Studio.
Because BizTelework app depends on some registry that registered on installing.

After installing BizTelework.msi, you can run from Visual Studio.

1. Open BizTelework.sln in Visual Studio 2019.
1. Select `Debug` mode and `x86` architecture.
1. Click `Build` menu > `Rebuild Solution`.
1. Install `BizTeleworkMsi\bin\Debug\ja-JP\BizTelework.msi`.
1. Kill BizTelework.exe process in Task Manager. For preventing duplicated processes.
1. Right click on BizTelework project -> `Set as StartUp Project`.
1. Press `F5` key.

You can see BizTelework icon on the notification area.

## Execute BizTeleworkUpdater.exe

BizTeleworkUpdater.exe runs as a Windows service that updates BizTelework and itself.
Therefore it is a little complicated to run BizTeleworkUpdater.exe that you build on your development environment.

You need to build BizTeleworkUpdate.exe and install `BizTeleworkMsi\bin\Debug\ja-JP\BizTelework.msi` before following steps.

1. Open Task Manager.
1. Open `Services` tab
1. Do right click `OptimalBizTeleworkUpdater` item.
1. Click `Stop`
1. Copy `Debug\BizTeleworkUpdater.exe` to `C:\Program Files (x86)\Optimal Biz Telework\BizTeleworkUpdater.exe`. You need to overwrite.
1. Start `OptimalBizTeleworkUpdater` service in Task Manager.

# Development cycle about a backlog

1. Set your backlog status as `IN PROGRESS`.
1. Create a topic branch as `btc/???`. The `???` should be placed the number of backlog. (ex. If you get backlog BTC-123, you need create topic branch as `btc/123`.
1. Commit your changes to the topic branch.
    * You should add `(BTC-???)` to the ending of commit title.
1. Push the topic branch to this project.
1. Create the merge request(MR) of the topic branch.
    * You should add `(BTC-???)` to the ending of MR title. By this, you can link Gitlab MR and Jira backlog.
1. Resolve the reviewdog comments and Sonarqube comments.
1. Set your backlog status as `REVIEW`.
1. Request a review about the MR on `#team_dev_b_with_vti` channel to 2 reviewers.
1. All reviewers should click `Approve` button if your review is approved.
1. Click `Merge` button if 2 reviewers approved. Anyone can click `Merge` button.

Note: If you need stop review, add `[WIP]` to the beginning of MR title.

# CI Environment

This project uses `incredibuild1` and `tanpop` that are Gitlab CI Runner.

You can see runner status on [CI/CI settins page](https://gitlab.tokyo.optim.co.jp/bizwindows/optimal_biz_windows_telework/-/settings/ci_cd).

You can access `incredibuild1` PC throw Microsoft Remote Desktop app.
Please note that you need connect OPTiM internal net(VPN) and your user accout.

The following command is restart gitlab runner process.

```
# You can execute commands on Powershell with admin permission.
PS> cd E:\GitLab-Runner
PS> .\gitlab-runner-windows-amd64.exe stop
PS> .\gitlab-runner-windows-amd64.exe start
```

About other command, see https://docs.gitlab.com/runner/commands/ .

# CI variables

CI variables can check and set on [Variables of CI/CI settings page](https://gitlab.tokyo.optim.co.jp/bizwindows/optimal_biz_windows_telework/-/settings/ci_cd).

## About SIGNING_CERTIFICATE_PEM

`SIGNING_CERTIFICATE_PEM` contains a signing certificate which is encoded in PEM format.

You can generate `SIGNING_CERTIFICATE_PEM` value with this command.
Set `C:\Path\to\file.pfx` to the PFX file path.

Note: You can get the `file.pfx` from `C:\PrivateKey\optim_authcode_nopassword_2020.pfx` of `incredibuild1`.

```
PS> certutil -encode C:\Path\to\file.pfx .\out.pem
```

Copy and paset content of `out.pem` to variable of `SIGNING_CERTIFICATE_PEM`.
And set type as `File`.

## About BIZ_TIMESTAMP_SERVER_TR

`BIZ_TIMESTAMP_SERVER_TR` contains URL of timestamp server. (ex. `http://timestamp.digicert.com`

# Error on docker-windows CI runner 

If you got this error on docker-windows CI runner,

```
Preparing the "docker-windows" executor
Using Docker executor with image gitlab-registry.tokyo.optim.co.jp/bizwindows/optimal_biz_windows_telework:1ea25f7cad20755595c4d6ffea6540addcd2b5fc ...
Authenticating with credentials from C:\WINDOWS\system32\config\systemprofile\.docker\config.json
Pulling docker image gitlab-registry.tokyo.optim.co.jp/bizwindows/optimal_biz_windows_telework:1ea25f7cad20755595c4d6ffea6540addcd2b5fc ...
ERROR: Preparation failed: Error response from daemon: Get https://gitlab-registry.tokyo.optim.co.jp/v2/bizwindows/optimal_biz_windows_telework/manifests/1ea25f7cad20755595c4d6ffea6540addcd2b5fc: unauthorized: HTTP Basic: Access denied (docker.go:142:0s)
```

you neet set your GitLab API token on `C:\WINDOWS\system32\config\systemprofile\.docker\config.json` like this.
The token should contains `read_registry` Scopes.

```
{
    "auths": {
        "gitlab-registry.tokyo.optim.co.jp": {
            "auth": "<SET YOUR API TOKEN>"
        }
    },
    "HttpHeaders": {
        "User-Agent": "Docker-Client/19.03.13 (windows)"
    }
}
```

# You need add config when you create a new Visual Studio project

When you create a new Visual Studio project that depends on vcpkg, you need add following `VcpkgTriplet` elements to the `*.vcxproj` file.

```
  <PropertyGroup Label="Globals">
    ...
    <VcpkgTriplet Condition="'$(Platform)'=='Win32'">x86-windows-static</VcpkgTriplet>
    <VcpkgTriplet Condition="'$(Platform)'=='x64'">x64-windows-static</VcpkgTriplet>
  </PropertyGroup>
```

Reference: https://devblogs.microsoft.com/cppblog/vcpkg-updates-static-linking-is-now-available/

# File signing

Files of *.msi, *.exe and *.dll are singned. The following list shows when files will be signed.

* BizTelework.msi will be singed on the post-build event of BizTeleworkMsi.wixproj.
* *.dll and BizTelework.exe will be singed on the post-build event of BizTelework.vcxproj.

Please note that files will be signed only if `EnableSiging` property is `true`.

# How to release

1. You need create `BizTeleworkMsi\customize\<Customize>.wxi` file if the file does not exist.
2. Create the tag for release( `release/w.x.y.z` or `release/<Customize>-w.x.y.z` ).
(`release/w.x.y.z` or `release/development-w.x.y.z` or don't set commit tag to build binary development.
Or `release/<Customize>-w.x.y.z` to build binary for testing, staging, production).
3. Push the tag to origin.
4. Check artifacts of Gitlab CI.

# How to update License.rtf

There is a problem that WiX can not preview a RTF file which genereate by Microsoft Word.
If you want preview RTF file on Wix, open and re-save the RTF file by WordPad.
See https://stackoverflow.com/a/6380750

# How to change versions

The following command is to change versions of BizTelework.exe and *.msi file to "1.0.1.0".
You need to run the command on powershell.

```
PS> .\Tools\Update-Versions.ps1 1.0.1.0
```

You may need to set ExecutionPolicy as RemoteSigned if the above command is failed.

```
# You need to run admin mode powershell.
PS> Set-ExecutionPolicy RemoteSigned
```

# How to upload debug symbols to sentry server

You need to create commit tag for release same as: ( `release/w.x.y.z_sentry` or `release/<Customize>-w.x.y.z_sentry` ).
