#ifndef __SDT_EVENT_HANDLER_H__
#define __SDT_EVENT_HANDLER_H__

#include "mbed.h"
#include "map"

using namespace std;

#define SEND_EVENT_ERROR(code, message) send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, (int)code, message});
#define SEND_EVENT_INFO(code, message) send_stack(SDT_EVENT_INFO, {__FUNCTION__, __LINE__, (int)code, message});


// enum SDT_EVENT_VALUE_TYPE
// {
//     SDT_EVENT_VALUE_CONST_CHAR = 0,
//     SDT_EVENT_VALUE_CHAR,
//     SDT_EVENT_VALUE_INT,
//     SDT_EVENT_VALUE_BYTE
// };

enum SDT_EVENT_TYPE
{
    SDT_EVENT_UPDATE = 0,
    SDT_EVENT_ERROR,
    SDT_EVENT_INFO,
    SDT_EVENT_CONNECT,
    SDT_EVENT_DISCONNECT,
    
    SDT_EVENT_HTTP_SEND_DATA,
    SDT_EVENT_HTTP_RECV_DATA
};

typedef struct
{
    int code;
    const char *value;
} SDT_Event_Value_t;

typedef struct
{
    const char *func;
    unsigned int line;
    int code;
    const char *message;
} SDT_Event_Stack_t;

typedef struct
{
    SDT_EVENT_TYPE type;
    const char *classname;
    void *event;
} SDT_Event_t;

typedef mbed::Callback<void(SDT_Event_t)> SDT_EVENT_CB;

class SDT_Event_Handler
{
private:
    Mutex _mutex;
    map<SDT_EVENT_TYPE, SDT_EVENT_CB> _events;

protected:
    int send_value(SDT_EVENT_TYPE type, SDT_Event_Value_t info);
    int send_stack(SDT_EVENT_TYPE type, SDT_Event_Stack_t error);
    void send_event(SDT_EVENT_TYPE type, void *event);
    void send_event(SDT_Event_t event);
    const char *_classname;
    SDT_Event_Handler(/* args */);
    ~SDT_Event_Handler();

public:
    void add_event_listener(SDT_EVENT_TYPE type, SDT_EVENT_CB cb);
    void remove_event_listener(SDT_EVENT_TYPE type);
};

#endif