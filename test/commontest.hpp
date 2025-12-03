#ifndef CMPI_COMMONTEST_HPP
#define CMPI_COMMONTEST_HPP


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

#define _STORE(dst, fn, conn, ...) \
{ \
    (dst) = (fn)((conn), ##__VA_ARGS__); \
    if((conn)->error_state) \
    { \
        std::cout << #fn \
                  << ": is_connected=" << (conn)->is_connected \
                  << ", error_state=" << (conn)->error_state << std::endl; \
        exit((conn)->error_state); \
    } \
}

#define _ASSERT(expr) \
{ \
    if(!!(expr)) \
    { \
        std::cout << "Assertion failed: " << #expr << std::endl; \
        exit(1); \
    } \
}

constexpr int
    STONE       = 1,
    GOLD_BLOCK  = 41,
    IRON_BLOCK  = 42;

constexpr int DFLX = 2, DFLY = 66, DFLZ = -83;


#endif // CMPI_COMMONTEST_HPP
