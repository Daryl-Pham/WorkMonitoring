#pragma once

#include <boost/filesystem.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <string>

// Definition log type
extern const std::wstring LogTypeKeyboard;
extern const std::wstring LogTypeMouse;

class UserInputLog
{
public:
    UserInputLog(const std::wstring& fileName, const std::wstring& inputType);
    UserInputLog(const std::wstring& basePath, const std::wstring& fileName, const std::wstring& inputType);
    ~UserInputLog();
    
    /**
     * Write a log if the time condition is passed.
     * @return true if no error happend. false if error has occurred.
     */
    BOOL WriteLogOnTimeCondition(const boost::chrono::system_clock::time_point &current);

    std::wstring GetFilePath() const { return m_FilePath.wstring(); }

protected:
    BOOL CreateFileAndWriteLog(const boost::chrono::system_clock::time_point &current);
    BOOL WriteLogIfNeccessary(const boost::chrono::system_clock::time_point &current);

private:
    bool DeleteWrongLastLine(const size_t size);
    const boost::filesystem::path m_FilePath;
    const std::wstring m_InputType;
};

