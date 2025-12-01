#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#define _CMPI_BUF_MIN_DFL_CAP 1024


static void _ensure_buf_capacity(cmpi_buffer_t *buf, size_t needed)
{
    if(buf->len + needed <= buf->cap)
        return;
    
    size_t new_cap = buf->cap == 0 
                   ? _CMPI_BUF_MIN_DFL_CAP 
                   : buf->cap * 2;
    
    while(new_cap < buf->len + needed)
        new_cap *= 2;
    
    buf->data = realloc(buf->data, new_cap);
    buf->cap = new_cap;
}


static void _init_buf(cmpi_buffer_t *buf)
{
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}


static void _cleanup_buf(cmpi_buffer_t *buf)
{
    if(buf->data)
    {
        free(buf->data);
        buf->data = NULL;
    }
}


// Wait while terminator is not present in buffer or connection is not terminated
// Returns 0 if recved terminator, -1 on error
static int _wait_for_input(cmpi_connection_t *conn, const char *terminators)
{
    while(conn->is_connected && !conn->error_state)
    {
        if(conn->input_buf.len > 0)
        {
            for(size_t i = 0; i < conn->input_buf.len; ++i)
            {
                if(strchr(terminators, conn->input_buf.data[i]))
                    return -1;
            }
        }

        uv_run(&conn->loop, UV_RUN_ONCE);
    }

    return 0;
}


// Memory allocation for recving
// We will allocate it in our input buffer, libuv will write directly into back of it
static void _on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    cmpi_connection_t *conn = handle->data;
    _ensure_buf_capacity(&conn->input_buf, suggested_size);
    buf->base = conn->input_buf.data + conn->input_buf.len;
    buf->len = conn->input_buf.cap - conn->input_buf.len;
}


// When recving of some piece of data ended
static void _on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    cmpi_connection_t *conn = stream->data;
    if(nread > 0)
        conn->input_buf.len += nread;
    else if(nread == 0)
        return;
    
    if(nread != UV_EOF)
    {
        fprintf(stderr, "Socket Read Error: %s\n", uv_strerror(nread));
        conn->error_state = 1;
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
        conn->error_state = 1;
        return;
    }

    conn->is_connected = 1;
    // Start recving data
    uv_read_start((uv_stream_t *)&conn->socket, _on_alloc, _on_read);
}


// When writing is done (cmpi_net_flush)
static void _on_write_done(uv_write_t *req, int status)
{
    cmpi_connection_t *conn = req->data;
    _cleanup_buf(&conn->output_buf);
    
    free(req);
    if(status < 0)
        fprintf(stderr, "Write Failed: %s\n", uv_strerror(status));
}


void cmpi_net_connect(cmpi_connection_t *conn, const char *address, int port)
{
    uv_loop_init(&conn->loop);

    uv_tcp_init(&conn->loop, &conn->socket);
    conn->socket.data = conn;
    conn->connect_req.data = conn;

    conn->is_connected = 0;
    conn->error_state = 0;
    conn->input_buf.data = NULL;
    _init_buf(&conn->input_buf);
    _init_buf(&conn->output_buf);

    struct sockaddr_in dst;
    uv_ip4_addr(address, port, &dst);

    uv_tcp_connect(&conn->connect_req, &conn->socket, (struct sockaddr *)&dst, _on_connect);
    while(!conn->is_connected && !conn->error_state)
    {
        uv_run(&conn->loop, UV_RUN_ONCE);
    }
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

    // Run the loop to clean up handles
    uv_run(&conn->loop, UV_RUN_NOWAIT);
    // Just in case
    uv_run(&conn->loop, UV_RUN_NOWAIT);

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

    _ensure_buf_capacity(&conn->output_buf, size);
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
        conn->error_state = 1;
        return;
    }

    req->data = conn;
    uv_buf_t buf = uv_buf_init(conn->output_buf.data, conn->output_buf.len);

    int r = uv_write(req, (uv_stream_t *)&conn->socket, &buf, 1, _on_write_done);
    if(r < 0) 
    {
        fprintf(stderr, "Flush Error: %s\n", uv_strerror(r));
        _cleanup_buf(&conn->output_buf);
        free(req);
    }

    uv_run(&conn->loop, UV_RUN_NOWAIT);
}


int cmpi_net_scanf(cmpi_connection_t *conn, const char *fmt, const char *terminators, ...)
{
    return -1;
}


int cmpi_count_separators(cmpi_connection_t *conn, int sep, int terminator)
{
    char terms[2] = {terminator, 0};

    if(_wait_for_input(conn, terms) < 0)
        return -1;
    
    int count = 0;
    for(size_t i = 0; i < conn->input_buf.len; ++i)
    {
        if(conn->input_buf.data[i] == terminator)
            break;
        
        if(conn->input_buf.data[i] == sep)
            ++count;
    }
    return count;
}
