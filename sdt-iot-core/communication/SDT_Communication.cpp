#include "SDT_Communication.h"

// #include "SocketAddress.h"
// #include "TCPSocket.h"
// #include "TLSSocket.h"
// #include "TLSSocketWrapper.h"

SDT_Communication::SDT_Communication()
    : _network(NULL),
      _socket(NULL),
      _ssl_ca_pem(NULL),
      _is_connected(false),
      _recv_buffer(NULL),
      _recv_size(0),
      _buffer_size(1024),
      _circular_buffer(NULL)
{
}

SDT_Communication::~SDT_Communication()
{
    disconnect();
}

char *SDT_Communication::get_string(int code, unsigned char *buffer, unsigned int start, unsigned int size)
{
    //unsigned int size = end - start + 1;
    char *str = new char[size + 1];
    memset(str, 0, size + 1);
    memcpy(str, buffer + start, size);
    SEND_EVENT_INFO(code, str);
    // printf("%d ~ %d(%d) |%s|\n", start, start + size, size, str);
    return str;
}

int SDT_Communication::set_network_interface(void *network)
{
    if (network == NULL)
    {
        // return send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, -3004, "Not connected to a network"});
        return SEND_EVENT_ERROR(-3004, "Not connected to a network");
    }

    _network = network;
    return 0;
}

int SDT_Communication::connect(char *host, uint16_t port, unsigned int buffer_size, const char *ssl_ca_pem)
{
    if (_network == NULL)
    {
        return SEND_EVENT_ERROR(-3004, "Not connected to a network");
    }

    if (_is_connected)
    {
        return SEND_EVENT_ERROR(-3015, "Socket is already connected");
    }
    SocketAddress address;
    NetworkInterface *networkInterface = static_cast<NetworkInterface *>(_network);
    int rc = networkInterface->gethostbyname(host, &address);
    if (rc != 0)
    {
        return SEND_EVENT_ERROR(rc, "gethostbyname() fail");
    }
    address.set_port(port);

    return connect(address, buffer_size, host, ssl_ca_pem);
}

int SDT_Communication::connect(SocketAddress address, unsigned int buffer_size, char *host, const char *ssl_ca_pem)
{
    if (_network == NULL)
    {
        return SEND_EVENT_ERROR(-3004, "Not connected to a network");
    }

    if (_is_connected)
    {
        return SEND_EVENT_ERROR(-3015, "Socket is already connected");
    }

    if (address == NULL)
    {
        return SEND_EVENT_ERROR(-3006, "IP address is not known");
    }

    int rc = 0;

    _buffer_size = buffer_size;
    _ssl_ca_pem = ssl_ca_pem;
    NetworkInterface *networkInterface = static_cast<NetworkInterface *>(_network);
    if (_ssl_ca_pem == NULL)
    {
        _socket = new TCPSocket();
        rc = ((TCPSocket *)_socket)->open(networkInterface);
    }
    else
    {
        _socket = new TLSSocket;
        rc = ((TLSSocket *)_socket)->open(networkInterface);
        if (rc == 0) {
            ((TLSSocket *)_socket)->set_hostname(host);
            rc = ((TLSSocket *)_socket)->set_root_ca_cert(_ssl_ca_pem);
        }
        // result = socket->set_root_ca_cert(cert);
        // if (result == NSAPI_ERROR_OK) {
        //     printf("Error: socket->set_root_ca_cert() returned %d\n", result);
        //     return result;
        // }

        // _socket = new TLSSocket;
        // rc = ((TLSSocket *)_socket)->open(networkInterface);
        // printf("1\n");
        // if (rc != 0)
        // {
        //     printf("%s\n", _ssl_ca_pem);
        //     rc = ((TLSSocket *)_socket)->set_root_ca_cert(_ssl_ca_pem);
        //     printf("%s\n", host);
        //     ((TLSSocket *)_socket)->set_hostname(host);
        // }
    }

    if (rc != 0)
    {
        delete _socket;
        _socket = NULL;
        return SEND_EVENT_ERROR(rc, "socket.open() fail");
    }

    if (_recv_buffer != NULL)
    {
        delete _recv_buffer;
    }
    _recv_buffer = (unsigned char *)malloc(_buffer_size);

    rc = _socket->connect(address);
    if (rc == 0)
    {
        _is_connected = true;
        send_value(SDT_EVENT_CONNECT, {0, "Connection success"});
        return SEND_EVENT_INFO(rc, "socket.connect() success");
    }
    else
    {
        return SEND_EVENT_INFO(rc, "socket.connect() fail");
    }
}

int SDT_Communication::disconnect()
{
    _is_connected = false;
    
    if (_socket != NULL)
    {
        _socket->close();
        delete _socket;
        _socket = NULL;
    }

    if (_circular_buffer != NULL)
    {
        delete _circular_buffer;
        _circular_buffer = NULL;
    }

    if (_recv_buffer != NULL)
    {
        delete _recv_buffer;
        _recv_buffer = NULL;
    }

    send_value(SDT_EVENT_DISCONNECT, {0, "Disconnection success"});
    return SEND_EVENT_INFO(0, "Disconnection() success");
}

int SDT_Communication::send(char *buffer, uint32_t buffer_size)
{
    if (!_is_connected)
    {
        //return send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, -3005, "Socket not available for use"});
        return SEND_EVENT_ERROR(-3005, "Socket not available for use");
    }
    // printf("[SEND - %d %d] %s\n", buffer_size, strlen(buffer), buffer);
    int send_ret = _socket->send(buffer, buffer_size);
    return send_ret;
}

int SDT_Communication::recv(SDT_EVENT_CB cb)
{
    if (!_is_connected)
    {
        // return send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, -3005, "Socket not available for use"});
        return SEND_EVENT_ERROR(-3005, "Socket not available for use");
    }

    if (_circular_buffer != NULL)
    {
        _circular_buffer = new SDT_Circular_Buffer<unsigned char>(_buffer_size, true);
    }
    int buffer_ret = SDT_CIRCULAR_SUCCESS;
    //SDT_Circular_Buffer<unsigned char> *circular_buffer = new SDT_Circular_Buffer<unsigned char>(_buffer_size, true);
    while (_is_connected)
    {
        memset(_recv_buffer, 0, _buffer_size);
        _recv_size = _socket->recv(_recv_buffer, _buffer_size);
        if (_recv_size <= 0)
        {
            break;
        }

        buffer_ret = _circular_buffer->push(_recv_buffer, 0, _recv_size);
        if (buffer_ret != SDT_CIRCULAR_SUCCESS)
        {
            break;
        }
    }

    if (buffer_ret != SDT_CIRCULAR_SUCCESS)
    {
        // return send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, buffer_ret, "Buffer push error"});
        return SEND_EVENT_ERROR(buffer_ret, "Buffer.push() error");
    }
    else
    {
        unsigned int size = _circular_buffer->get_used();
        unsigned char *recv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * size);
        buffer_ret = _circular_buffer->pop(recv_buffer, 0, size);
        if (buffer_ret == SDT_CIRCULAR_SUCCESS)
        {
            send_value(SDT_EVENT_UPDATE, {0, (const char *)recv_buffer});
        }
        else
        {
            // send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, buffer_ret, "Buffer pop error"});
            SEND_EVENT_ERROR(buffer_ret, "Buffer.Buffer() error");
        }
        delete recv_buffer;
    }
    _circular_buffer->reset();
    return buffer_ret;
}