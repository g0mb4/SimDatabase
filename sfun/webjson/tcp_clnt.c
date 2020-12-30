#include "tcp_clnt.h"

static SOCKET _sock = INVALID_SOCKET;

int tcp_connect(const char * addr, int port) {
    WSADATA wsaData;
    char port_str[6];
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    int res;

    res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != 0) {
        dconsole_error("WSAStartup() failed with error: %d", res);
        return -1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    _itoa(port, port_str, 10);
    res = getaddrinfo(addr, port_str, &hints, &result);
    if (res != 0) {
        dconsole_error("getaddrinfo() failed with error: %d", res);
        WSACleanup();
        return -2;
    }

    /* Attempt to connect to an address until one succeeds */
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        _sock = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (_sock == INVALID_SOCKET) {
            dconsole_error("socket() failed with error: %d", WSAGetLastError());
            WSACleanup();
            return -3;
        }

        res = connect(_sock, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (res == SOCKET_ERROR) {
            closesocket(_sock);
            _sock = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);

    if (_sock == INVALID_SOCKET) {
        dconsole_error("connect() failed");
        WSACleanup();
        return -4;
    }

    dconsole_write(GREEN, BLACK, "connected to %s:%d", addr, port);

    return 0;
}

void tcp_disconnect(void) {
    if (_sock != INVALID_SOCKET) {
        int res = shutdown(_sock, SD_SEND);
        if (res == SOCKET_ERROR) {
            dconsole_error("shutdown() failed with error: %d",
                           WSAGetLastError());
        }

        closesocket(_sock);
        WSACleanup();
    }
}

int tcp_get_data(str_buf_t * buf, const char * start_date,
                 const char * start_time, const char * end_date,
                 const char * end_time, const char * sensors) {
    int res;
    char recvbuf[512];
    int recvbuflen = 512;

    // /api/api.php?startDate=2018-04-17&startTime=06:00:00&endDate=2018-04-17&endTime=18:00:00&sensors=I,TW
    str_buf_t * send_str = str_buf_create();
    str_buf_add(send_str, "GET /api/api.php?startDate=");
    str_buf_add(send_str, start_date);
    str_buf_add(send_str, "&startTime=");
    str_buf_add(send_str, start_time);
    str_buf_add(send_str, "&endDate=");
    str_buf_add(send_str, end_date);
    str_buf_add(send_str, "&endTime=");
    str_buf_add(send_str, end_time);
    str_buf_add(send_str, "&sensors=");
    str_buf_add(send_str, sensors);
    str_buf_add(send_str, " HTTP/1.0\r\n\r\n");

    res = send(_sock, send_str->str, send_str->len, 0);
    str_buf_destroy(send_str);
    if (res == -1) {
        dconsole_error("send() failed with error: %d", WSAGetLastError());
        closesocket(_sock);
        WSACleanup();
        return -1;
    }
    // Receive until the peer closes the connection
    memset(recvbuf, 0, recvbuflen);
    do {
        res = recv(_sock, recvbuf, recvbuflen, 0);
        if (res > 0) {
            if (str_buf_addn(buf, recvbuf, recvbuflen) < 0) {
                dconsole_error("str_buf_addn() failed");
                closesocket(_sock);
                WSACleanup();
                return -2;
            }
            memset(recvbuf, 0, recvbuflen);
        } else if (res == 0) {
            dconsole_info("connection closed");
        } else {
            dconsole_error("recv() failed with error: %d", WSAGetLastError());
        }
    } while (res > 0);

    dconsole_write(LIGHTGRAY, BLACK, "recieved:\n%s", buf->str);

    return 0;
}
