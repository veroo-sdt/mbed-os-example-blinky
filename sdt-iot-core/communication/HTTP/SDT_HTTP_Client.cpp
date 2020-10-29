#include "SDT_HTTP_Client.h"
#include "http_request_builder.h"
#include "http_request_parser.h"

SDT_HTTP_Client::SDT_HTTP_Client()
    : SDT_Communication(),
      _state(SDT_HTTP_PARSE_STATE_START),
      _response(NULL),
      _recv_offset(0)
{
    _classname = "SDT_HTTP_Client";
}

SDT_HTTP_Client::~SDT_HTTP_Client()
{
    disconnect();
}

SDT_HTTP_Client::SDT_HTTP_Response_t *SDT_HTTP_Client::get(const char *url, const char *ssl_ca_pem)
{
    return send_http(HTTP_GET, url, NULL, ssl_ca_pem);
}

SDT_HTTP_Client::SDT_HTTP_Response_t *SDT_HTTP_Client::post(const char *url, const char *body, const char *ssl_ca_pem)
{
    return send_http(HTTP_POST, url, body, ssl_ca_pem);
}

SDT_HTTP_Client::SDT_HTTP_Response_t *SDT_HTTP_Client::send_http(int method, const char *url, const char *body, const char *ssl_ca_pem)
{
    ParsedUrl *parsed_url = new ParsedUrl(url);
    http_method a_method = HTTP_GET;
    if (method == 3)
    {
        a_method = HTTP_POST;
    }
    HttpRequestBuilder *request_builder = new HttpRequestBuilder(a_method, parsed_url);
    //request_builder->set_header("Transfer-Encoding", "chunked");
    if (method == 3)
    {
        request_builder->set_header("Content-Type", "application/json");
    }
    // request_builder->set_header("Connection", "close");
    uint32_t request_size = 0;
    unsigned int body_length = body != NULL ? strlen(body) : 0;
    char *request = request_builder->build(body, body_length, request_size);
    int rc = connect(parsed_url->host(), parsed_url->port(), 256, ssl_ca_pem);

    SDT_HTTP_Client::SDT_HTTP_Response_t *response = NULL;
    if (rc != 0)
    {
        SEND_EVENT_ERROR(rc, "Connect error");
    }
    else
    {
        if (send(request, request_size) > 0)
        {
            response = recv();
        }
        else
        {
            SEND_EVENT_ERROR(rc, "Send data size is 0");
        }
    }
    delete parsed_url;
    delete request_builder;
    delete request;
    disconnect();
    return response;
}

SDT_HTTP_Client::SDT_HTTP_Response_t *SDT_HTTP_Client::recv()
{
    if (!_is_connected)
    {
        SEND_EVENT_ERROR(-3005, "Socket not available for use");
        return NULL;
    }
    _state = SDT_HTTP_PARSE_STATE_START;
    unsigned int remain_size = 0;
    // memset(_recv_buffer, 0, _buffer_size);
    while (_is_connected)
    {
        memset(_recv_buffer + remain_size, 0, _buffer_size - remain_size);
        _recv_size = _socket->recv(_recv_buffer + remain_size, _buffer_size - remain_size);
        // memset(_recv_buffer, 0, _buffer_size);
        // _recv_size = _socket->recv(_recv_buffer, _buffer_size);
        // printf("RECV - %d, Remain - %d\n", _recv_size, remain_size);
        printf("\n=================\n|%s|\n================\n", _recv_buffer);
        if (_recv_size <= 0)
        {
            break;
        }
        _recv_offset = 0;
        if (parse_response(_recv_buffer, _recv_size + remain_size))
        {
            break;
        }

        remain_size = (_recv_size + remain_size) - _recv_offset;
        if(_recv_offset == 0)
        {
            if(remain_size == _buffer_size)
            {
                _buffer_size += _buffer_size;
                _recv_buffer = (unsigned char *)realloc(_recv_buffer, _buffer_size);
            }
        }
        else 
        {
            if(remain_size > 0)
            {
                memcpy(_recv_buffer, _recv_buffer + _recv_offset, remain_size);
            }
            else
            {
                remain_size = 0;
            }
        }

        // remain_size = (_recv_size + remain_size) - _recv_offset;
        // if (remain_size > 0)
        // {
        //     if(remain_size > _buffer_size)
        //     {
        //         _recv_buffer = (unsigned char *)realloc(_recv_buffer, remain_size);
        //         _buffer_size = remain_size;
        //     }
        //     if(_recv_offset != 0)
        //     {
        //         memcpy(_recv_buffer, _recv_buffer + _recv_offset, remain_size);
        //         memset(_recv_buffer + remain_size, 0, _buffer_size - _recv_offset);
        //     }
        // }
        // else
        // {
        //     remain_size = 0;
        //     memset(_recv_buffer, 0, _buffer_size);
        // }
    }

    if (_recv_size < 0)
    {
        SEND_EVENT_ERROR(_recv_size, "Receive error");
        return NULL;
    }

    return _response;
}

