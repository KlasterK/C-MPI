#include "net.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <uv.h>

#define _CMPI_BUF_MIN_DFL_CAP 1024

#define _CMPI_INTERNALERROR_OOM 10


static void _init_buf(cmpi_net_buffer_t *buf)
{
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}


static void _cleanup_buf(cmpi_net_buffer_t *buf)
{
    if(buf->data)
    {
        free(buf->data);
        buf->data = NULL;
    }
    
    buf->len = 0;
    buf->cap = 0;
}


// Returns false if OOM, true otherwise.
static bool _ensure_buf_capacity(cmpi_net_buffer_t *buf, size_t needed)
{
    if(buf->len + needed <= buf->cap)
        return true;
    
    size_t new_cap = buf->cap == 0 
                   ? _CMPI_BUF_MIN_DFL_CAP 
                   : buf->cap * 2;
    
    while(new_cap < buf->len + needed)
        new_cap *= 2;
    
    char *data = realloc(buf->data, new_cap);
    if(data == NULL)
    {
        _cleanup_buf(buf);
        return false;
    }

    buf->data = data;
    buf->cap = new_cap;
    return true;
}


static void _shift_buf(cmpi_net_buffer_t *buf, int n)
{
    if(n <= 0)
        return;
    
    if(n >= buf->len)
    {
        buf->len = 0;
        return;
    }

    buf->len -= n;
    memmove(buf->data, buf->data + n, buf->len);
}


// Wait while terminator is not present in buffer or connection is not terminated.
// Returns conn->input_buf index to received terminator, -1 on error.
static int _wait_for_input(cmpi_connection_t *conn, const char *terminators)
{
    while(conn->is_connected && !conn->error_state)
    {
        for(size_t i = 0; i < conn->input_buf.len; ++i)
        {
            if(NULL != strchr(terminators, conn->input_buf.data[i]))
                return i;
        }
        
        conn->error_uv = uv_run(&conn->loop, UV_RUN_ONCE);
        if(0 != conn->error_uv)
            conn->error_state = true;
    }

    return -1;
}


// Memory allocation for recving.
// We will allocate it in our input buffer, libuv will write directly into back of it.
static void _on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    cmpi_connection_t *conn = handle->data;
    if(!_ensure_buf_capacity(&conn->input_buf, suggested_size))
    {
        conn->error_state = true;
        conn->error_internal = _CMPI_INTERNALERROR_OOM;
        return;
    }

    buf->base = conn->input_buf.data + conn->input_buf.len;
    buf->len = conn->input_buf.cap - conn->input_buf.len;
}


/// When recving of some piece of data ended.
static void _on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    cmpi_connection_t *conn = stream->data;
    if(nread >= 0)
    {
        conn->input_buf.len += nread;
        return;
    }
    
    if(nread != UV_EOF)
    {
        // nread is uv error code
        conn->error_state = true;
        conn->error_uv = nread;
    }

    // Server closed the connection (at least, its own output stream)
    conn->is_connected = 0;
}


// When connection established
static void _on_connect(uv_connect_t *req, int status)
{
    cmpi_connection_t *conn = req->data;
    if(status < 0)
    {
        conn->error_state = true;
        conn->error_uv = status;
        return;
    }

    conn->is_connected = 1;
    // Start recving data
    conn->error_uv = uv_read_start((uv_stream_t *)&conn->socket, _on_alloc, _on_read);
    if(0 != conn->error_uv)
        conn->error_state = true;
}


// When writing is done (cmpi_net_flush)
static void _on_write_done(uv_write_t *req, int status)
{
    cmpi_connection_t *conn = req->data;
    _cleanup_buf(&conn->output_buf);
    
    free(req);
    if(status < 0)
    {
        conn->error_state = true;
        conn->error_uv = status;
    }
}


