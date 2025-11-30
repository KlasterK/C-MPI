#include "minecraft.h"
#include <iostream>

int main()
{
    cmpi_connection_t conn;
    cmpi_net_connect_default(&conn);

    cmpi_set_block(&conn, 2, 66, -83, 41); // golden block
    // std::cout << "Block @(0; 50; 0) " << cmpi_get_block(&conn, 0, 50, 0) << std::endl;

    cmpi_net_close(&conn);
}
