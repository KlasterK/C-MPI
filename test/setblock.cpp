#include "minecraft.h"
#include <iostream>

#define _TST(fn, conn, ...) \
{ \
    (fn)((conn), ##__VA_ARGS__); \
    if((conn)->error_state) \
    { \
        std::cout << #fn \
                  << ": is_connected=" << (conn)->is_connected \
                  << ", error_state=" << (conn)->error_state << std::endl; \
        exit((conn)->error_state); \
    } \
}
constexpr int GOLD_BLOCK = 41;

int main()
{
    cmpi_connection_t conn;

    _TST(cmpi_net_connect_default, &conn);
    _TST(cmpi_set_block, &conn, 2, 66, -83, GOLD_BLOCK);
    _TST(cmpi_net_close, &conn);

    return 0;
}