void cmpi_net_connect(cmpi_connection_t *conn, const char *address, int port)
{
    conn->is_connected = false;
    conn->error_state = false;
    conn->error_uv = 0;
    conn->error_internal = 0;

    conn->error_uv = uv_loop_init(&conn->loop);
    if(0 != conn->error_uv)
        goto error_close_nothing;

    conn->error_uv = uv_tcp_init(&conn->loop, &conn->socket);
    if(0 != conn->error_uv)
        goto error_close_loop;
    conn->socket.data = conn;

    conn->connect_req.data = conn;

    _init_buf(&conn->input_buf);
    _init_buf(&conn->output_buf);

    struct sockaddr_in dst;
    conn->error_uv = uv_ip4_addr(address, port, &dst); // TODO: auto-selection of IPv4 or IPv6 using getaddrinfo
    if(0 != conn->error_uv)
        goto error_close_socket_loop;

    conn->error_uv = uv_tcp_connect(&conn->connect_req, &conn->socket, (struct sockaddr *)&dst, _on_connect);
    if(0 != conn->error_uv)
        goto error_close_socket_loop;
    
    while(!conn->is_connected && !conn->error_state)
    {
        conn->error_uv = uv_run(&conn->loop, UV_RUN_ONCE);
        if(0 != conn->error_uv)
            goto error_close_socket_loop;
    }
    return; // success

error_close_socket_loop:
    uv_close((uv_handle_t *)&conn->socket, NULL);
    uv_run(&conn->loop, UV_RUN_DEFAULT);
error_close_loop:
    uv_loop_close(&conn->loop);
error_close_nothing:
    conn->error_state = true;
}


void cmpi_net_connect_default(cmpi_connection_t *conn)
{
    cmpi_net_connect(conn, "127.0.0.1", 4711);
}


void cmpi_net_close(cmpi_connection_t *conn)
{
    if(!conn->is_connected)
        return;

    if(!uv_is_closing((uv_handle_t *)&conn->socket))
        uv_close((uv_handle_t *)&conn->socket, NULL);

    uv_run(&conn->loop, UV_RUN_DEFAULT);

    uv_loop_close(&conn->loop);
    _cleanup_buf(&conn->input_buf);
    _cleanup_buf(&conn->output_buf);
    conn->is_connected = 0;
}


void cmpi_net_printf(cmpi_connection_t *conn, const char *fmt, ...)
{
    if(!conn->is_connected)
        return;
    
    va_list args, args_copy;
    va_start(args, fmt);

    // Get size of new string
    va_copy(args_copy, args);
    int size = 1 + vsnprintf(NULL, 0, fmt, args_copy); // with NUL
    va_end(args_copy);

    if(!_ensure_buf_capacity(&conn->output_buf, size))
    {
        conn->error_state = true;
        conn->error_internal = _CMPI_INTERNALERROR_OOM;
        return;
    }
    char *begin = conn->output_buf.data + conn->output_buf.len;
    int nwrite = conn->output_buf.cap - conn->output_buf.len;

    vsnprintf(begin, nwrite, fmt, args);
    conn->output_buf.len += size - 1; // don't write NUL to socket
    va_end(args);
}


void cmpi_net_flush(cmpi_connection_t *conn)
{
    if (!conn->is_connected || conn->output_buf.len == 0) 
        return;
    
    uv_write_t *req = malloc(sizeof(uv_write_t));
    if(!req)
    {
        // Then our resources may be useful for OS
        _cleanup_buf(&conn->input_buf);
        _cleanup_buf(&conn->output_buf);
        conn->error_state = true;
        conn->error_internal = _CMPI_INTERNALERROR_OOM;
        return;
    }

    req->data = conn;
    uv_buf_t buf = uv_buf_init(conn->output_buf.data, conn->output_buf.len);

    conn->error_uv = uv_write(req, (uv_stream_t *)&conn->socket, &buf, 1, _on_write_done);
    if(0 != conn->error_uv) 
    {
        _cleanup_buf(&conn->output_buf);
        free(req);
        conn->error_state = true;
        return;
    }

    conn->error_uv = uv_run(&conn->loop, UV_RUN_DEFAULT);
    if(0 != conn->error_uv) 
        conn->error_state = true;

    _cleanup_buf(&conn->output_buf);
    free(req);
}