void SDT_HTTP_Client::parse_response_async()
{
}

bool SDT_HTTP_Client::parse_response(unsigned char *buffer, unsigned int length)
{
    unsigned int recv_check_offset = 0;
    while (_state != SDT_HTTP_PARSE_STATE_END)
    {
        // printf("[%d] %d, %d, %d\n", _state, length, _recv_offset, recv_check_offset);
        switch (_state)
        {
        case SDT_HTTP_PARSE_STATE_START:
            _response = new SDT_HTTP_Client::SDT_HTTP_Response_t; //[];//(SDT_HTTP_Client::SDT_HTTP_Response_t *)malloc(sizeof(SDT_HTTP_Client::SDT_HTTP_Response_t));
            // _recv_size = 0;
            _recv_offset = 0;
            _state = SDT_HTTP_PARSE_STATE_HTTP;
            break;
        case SDT_HTTP_PARSE_STATE_HTTP:
            recv_check_offset += get_http(buffer, length - _recv_offset);
            break;
        case SDT_HTTP_PARSE_STATE_HTTP_VERSION:
            recv_check_offset += get_version(buffer, length - _recv_offset);
            break;
        case SDT_HTTP_PARSE_STATE_CODE:
            recv_check_offset += get_code(buffer, length - _recv_offset);
            break;
        case SDT_HTTP_PARSE_STATE_REASON:
            recv_check_offset += get_reason(buffer, length - _recv_offset);
            break;
        case SDT_HTTP_PARSE_STATE_HEADER:
            recv_check_offset += get_header(buffer, length - _recv_offset);
            break;
        case SDT_HTTP_PARSE_STATE_BODY:
            recv_check_offset += get_body(buffer, length - _recv_offset);
            break;
        default:
            break;
        }

        if (length <= recv_check_offset)
        {
            break;
        }
    }

    return _state == SDT_HTTP_PARSE_STATE_END;
}

unsigned int SDT_HTTP_Client::get_http(unsigned char *buffer, unsigned int length)
{
    if (_state != SDT_HTTP_PARSE_STATE_HTTP)
    {
        return length;
    }
    unsigned int index = 0;
    for (index = 0; index < length; index++)
    {
        if (buffer[_recv_offset + index] != '/')
        {
            continue;
        }
        _response->type = get_string(_state, buffer, _recv_offset, index);
        _recv_offset += ++index;
        _state = SDT_HTTP_PARSE_STATE_HTTP_VERSION;
        break;
    }
    return index;
}

unsigned int SDT_HTTP_Client::get_version(unsigned char *buffer, unsigned int length)
{
    if (_state != SDT_HTTP_PARSE_STATE_HTTP_VERSION)
    {
        return 0;
    }

    unsigned int index = 0;
    for (index = 0; index < length; index++)
    {
        if (buffer[_recv_offset + index] == ' ' || buffer[_recv_offset + index] == '\t')
        {
            _response->version = get_string(_state, buffer, _recv_offset, index);
            _recv_offset += ++index;
            _state = SDT_HTTP_PARSE_STATE_CODE;
            break;
        }
    }

    return index;
}

