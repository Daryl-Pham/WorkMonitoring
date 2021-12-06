#pragma once

#include <string>
#include <boost/filesystem.hpp>

class ConfigUtility
{
public:
    /*
     * @return "C:\ProgramData\OPTiM\BizTelework" if succeeded. "" if failed.
     */
    static std::wstring GetDataDirectory();

    static boost::filesystem::path GetLogDirectory();

    /*
     * @return "http://<host>/" if succeeded. "" if failed.
     */
    static std::wstring GetUpdateUrl();

    /*
     * @return UpgradeCode like"{UUID-UUID-UUID-UUID}" if succeeded. "" if failed.
     */
    static std::wstring GetUpgradeCode();

    /*
     * @return Version code like "1.0.0.0" if succeeded. "" if failed.
     */
    static std::wstring GetVersion();

    /*
     * @return RPC Endpoint name.
     */
    static std::wstring GetBizTeleworkUpdaterRPCEndpoint();
};
