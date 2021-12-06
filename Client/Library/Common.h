#pragma once
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/optional.hpp>
#include <boost/operators.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/fusion/iterator/equal_to.hpp>
#include <boost/fusion/include/equal_to.hpp>
#include <boost/fusion/tuple.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/chrono/duration.hpp>

#include <wincrypt.h>

// Time interval
static const DWORD TimerIntervalCheckAuthen = 1800;
static const DWORD TimerIntervalCheckLog = 60;

// Defination log file name
static const std::wstring KeyboardLogFileName = L"keyboard_logs.csv";
static const std::wstring MouseLogFileName = L"mouse_logs.csv";
static const std::wstring ActiveAppLogFileName = L"application_info.ltsv";

// Defination lock file name
static const std::wstring LockFileName = L"locked_file.lock";

// Defination file contains last time have activity log
static const std::wstring LastLogTimeFileName = L"last_get_log_time.txt";

// Infor agent infor optimal biz telework in registry
static const CString TeleworkAgentInfor32Reg = _T("SOFTWARE\\OPTiM\\OptimalBizTelework\\AgentInfo");
static const CString TeleworkAgentInfor64Reg = _T("SOFTWARE\\WOW6432Node\\OPTiM\\OptimalBizTelework\\AgentInfo");

// Infor optimal biz telework in registry
static const CString Telework32Reg = _T("SOFTWARE\\OPTiM\\OptimalBizTelework");
static const CString Telework64Reg = _T("SOFTWARE\\WOW6432Node\\OPTiM\\OptimalBizTelework");

// Optional entropy
static const DWORD LenEntropyData = 16;
static const BYTE EntropyData[LenEntropyData] = { 0x93,0xFE,0xF9,0xA8,0x3B,0x5D,0x48,0xA0,0xA0,0xE5,0x45,0x47,0x32,0xA5,0x59,0xFE };
static const DATA_BLOB OptionalEntropy = { LenEntropyData, const_cast<BYTE*>(&EntropyData[0]) };

class AgentCredential
{
public:
    boost::optional<std::wstring> company_guid;
    boost::optional<std::wstring> agent_guid;

private:
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive& ar, unsigned)
    {
        ar & BOOST_SERIALIZATION_NVP(company_guid);
        ar & BOOST_SERIALIZATION_NVP(agent_guid);
    }
};

struct BizGuid
    : boost::totally_ordered<BizGuid>
{
    boost::array<BYTE, 32> data;
};
static_assert(sizeof(BizGuid) == 32, "sizeof(BizGuid) should be 0.");

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const BizGuid& guid)
{
    std::array<char, 64> tmp;
    boost::algorithm::hex(guid.data, tmp.begin());
    // widenを通すため、ostreambuf_iteratorではなくostream_iteratorを使用している。
    boost::algorithm::to_lower_copy(std::ostream_iterator<char, CharT, Traits>(os), tmp, os.getloc());
    return os;
}

template<class CharT, class Traits>
inline std::basic_istream<CharT, Traits>& operator >> (std::basic_istream<CharT, Traits>& is, BizGuid& guid)
{
    // 移植性がない。本来はis.narrowでTからcharに変換すべき
    // WindowsではASCIIとUTF-16なので、期待通りに動くが

    std::array<CharT, 64> s;
    if (is.read(s.data(), s.size()))
    {
        boost::algorithm::unhex(s, guid.data.begin());
    }
    return is;
}

struct DataBlob
{
    DWORD cbData;
    boost::shared_ptr<BYTE> pbData;
};
