# Summary

Updater is a module that updates the BizTelework application and itself automatically.
This document describes how the updater is executed and updates BizTelework.

## Setup Updater

The updater will be installed by BizTelework.msi installer.

The installer also creates these registry entries.

* `UpdateUrl`
  * `UpdateUrl` shows API for checking update.
* `ExecuteBizTelework`
  * `ExecuteBizTelework` shows whether Updater should execute BizTelework every logged-in session.

The updater should be installed as Windows service.
Because the updater need the aministrator permission for executing a new version installer and BizTelework.exe on all user's sessions.

## The first installing about updater

This diagrams draws only the updater and relating files.

```plantuml
@startuml
actor User
participant Windows
participant "BizTelework.exe" as BT
participant "BizTeleworkUpdater.exe" as Updater
participant "Installer"

activate Windows

User -> Windows: Execute Installer
create Installer
Windows -> Installer: Execute
activate Installer

Installer -> Installer: Copy files to the install directory.
Installer -> Windows: Register BizTelewokrUpdate.exe as Windows service.
Installer -> Windows: Start BizTelewokrUpdate.exe as Windows service.

create Updater
Windows -> Updater: Start as Windows service with administrator permission.
activate Updater

ref over Updater
    Execute BizTelework from Updater
end ref

@enduml
```

## Execute BizTelework from Updater

```plantuml
participant Windows
participant "BizTelework.exe" as BT
participant "BizTeleworkUpdater.exe" as Updater

activate Windows
activate Updater

Updater -> Windows: Get value of "ExecuteBizTelework" of Windows Registry
Windows -> Updater: Return value of "ExecuteBizTelework"

note over Updater
    Execute BizTelework process every logged on session after installed once.
end note

alt "ExecuteBizTelework" equals 1
    loop for each User logged-on sessions
        alt There is not BizTelework.exe process on the logged-on session
            create BT
            Updater -> BT: Execute on a user's logged-on session.
            activate BT
        end alt
    end loop

    note over Updater
        By settign 0 to "ExecuteBizTelework", Preventing that Updater execute BizTelework until next installation.
    end note
    Updater -> Windows: Set 0 to "ExecuteBizTelework"
    Windows -> Updater: Done

end alt
```

## Updater sequence

```plantuml
@startuml
actor User
participant Windows
participant "BizTelework.exe" as BT
participant "BizTeleworkUpdater.exe" as Updater
participant Server

User -> Windows: Boot Windows OS.
activate Windows

create Updater
Windows -> Updater: Start as Windows service with administrator permission.
activate Updater

User <- Windows: Show logged-on screen.
User -> Windows: Input authentication information for logged-on.
User <- Windows: Show User's desktop window.

create BT
Windows -> BT: After user is logged-on, execute on User's logon session. 
activate BT

note over Updater
    BizTeleworkUpdater.exe is the Updater
    which is described on this document.
end note

loop
    ref over Updater
        Sleep loop procedure
    end ref

    Updater -> Windows: Get value of UpdateUrl registry item.
    Updater <- Windows: Return value of UpdateUrl registry item.

    Updater -> BT: Request: https://download.biztw.optim.co.jp/windows/version.txt
    note over BT
        Because BizTeleworkUpdater uses proxy server of user's setting, BizTeleworkUpdater depends on BizTelework to send a reuqest to the server.
    end note
    BT -> Server: Request: https://download.biztw.optim.co.jp/windows/version.txt
    activate Server
    BT <- Server: Response: Latest version. This body of response is only version.
    BT -> Updater: Response: Latest version. This body of response is only version.

    note over Updater
        Content of version.txt is version of https://download.biztw.optim.co.jp/BizTelework.msi
    end note
    note over Updater
        Example of the Response

        HTTP/1.1 200 OK\r\n
        Content-Type: text/plain\r\n
        \r\n
        1.0.2.0\r\n
        \r\n
    end note
    deactivate Server

    alt Network error or HTTP status code != 200
        Updater -> Updater: Continue loop. (Goto the top of this loop)
    end alt

    ref over Updater
        Updating install
    end ref
end loop
@enduml
```

