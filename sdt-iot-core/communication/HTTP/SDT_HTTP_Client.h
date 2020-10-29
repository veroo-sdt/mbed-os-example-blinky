#ifndef __SDT_HTTP_H__
#define __SDT_HTTP_H__

#include "SDT_Communication.h"
#include "map"

using namespace std;

class SDT_HTTP_Client : public SDT_Communication
{
public:
    typedef struct
    {
        char *type;
        char *version;
        int code;
        char *reason;
        map<char *, char *> headers;
        char *body;
        unsigned int body_length;
    } SDT_HTTP_Response_t;
private:
    enum SDT_HTTP_PARSE_STATE
    {
        SDT_HTTP_PARSE_STATE_START = 0,
        SDT_HTTP_PARSE_STATE_HTTP,
        SDT_HTTP_PARSE_STATE_HTTP_VERSION,
        SDT_HTTP_PARSE_STATE_CODE,
        SDT_HTTP_PARSE_STATE_REASON,
        SDT_HTTP_PARSE_STATE_HEADER,
        SDT_HTTP_PARSE_STATE_BODY,
        SDT_HTTP_PARSE_STATE_END
    };

private:
    SDT_HTTP_Response_t *recv();
    SDT_HTTP_Response_t *send_http(int method, const char *url, const char *body, const char *ssl_ca_pem = NULL);
    // virtual int disconnect();
    void parse_response_async();
    bool parse_response(unsigned char *buffer, unsigned int length);

    unsigned int get_http(unsigned char *buffer, unsigned int length);
    unsigned int get_version(unsigned char *buffer, unsigned int length);
    unsigned int get_code(unsigned char *buffer, unsigned int length);
    unsigned int get_reason(unsigned char *buffer, unsigned int length);
    unsigned int get_header(unsigned char *buffer, unsigned int length);
    unsigned int get_body(unsigned char *buffer, unsigned int length);

    SDT_HTTP_PARSE_STATE _state;
    SDT_HTTP_Response_t *_response;
    unsigned int _recv_offset;

public:
    SDT_HTTP_Client();
    ~SDT_HTTP_Client();
    SDT_HTTP_Response_t *get(const char *url, const char *ssl_ca_pem = NULL);
    SDT_HTTP_Response_t *post(const char *url, const char *body, const char *ssl_ca_pem = NULL);
};

#endif
