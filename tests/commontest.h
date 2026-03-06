#ifndef CMPI_COMMONTEST_H
#define CMPI_COMMONTEST_H

#include <stdio.h>
#include <stdlib.h>
#include "net.h"

#define _TST(fn, conn, ...) \
{ \
    (fn)((conn), ##__VA_ARGS__); \
    if((conn)->error_state) \
    { \
        printf( \
            "%s: is_connected=%d, error_state=%d, cmpi_net_get_error='%s'\n", \
            #fn, \
            (conn)->is_connected, \
            (conn)->error_state, \
            cmpi_net_get_error((conn)) \
        ); \
        exit(1); \
    } \
}

#define _STORE(dst, fn, conn, ...) \
{ \
    (dst) = (fn)((conn), ##__VA_ARGS__); \
    if((conn)->error_state) \
    { \
        printf( \
            "%s: is_connected=%d, error_state=%d, cmpi_net_get_error='%s'\n", \
            #fn, \
            (conn)->is_connected, \
            (conn)->error_state, \
            cmpi_net_get_error((conn)) \
        ); \
        exit(1); \
    } \
}

#define _ASSERT(expr) \
{ \
    if(!!(expr)) \
    { \
        printf("Assertion failed: %s\n", #expr); \
        exit(1); \
    } \
}

static const int
    STONE       = 1,
    GOLD_BLOCK  = 41,
    IRON_BLOCK  = 42;

static const int DFLX = 2, DFLY = 66, DFLZ = -83;


#endif // CMPI_COMMONTEST_H