## Sleep loop sequence

```plantuml
@startuml
participant "BizTeleworkUpdater.exe" as Updater

activate Updater

note over Updater
    Sleep duration is decided in range of uniform distribution random between 1 hour and 168 hours.
    Random duration is for avoiding unintended DDoS attack to Server.
end note

alt `C:\ProgramData\OPTiM\BizTelework\update_datetime.txt` does not exist

    Updater -> Updater: Write the current UNIX time plus UniformDistributionRandom(1, 168 hours) to `C:\ProgramData\OPTiM\BizTelework\update_datetime.txt`.
end alt

loop Current UNIX time < UNIX time of `C:\ProgramData\OPTiM\BizTelework\update_datetime.txt`
    alt `C:\ProgramData\OPTiM\BizTelework\update_datetime.txt` does not exist
        Updater -> Updater: Write the current UNIX time plus UniformDistributionRandom(1, 168 hours) to `C:\ProgramData\OPTiM\BizTelework\update_datetime.txt`.
    end alt

    alt (Current UNIX time + 168) > `C:\ProgramData\OPTiM\BizTelework\update_datetime.txt`
        note over Updater
            This condition for avoiding to sleep for over 168 hours.
            Ex. When the user sets far past time as current time manually.
        end note
        Updater -> Updater: Re-write the current UNIX time plus UniformDistributionRandom(1, 168 hours) to `C:\ProgramData\OPTiM\BizTelework\update_datetime.txt`.
    end alt

    Updater -> Updater: Sleep for 10 secondes.
end loop
@enduml
```

## Updating install sequence

```plantuml
@startuml
participant Windows
participant "BizTelework.exe" as BT
participant "BizTeleworkUpdater.exe" as Updater
participant Server
participant "New Version Installer" as Installer


activate Windows
activate BT
activate Updater

Updater -> BT: Request BizTelework.msi
BT -> Server: Request BizTelework.msi
activate Server
BT <- Server: Response BizTelework.msi
Updater <- BT: Response BizTelework.msi
deactivate Server

Updater -> Updater: Save BizTelework.msi on `C:\ProgramData\OPTiM\BizTelework` directory whether BizTelework.msi already exists.

alt Failed to save BizTelework.msi on `C:\ProgramData\OPTiM\BizTelework` directory
    Updater -> Updater: Continue loop. (Goto the top of this loop)
end

alt Common Name(CN) of signing of BizTeleworkUpdater.exe != CN of signing of BizTelework.msi
    Updater -> Updater: Remove `C:\ProgramData\OPTiM\BizTelework\BizTelework.msi`
    Updater -> Updater: Continue loop. (Goto the top of this loop)
end alt

alt Upgrade code of installed application != Upgrade code of BizTelework.msi
    Updater -> Updater: Remove `C:\ProgramData\OPTiM\BizTelework\BizTelework.msi`
    Updater -> Updater: Continue loop. (Goto the top of this loop)
end alt

alt Version code of installed application == Version code of BizTelwork.msi
    Updater -> Updater: Remove `C:\ProgramData\OPTiM\BizTelework\BizTelework.msi`
    Updater -> Updater: Continue loop. (Goto the top of this loop)
end alt

create Installer

Updater -> Installer: Execute
break
    Updater -> Updater: Exit from loop. And wait until stopped.
end break

activate Installer
Installer -> Windows: Stop BizTelewokrUpdate.exe Windows service.
Windows -> Updater: Stop service
destroy Updater
Installer -> Windows: Unresgister BizTelewokrUpdate.exe Windows service.


loop for each BizTelework.exe processes
    Installer -> BT: Kill a BizTelework.exe process force.
    destroy BT
end loop

Installer -> Installer: Replace all files of BitTelwork application.

Installer -> Windows: Register BizTelewokrUpdate.exe as Windows service.
Installer -> Windows: Start BizTelewokrUpdate.exe as Windows service.

Windows -> Updater: Start as Windows service with administrator permission.
activate Updater

ref over Updater
    Execute BizTelework from Updater
end ref

Installer -> Installer: Finish installing
destroy Installer
@enduml
```
