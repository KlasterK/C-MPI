#include <winsock2.h>
#include <iostream>
#include <tuple>
#include <functional>
#include <cstdarg>

template<auto Func, typename... Args>
class Cleanup
{
public:
    Cleanup(Args... args) : m_args(std::make_tuple(args...)) {}
    ~Cleanup() { std::apply(Func, m_args); }

private:
    std::tuple<Args...> m_args;
};

void netscanf(const char *fmt, const char *terminators, ...)
{
    va_list args;
    va_start(args, terminators);
    char number_buf[350];

    for(int input_char; *fmt != 0; ++fmt)
    {
        if(*fmt == '%')
        {
            ++fmt;
            if(*fmt == 'd')
            {
                int next_char = *++fmt;
                int *dst = va_arg(args, int *);
                *dst = 0;

                for(;;)
                {
                    input_char = getchar();
                    if(input_char == next_char)
                        continue;

                    int digit = input_char - '0';
                    if(digit < 0 || digit > 9)
                        break;

                    *dst *= 10;
                    *dst += digit;
                }
            }
            else if(*fmt == 'f')
            {
                int next_char = *++fmt;
                double *dst = va_arg(args, double *);

                for(int i = 0;;)
                {
                    input_char = getchar();
                    if(input_char == next_char)
                    {
                        number_buf[i] = 0;
                        *dst = strtod(number_buf, NULL);
                    }
                    else if (
                        input_char > '0' && input_char < '9'
                        || input_char == '.'
                        || tolower(input_char) == 'e'
                        || input_char == '+'
                        || input_char == '-'
                    ) number_buf[i++] = input_char;
                    else break;
                }
            }
            else if(*fmt == 's')
            {
                char *dst = va_arg(args, char *);
                size_t size = va_arg(args, size_t);
                gets_s(dst, size);
            }
        }

        input_char = getchar();
        if(strchr(terminators, input_char) != NULL || input_char != *fmt)
            goto cleanup;
    }

cleanup:
    va_end(args);
}

int main()
{
    constexpr char *fmt = "world.getNtt(%s,%d,%f,%s)";
    std::cout << "fmt: " << fmt << std::endl;
    char a[10], d[20];
    int b; 
    double c;
    for(;;)
    {
        netscanf(fmt, "\n", a, 10, b, c, d, 20);
        std::cout << a << ", " << b << ", " << c << ", " << d << std::endl;
    }
    WSADATA ws;
    if (int result = WSAStartup(MAKEWORD(1, 1), &ws); FAILED(result))
    {
        int error = WSAGetLastError();
        return 1;
    }
    Cleanup<WSACleanup> raii_wsa;

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET)
    {
        return 1;
    }
    Cleanup<closesocket, SOCKET> raii_shutdown(sock);
}