#pragma once
#include <string>
#include <vector>
#include <utility>

#include "boost/filesystem/path.hpp"

typedef std::vector<std::pair<std::wstring, std::wstring>> LogLines;

class Logging
{
public:
    Logging(const boost::filesystem::path&, const std::wstring&);

    BOOL WriteLog(LogLines content);

    BOOL ReadLog(std::istream &logStream, LogLines &jsonStrigify);
    BOOL ReadLogActiveApp(std::istream& logStream, LogLines* jsonStrigify) const;

    BOOL ReadLog(LogLines &jsonStrigify);

    BOOL ReadLog(LogLines* jsonStrigify, BOOL* existFile);

    BOOL ReadLogActiveApp(LogLines* jsonStrigify);

    static BOOL DeviceLogStringifyAsJsonBody(const LogLines& logLines, std::wstring* jsonStrigify);
    static BOOL ActiveAppLogStringifyAsJsonBody(const LogLines& logLines, std::wstring* jsonStrigify);

    BOOL DeleteLog();

    std::wstring GetFilePath() const;

private:
    boost::filesystem::wpath m_DataDirectory;
    std::wstring m_FileName;
};