unsigned int SDT_HTTP_Client::get_code(unsigned char *buffer, unsigned int length)
{
    if (_state != SDT_HTTP_PARSE_STATE_CODE)
    {
        return length;
    }

    unsigned int index = 0;
    for (index = 0; index < length; index++)
    {
        if (buffer[_recv_offset + index] == ' ' || buffer[_recv_offset + index] == '\t')
        {
            char* tmp = get_string(_state, buffer, _recv_offset, index);
            _response->code = atoi(tmp);
            _recv_offset += ++index;
            _state = SDT_HTTP_PARSE_STATE_REASON;
            break;
        }
    }

    return index;
}

unsigned int SDT_HTTP_Client::get_reason(unsigned char *buffer, unsigned int length)
{
    if (_state != SDT_HTTP_PARSE_STATE_REASON)
    {
        return length;
    }

    unsigned int index = 0;
    for (index = 0; index < length; index++)
    {
        if (buffer[_recv_offset + index] == '\r')
        {
            index++;
            if (buffer[_recv_offset + index] == '\n')
            {
                _response->reason = get_string(_state, buffer, _recv_offset, index - 1);
                _recv_offset += ++index;
                _state = SDT_HTTP_PARSE_STATE_HEADER;
                break;
            }
        }
    }

    return index;
}

unsigned int SDT_HTTP_Client::get_header(unsigned char *buffer, unsigned int length)
{
    if (_state != SDT_HTTP_PARSE_STATE_HEADER)
    {
        return length;
    }

    bool is_new_line = true;

    unsigned int start_index = _recv_offset;
    unsigned int offset_key = _recv_offset;
    unsigned int offset_key_size = 0;

    unsigned int offset_value = 0;
    unsigned short header_part = 0;

    unsigned int index = 0;
    unsigned int idx_tmp = 0;
    for (index = 0; index < length; index++)
    {
        idx_tmp = start_index + index;
        if (is_new_line && (buffer[idx_tmp] == '\r' && buffer[idx_tmp + 1] == '\n'))
        {
            index += 2;
            _recv_offset = start_index + index;
            _state = SDT_HTTP_PARSE_STATE_BODY;
            break;
        }
        else
        {
            if (header_part == 0)
            {
                is_new_line = false;
                if (buffer[idx_tmp] == ':')
                {
                    offset_key_size = idx_tmp - offset_key;
                    header_part = 1;
                }
            }
            else if (header_part == 1)
            {
                if (buffer[idx_tmp] != ' ' && buffer[idx_tmp] != '\t')
                {
                    offset_value = idx_tmp;
                    header_part = 2;
                }
            }
            else if (header_part == 2)
            {
                if (buffer[idx_tmp] == '\r')
                {
                    index++;
                    if (buffer[idx_tmp + 1] == '\n')
                    {
                        header_part = 0;
                        is_new_line = true;
                        char *key = get_string(_state, buffer, offset_key, offset_key_size);
                        char *value = get_string(_state, buffer, offset_value, idx_tmp - offset_value);
                        
                        _recv_offset = start_index + index + 1;
                        offset_key = _recv_offset;
                        if (strcmp(key, "Content-Length") == 0)
                        {
                            _response->body_length = atoi(value);
                        }
                    }
                }
            }
        }
    }

    return index;
}

unsigned int SDT_HTTP_Client::get_body(unsigned char *buffer, unsigned int length)
{
    if (length < _response->body_length)
    {
        return length;
    }
    
    _response->body = get_string(_state, buffer, _recv_offset, _response->body_length);
    _recv_offset += _response->body_length;
    _state = SDT_HTTP_PARSE_STATE_END;
    return _response->body_length;
}