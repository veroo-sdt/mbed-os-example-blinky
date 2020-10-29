// #ifndef __SDT_SOCKET_H__
// #define __SDT_SOCKET_H__

// #include "SDT_IoT_Core_Types.h"
// #include "SDT_Event_Handler.h"

// class SDT_Socket : public SDT_Communication
// {
// private:
//     // void *_network;
//     const char *_host;
//     uint16_t _port;
//     Socket *_socket;
//     bool _is_connected;
//     volatile bool _is_close;
//     SDT_Socket_Protocol_t _type;
//     const char *_ssl_ca_pem;
//     int _buffer_size;
//     uint8_t *_recv_buffer;
//     Thread _recv_thread;

//     void recv_run(SDT_Socket_Message_CB cb);
//     void recv_process(SDT_Socket_Message_CB cb);
//     void recv_buffer(SDT_Socket_Message_CB cb);

// public:
//     SDT_Socket();
//     ~SDT_Socket();

//     // nsapi_error_t connect();
//     nsapi_size_or_error_t send(char *buffer, uint32_t buffer_size);
//     void recv(SDT_Socket_Message_CB cb);
//     // nsapi_error_t disconnected();
// };

// #endif



0123456789
HTTP/1.1 40
6 NOT ACCE
PTABLEDate:
 Thu, 08 Oct
  2020 04:43
  :03 GMTCo
  ntent-Type
  : applicatio
  n/jsonCont
  ent-Length
  : 142Conne
  ction: keep-
  aliveServer:
   gunicorn/
   19.9.0Acce
   ss-Control
   -Allow-Ori
   gin: *Acce
   ss-Control
   -Allow-Cre
   dentials: tr
   ue
