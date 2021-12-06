#include <gtest/gtest.h>
#include "SessionEnumerator.h"
#include "UtilityForTest.h"

TEST(testSessionEnumerator_NotRunOnCI, EnumerateSession)
{
    DWORD sessionId = 0;
    ASSERT_TRUE(ProcessIdToSessionId(GetCurrentProcessId(), &sessionId));

    bool gotSessionId = false;
    SessionEnumerator se;
    se.EnumerateSession([&sessionId , &gotSessionId](DWORD sid)
        {
            if (sessionId == sid)
            {
                gotSessionId = true;
            }
        });
    EXPECT_TRUE(gotSessionId);
}
