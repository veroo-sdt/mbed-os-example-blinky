// #include "SDT_Socket.h"
// #include "TCPSocket.h"
// #include "TLSSocket.h"

// SDT_Socket::SDT_Socket(void *network, char *host, uint16_t port, int buffer_size, SDT_Socket_Protocol_t type, const char *ssl_ca_pem)
//     : _network(network),
//       _host(host),
//       _port(port),
//       _socket(NULL),
//       _is_connected(false),
//       _is_close(false),
//       _type(type),
//       _ssl_ca_pem(ssl_ca_pem),
//       _buffer_size(buffer_size),
//       _recv_buffer(NULL)
// {
//     //connect();
// }

// SDT_Socket::~SDT_Socket()
// {
//     disconnected();
// }

// int SDT_Socket::connect()
// {
//     if (_network == NULL)
//     {
//         return NSAPI_ERROR_NO_CONNECTION;
//     }

//     if (_is_connected)
//     {
//         return NSAPI_ERROR_OK;
//     }

//     SocketAddress address;
//     NetworkInterface *network = static_cast<NetworkInterface *>(_network);
//     nsapi_error_t rc = network->gethostbyname(_host, &address);
//     if (rc != NSAPI_ERROR_OK)
//     {
//         return rc;
//     }
//     address.set_port(_port);
//     if (_type == SDT_SOCKET_PROTOCOL_TCP)
//     {
//         _socket = new TCPSocket();
//         rc = ((TCPSocket *)_socket)->open(network);
//     }
//     else if (_type == SDT_SOCKET_PROTOCOL_TLS)
//     {
//         _socket = new TLSSocket();
//         rc = ((TLSSocket *)_socket)->open(network);
//     }
//     else
//     {
//         return NSAPI_ERROR_UNSUPPORTED;
//     }

//     if (rc != NSAPI_ERROR_OK)
//     {
//         delete _socket;
//         _socket = NULL;
//         return rc;
//     }

//     if (_type == SDT_SOCKET_PROTOCOL_TLS)
//     {
//         rc = ((TLSSocket *)_socket)->set_root_ca_cert(_ssl_ca_pem);
//         if (rc != NSAPI_ERROR_OK)
//         {
//             delete _socket;
//             _socket = NULL;
//             return rc;
//         }
//     }

//     if (_recv_buffer != NULL)
//     {
//         delete _recv_buffer;
//     }
//     _recv_buffer = (uint8_t *)malloc(_buffer_size);

//     return _socket->connect(address);
// }

// nsapi_size_or_error_t SDT_Socket::send(char *buffer, uint32_t buffer_size)
// {
//     nsapi_error_t error = connect();
//     if (error != NSAPI_ERROR_OK)
//     {
//         return error;
//     }

//     return _socket->send(buffer, buffer_size);
// }

// void SDT_Socket::recv(SDT_Socket_Message_CB cb)
// {
//     recv_buffer(cb);
//     //_recv_thread.terminate();
//     //_recv_thread.start(mbed::callback(this, &SDT_Socket::recv_buffer, cb));
//     //_recv_thread.start(callback(&SDT_Socket::recv_buffer, &cb));
// }

// void SDT_Socket::recv_run(SDT_Socket_Message_CB cb)
// {
//     // while (!_is_close)
//     // {
//     //     ThisThread::sleep_for(100);
//     // }
// }

// void SDT_Socket::recv_buffer(SDT_Socket_Message_CB cb)
// {
//     nsapi_size_or_error_t recv_ret;
//     while (true)
//     {
//         recv_ret = _socket->recv(_recv_buffer, _buffer_size);
//         if (recv_ret <= 0)
//         {
//             break;
//         }

//         cb(SDT_SOCKET_RECEIVE_ING, _recv_buffer, recv_ret);
//     }
//     cb(SDT_SOCKET_RECEIVE_END, NULL, 0);
// }

// int SDT_Socket::disconnect()
// {
//     nsapi_error_t rc = NSAPI_ERROR_OK;

//     if (!_is_connected)
//     {
//         return rc;
//     }
//     _is_close = true;
//     _is_connected = false;
//     if (_socket != NULL)
//     {
//         rc = _socket->close();
//         delete _socket;
//         _socket = NULL;
//     }

//     if (_recv_buffer != NULL)
//     {
//         delete _recv_buffer;
//         _recv_buffer = NULL;
//     }

//     _recv_thread.terminate();

//     return rc;
// }