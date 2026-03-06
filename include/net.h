#ifndef CMPI_NET_H
#define CMPI_NET_H

#include <stddef.h>
#include <uv.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    char *data;
    size_t len;
    size_t cap;
} cmpi_net_buffer_t;

typedef struct {
    uv_loop_t loop;                 //!< Private
    uv_tcp_t socket;                //!< Private
    uv_connect_t connect_req;       //!< Private
    cmpi_net_buffer_t input_buf;    //!< Private
    cmpi_net_buffer_t output_buf;   //!< Private
    int error_uv;                   //!< Private
    int error_internal;             //!< Private

    /// If true, then connected to a server, false otherwise
    bool is_connected;
    /// If true, then an error happened.
    /// cmpi_net functions will immediately return if a connection with error_state set was passed.
    bool error_state;
} cmpi_connection_t;

/** 
 * Establishes a connection to MCPI. 
 *
 * Check conn->is_connected after call of this function.
 */
void cmpi_net_connect(cmpi_connection_t *conn, const char *address, int port);

/** 
 * Establishes a connection to MCPI using defaults (localhost:4711).
 *
 * Check conn->is_connected after call of this function.
 */
void cmpi_net_connect_default(cmpi_connection_t *conn);

/** Closes MCPI connection. */
void cmpi_net_close(cmpi_connection_t *conn);

/**
 * Sends formatted string to connection.
 * 
 * It supports these %-specificators:
 * - %d, expects int, formats as a decimal integer.
 * - %g, expects double, formats as a floating point number using as shortest way as possible.
 * - %s, expects const char *, formats as a string.
 * 
 * It doesn't guarantee if all the data will be sent. To ensure it, use cmpi_net_flush.
 */
void cmpi_net_printf(cmpi_connection_t *conn, const char *fmt, ...);

/** Flushes buffer of connection. */
void cmpi_net_flush(cmpi_connection_t *conn);

/**
 * Scans connection input for fmt until any of terminators are encountered and returns it.
 * 
 * It supports these %-specificators:
 * - %d, expects int *, reads a decimal integer.
 * - %f, expects double *, reads a floating point number.
 * - %s, expects char * and size_t (buffer size), reads a string.
 * 
 * On error not letting to sufficiently read input, or insufficient input, returns -1.
 */
int cmpi_net_scanf(cmpi_connection_t *conn, const char *fmt, const char *terminators, ...);

/** Returns count of sep presences until terminator is encountered. */
int cmpi_net_count_separators(cmpi_connection_t *conn, int sep, int terminator);

/** 
 * Returns statically stored net system error message.
 * 
 * - "Success" if no error on this connection
 * - "Unknown" if an unknown error on this connection
 * - Error message from net backend (libuv).
 *
 * Don't free given memory. NULL is never returned.
 */
const char *cmpi_net_get_error(cmpi_connection_t *conn);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // CMPI_NET_H
