#include "minecraft.h"
#include <iostream>

constexpr int GOLD_BLOCK = 41;

int main()
{
    int error_state{};
    cmpi_connection_t conn;

    cmpi_net_connect_default(&conn);
    std::cout << "cmpi_net_connect_default : is_connected=" << conn.is_connected
              << ", error_state=" << conn.error_state << std::endl;
    error_state |= conn.error_state;

    cmpi_set_block(&conn, 2, 66, -83, GOLD_BLOCK);
    std::cout << "cmpi_set_block : is_connected=" << conn.is_connected
              << ", error_state=" << conn.error_state << std::endl;
    error_state |= conn.error_state;

    cmpi_net_close(&conn);
    std::cout << "cmpi_net_close : is_connected=" << conn.is_connected
              << ", error_state=" << conn.error_state << std::endl;
    error_state |= conn.error_state;

    return error_state;
}
