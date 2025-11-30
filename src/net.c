#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#define _CALL_CHECK_ERR_RET(expr) if((expr) < 0) { conn->error_state = 1; return; }
#define _CALL_CHECK_ERR_CLEANUP(expr) if((expr) < 0) { conn->error_state = 1; goto cleanup; }

static void _on_connect(uv_connect_t *request, int status)
{
    cmpi_connection_t *conn = request->data;
    if(status < 0)
        conn->error_state = 1;
}

void cmpi_net_connect(cmpi_connection_t *conn, const char *address, int port)
{
    // Initialize fields
    conn->loop = uv_default_loop();
    _CALL_CHECK_ERR_RET(uv_tcp_init(conn->loop, &conn->socket))
    conn->error_state = 0;
    conn->is_connected = 0;

    // Make address
    struct sockaddr_in addr;
    _CALL_CHECK_ERR_RET(uv_ip4_addr(address, port, &addr))

    // Make conection request with cmpi_connection_t as data
    uv_connect_t request;
    request.data = conn;

    // Try to connect
    _CALL_CHECK_ERR_RET(uv_tcp_connect(&request, &conn->socket, &addr, _on_connect))
    _CALL_CHECK_ERR_RET(uv_run(conn->loop, UV_RUN_DEFAULT)) // Callback will stop the loop

    // If failed, close
    if(conn->error_state) 
    {
        conn->is_connected = 0;
        uv_close(&conn->socket, NULL);
        uv_run(conn->loop, UV_RUN_DEFAULT);
    }
}

void cmpi_net_connect_default(cmpi_connection_t *conn)
{
    cmpi_net_connect(conn, "localhost", 4711);
}

void cmpi_net_close(cmpi_connection_t *conn)
{
    if(conn->is_connected == 0) return;

    uv_close(&conn->socket, NULL);
    conn->is_connected = 0;

    _CALL_CHECK_ERR_RET(uv_run(conn->loop, UV_RUN_DEFAULT))
}

void cmpi_net_printf(cmpi_connection_t *conn, const char *fmt, ...)
{
    va_list args, args_copy;
    va_start(args, fmt);

    va_copy(args_copy, args);
    int buf_size = 1 + vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    char buf[buf_size];
    vsnprintf(buf, buf_size, fmt, args);
    va_end(args);

    uv_write_t req;
    uv_buf_t bufs[] = {uv_buf_init(buf, buf_size)};

    _CALL_CHECK_ERR_RET(uv_write(&req, &conn->socket, bufs, 1, NULL))
    _CALL_CHECK_ERR_RET(uv_run(conn->loop, UV_RUN_DEFAULT))
}

void cmpi_net_flush(cmpi_connection_t *conn)
{
    // TODO: We wait for sending in cmpi_net_printf, so there's no need to flush anything
}

static void _alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) 
{
    // Мы игнорируем suggested_size и выделяем наш стандартный блок
    buf->base = malloc(suggested_size);
    if (buf->base == NULL) 
    {
        // Крайне важно: если не удалось выделить память, нужно уведомить libuv
        buf->len = 0; 
        return;
    }
    buf->len = suggested_size;
}

int cmpi_net_scanf(cmpi_connection_t *conn, const char *fmt, const char *terminators, ...)
{
    if (!conn->is_connected) return -1;
    
    // 1. Сброс состояния чтения
    conn->recv_len = 0;
    conn->read_complete = 0;
    conn->last_read_status = 0;

    // 2. Инициализация хэндла данными
    conn->socket.data = conn;
    
    // 3. Запуск асинхронного чтения
    int r = uv_read_start((uv_stream_t*)&conn->socket, _alloc_cb, read_cb);
    if (r != 0) {
        return r;
    }

    // 4. Блокировка: Ждем, пока read_cb не установит read_complete и не вызовет uv_stop
    uv_run(conn->loop, UV_RUN_DEFAULT); 
    
    // 5. Обработка завершения
    if (conn->last_read_status < 0) {
        // Ошибка или EOF
        return conn->last_read_status; 
    }
    
    if (!conn->read_complete) {
        // Непредвиденный выход из цикла (очень редкий случай)
        return -1;
    }
    
    // 6. Парсинг ответа (аналог scanf)
    va_list args;
    va_start(args, format);
    
    // Мы используем vsprintf для парсинга данных, которые находятся в conn->recv_buffer
    // (он уже обрезан и содержит \0)
    int parsed_count = vsscanf(conn->recv_buffer, format, args);
    
    va_end(args);

    // 7. Очистка буфера для следующего ответа
    conn->recv_len = 0; 
    
    // 8. Сдвиг буфера (если ответ был не полным, но это усложнение)
    // В MVP мы просто полагаемся на то, что буфер очищается
    
    return parsed_count;
}
