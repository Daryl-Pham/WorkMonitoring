#include <iostream>
#include <stdlib.h>
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "crypt32.lib")

using namespace std;
using namespace utility;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

namespace
{
    void resetCin()
    {
        cin.clear();
        cin.ignore(1024, '\n');
    }
}  // namespace

int main()
{
    wstring proxyUser;
    wstring proxyPassword;

    for (;;)
    {
        http_client_config client_config;
        web::web_proxy proxy(web::web_proxy::use_auto_discovery);

        if (!proxyUser.empty() || !proxyPassword.empty())
        {
            web::credentials cred(proxyUser, proxyPassword);
            proxy.set_credentials(cred);
        }
        client_config.set_proxy(proxy);

        client_config.set_nativehandle_options([](web::http::client::native_handle handle) {
            WINHTTP_PROXY_INFO pi;
            DWORD dwSize = sizeof(pi);
            if (!WinHttpQueryOption(handle, WINHTTP_OPTION_PROXY, &pi, &dwSize))
            {
                cerr << "WinHttpQueryOption() failed. " << GetLastError() << endl;
            }

            if (pi.dwAccessType == WINHTTP_ACCESS_TYPE_NO_PROXY)
            {
                cout << "No Proxy" << endl;
            }
            else
            {
                wcout << L"Proxy URL: " << pi.lpszProxy << endl;
            }
        });

        // Open a stream to the file to write the HTTP response body into.
        http_client client(U("https://httpbin.org/get"), client_config);

        try
        {
            web::http::status_code statusCode = 0;
            string responseBody;
            client.request(methods::GET)
                .then([&responseBody, &statusCode](http_response response)
                {
                    statusCode = response.status_code();
                    responseBody = response.extract_utf8string().get();
                })
                // Wait for the entire response body to be written into the file.
                .wait();

            cout << "Response status code " << statusCode << " returned." << endl;
            cout << responseBody << endl;

            cout << "Continue [yn]: ";
            char input;
            cin >> input;
            if (input == 'n')
            {
                break;
            }
            resetCin();

            if (statusCode == status_codes::ProxyAuthRequired)
            {
                cout << "The proyx requires authentication." << endl;
                cout << "Proxy User: ";
                wcin >> proxyUser;
                resetCin();

                cout << "Proxy Password: ";
                wcin >> proxyPassword;
                resetCin();
            }
        }
        catch (const std::exception& e)
        {
            cerr << "Got an exception " << e.what() << endl;
            return 1;
        }
    }

    return 0;
}
