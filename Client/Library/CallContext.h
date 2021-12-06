#pragma once

enum class TypeCallContext
{
    WINDOWS_DEVICE_LOGS,
    WINDOWS_WORK_LOGS,
    TIME_CARDS_TODAY,
    WINDOWS_USER_CONDITION_LOGS,
    WINDOWS_DAY_OFF_REQUESTS,
    WINDOWS_APPLICATION_LOGS,
    WINDOWS_AUTHENTICATE,
    WINDOWS_WORK_TARGET,
};

class CallContext
{
public:
    /**
     * Wait for response from server.
     * @return true if succeeded. false if failed.
     */
    virtual bool Wait() = 0;

    /**
     * Stop the request
    */
    virtual void Cancel() = 0;

    /**
     * HTTP Status Code. You can call this after Wait() is succeeded.
     * @return HTTP Status Code from server.
     */
    virtual int StatusCode() = 0;

    /**
     * HTTP Response Body. You can call this after Wait() is succeeded.
     * @return HTTP response body from server.
     */
    virtual std::wstring Body() = 0;


    virtual TypeCallContext GetTypeCallContext() const = 0;
    virtual std::wstring GetStringType() const = 0;

    virtual ~CallContext() = default;
};

class CallContextForWindowsDeviceLogs : public CallContext
{
public:
    /**
     * HTTP Status Code. You can call this after Wait() is succeeded.
     * @return HTTP Status Code from server.
     *         201: Created
     *         401: UnauthorizedError
     *         500: Internal Server Error
     */

    TypeCallContext GetTypeCallContext() const
    {
        return TypeCallContext::WINDOWS_DEVICE_LOGS;
    }

    std::wstring GetStringType() const
    {
        return L"WINDOWS_DEVICE_LOGS";
    }

    virtual ~CallContextForWindowsDeviceLogs() = default;
};

class CallContextForWindowsWorkLogs : public CallContext
{
public:
    /**
     * HTTP Status Code. You can call this after Wait() is succeeded.
     * @return HTTP Status Code from server.
     *         201: Created
     *         400: Bad Request
     *         401: UnauthorizedError
     *         500: Internal Server Error
     */

    TypeCallContext GetTypeCallContext() const
    {
        return TypeCallContext::WINDOWS_WORK_LOGS;
    }

    std::wstring GetStringType() const
    {
        return L"WINDOWS_WORK_LOGS";
    }


    virtual ~CallContextForWindowsWorkLogs() = default;
};

class CallContextForTimeCardsToday : public CallContext
{
public:
    /**
     * HTTP Status Code. You can call this after Wait() is succeeded.
     * @return HTTP Status Code from server.
     *         200: Okie Responsive
     *         400: Bad Request
     *         401: UnauthorizedError
     *         500: Internal Server Error
     */

    TypeCallContext GetTypeCallContext() const
    {
        return TypeCallContext::TIME_CARDS_TODAY;
    }

    std::wstring GetStringType() const
    {
        return L"TIME_CARDS_TODAY";
    }


    virtual ~CallContextForTimeCardsToday() = default;
};

class CallContextForWindowsUserConditionLogs : public CallContext
{
public:
    /**
     * HTTP Status Code. You can call this after Wait() is succeeded.
     * @return HTTP Status Code from server.
     *         201: Created
     *         400: Bad Request
     *         401: UnauthorizedError
     *         500: Internal Server Error
     */

    TypeCallContext GetTypeCallContext() const
    {
        return TypeCallContext::WINDOWS_USER_CONDITION_LOGS;
    }

    std::wstring GetStringType() const
    {
        return L"WINDOWS_USER_CONDITION_LOGS";
    }


    virtual ~CallContextForWindowsUserConditionLogs() = default;
};

class CallContextForWindowsDayOffRequests : public CallContext
{
public:
    /**
     * HTTP Status Code. You can call this after Wait() is succeeded.
     * @return HTTP Status Code from server.
     *         201: Created
     *         400: Bad Request
     *         401: UnauthorizedError
     *         500: Internal Server Error
     */

    TypeCallContext GetTypeCallContext() const
    {
        return TypeCallContext::WINDOWS_DAY_OFF_REQUESTS;
    }

    std::wstring GetStringType() const
    {
        return L"WINDOWS_DAY_OFF_REQUESTS";
    }


    virtual ~CallContextForWindowsDayOffRequests() = default;
};

class CallContextForWindowsApplicationLogs : public CallContext
{
public:
    /**
     * HTTP Status Code. You can call this after Wait() is succeeded.
     * @return HTTP Status Code from server.
     *         201: Created
     *         401: UnauthorizedError
     *         500: Internal Server Error
     */

    TypeCallContext GetTypeCallContext() const
    {
        return TypeCallContext::WINDOWS_APPLICATION_LOGS;
    }

    std::wstring GetStringType() const
    {
        return L"WINDOWS_APPLICATION_LOGS";
    }

    virtual ~CallContextForWindowsApplicationLogs() = default;
};

class CallContextForWindowsAuthenticate : public CallContext
{
public:
    /**
     * HTTP Status Code. You can call this after Wait() is succeeded.
     * @return HTTP Status Code from server.
     *         200: OK
     *         400: Bad Request
     */

    TypeCallContext GetTypeCallContext() const
    {
        return TypeCallContext::WINDOWS_AUTHENTICATE;
    }

    std::wstring GetStringType() const
    {
        return L"WINDOWS_AUTHENTICATE";
    }


    virtual ~CallContextForWindowsAuthenticate() = default;
};

class CallContextForWindowsWorkTarget : public CallContext
{
public:
    /**
     * HTTP Status Code. You can call this after Wait() is succeeded.
     * @return HTTP Status Code from server.
     *         201: Created
     *         400: Bad Request
     *         401: UnauthorizedError
     *         500: Internal Server Error
     */

    TypeCallContext GetTypeCallContext() const override
    {
        return TypeCallContext::WINDOWS_WORK_TARGET;
    }

    std::wstring GetStringType() const override
    {
        return L"WINDOWS_WORK_TARGET";
    }

    ~CallContextForWindowsWorkTarget() override = default;
};
