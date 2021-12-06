#pragma once

#include <memory>
#include <string>

class AppVersion
{
private:
    AppVersion(unsigned int major, unsigned int minor, unsigned int patch, unsigned int build);

public:
    virtual ~AppVersion() = default;

    /* Parse version string.
     * @return AppVersion object if parsing is succeeded. nullptr if parsing is failed.
     */
    static std::unique_ptr<AppVersion> Parse(const std::wstring &versionString);

    bool IsNewerThan(const AppVersion& other) const;
    bool IsEqual(const AppVersion& other) const;

    const unsigned int major;
    const unsigned int minor;
    const unsigned int patch;
    const unsigned int build;
};