int cmpi_net_scanf(cmpi_connection_t *conn, const char *fmt, const char *terminators, ...)
{
    if(!conn->is_connected)
        return -1;

    int term_idx = _wait_for_input(conn, terminators);
    if(term_idx < 0)
        conn->error_state = true;
    if(conn->error_state)
        return -1;
    
    char *input_it = conn->input_buf.data;
    char *input_end = input_it + term_idx;
    if(input_end == NULL)
        return -1;

    const char *fmt_it = fmt;
    const char *fmt_end = fmt + strlen(fmt);

    int term = *input_end;
    int ret_value = -1;

    va_list args;
    va_start(args, terminators);

    for(; input_it < input_end; ++input_it)
    {
        // If terminator encountered
        if(*input_it == term)
        {
            // Fmt didn't reach the end, it's an error
            ret_value = -1;
            goto cleanup;
        }
        
        // If fmt is not %
        if(*fmt_it != '%')
        {
            // If input doesn't match fmt, it's an error
            if(*input_it != *fmt_it)
            {
                ret_value = -1;
                goto cleanup;
            }

            // Increment fmt
            ++fmt_it;
            // If fmt reached the end and next input is terminator
            if(fmt_it == fmt_end)
            {
                // If next input is not term, it's an error
                ret_value = *++input_it == term ? term : -1;
                goto cleanup;
            }
            continue;
        }

        // Scanning logic
        ++fmt_it;
        if(fmt_it == fmt_end)
        {
            // Scan a pure % character from input
            // If input is % and next input is terminator, it's a success
            ret_value = *input_it == '%' && ++input_it == input_end ? term : -1;
            goto cleanup;
        }

        if(*fmt_it != 's' && *fmt_it != 'd' && *fmt_it != 'f' || *fmt_it == '%')
        {
            // Scan a pure % character
            if(*input_it != '%')
            {
                ret_value = -1;
                goto cleanup;
            }

            // Increment fmt
            ++fmt_it;
            // If fmt reached the end and next input is terminator
            if(fmt_it == fmt_end)
            {
                // If next input is not term, it's an error
                ret_value = *++input_it == term ? term : -1;
                goto cleanup;
            }
            continue;
        }

        // Get next to %-specificator character
        // If fmt is ended here, using terminator
        int spec = *fmt_it;
        int next = ++fmt_it == fmt_end ? term : *fmt_it;

        char *block_begin = input_it;
        for(; *input_it != next; ++input_it);
        
        // Now input_it points to block end
        // Zero it to make a NUL-terminated string
        *input_it = 0;

        if(spec == 's')
        {
            // Write into a string
            char *dst = va_arg(args, char *);
            size_t size = va_arg(args, size_t);
            snprintf(dst, size, "%s", block_begin);
        }
        else if(spec == 'd')
        {
            int *dst = va_arg(args, int *);
            *dst = strtol(block_begin, NULL, 10);
        }
        else // %f
        {
            double *dst = va_arg(args, double *);
            *dst = strtod(block_begin, NULL);
        }

        // Done scanning
        if(fmt_it == fmt_end)
        {
            ret_value = term;
            goto cleanup;
        }
    }

cleanup:
    va_end(args);
    _shift_buf(&conn->input_buf, input_end - conn->input_buf.data);
    return ret_value;
}


int cmpi_net_count_separators(cmpi_connection_t *conn, int sep, int terminator)
{
    char terms[2] = {terminator, 0};


    int term_idx = _wait_for_input(conn, terms);
    if(term_idx < 0)
        conn->error_state = true;
    if(conn->error_state)
        return -1;
    
    int count = 0;
    for(size_t i = 0; i < term_idx; ++i)
    {
        if(conn->input_buf.data[i] == sep)
            ++count;
    }

    _shift_buf(&conn->input_buf, term_idx+1);
    return count;
}


const char *cmpi_net_get_error(cmpi_connection_t *conn)
{
    if(!conn->error_state)
        return "Success";

    if(conn->error_uv != 0)
        return uv_strerror(conn->error_uv);

    switch(conn->error_internal)
    {
    case _CMPI_INTERNALERROR_OOM:
        return "Out Of Memory";
    default:
        return "Unknown (invalid conn->error_literal)";
    }
}
