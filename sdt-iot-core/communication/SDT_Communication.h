#ifndef __SDT_COMMUNICATION_H__
#define __SDT_COMMUNICATION_H__

#include "mbed.h"
#include "SDT_Event_Handler.h"
#include "SDT_Circular_Buffer.h"

class SDT_Communication : public SDT_Event_Handler
{
public:
    typedef enum
    {
        SDT_SOCKET_PROTOCOL_TCP = 0,
        SDT_SOCKET_PROTOCOL_TLS,
    } SDT_Socket_Protocol_t;

    typedef enum
    {
        SDT_SOCKET_RECEIVE_END = 0,
        SDT_SOCKET_RECEIVE_ING,
        SDT_SOCKET_RECEIVE_ERROR,
    } SDT_Socket_Receive_t;

protected:
    char *get_string(int code, unsigned char *buffer, unsigned int start, unsigned int size);

    void *_network;
    Socket *_socket;
    const char *_ssl_ca_pem;
    volatile bool _is_connected;
    unsigned char *_recv_buffer;
    int _recv_size;
    unsigned int _buffer_size;
    SDT_Circular_Buffer<unsigned char> *_circular_buffer;

public:
    SDT_Communication();
    ~SDT_Communication();
    int set_network_interface(void *network);
    int connect(char *host, uint16_t port, unsigned int buffer_size = 1024, const char *ssl_ca_pem = NULL);
    int connect(SocketAddress address, unsigned int buffer_size = 1024, char *host = NULL, const char *ssl_ca_pem = NULL);
    virtual int send(char *buffer, uint32_t buffer_size);
    virtual int recv(SDT_EVENT_CB cb);
    virtual int disconnect();
};

#endif