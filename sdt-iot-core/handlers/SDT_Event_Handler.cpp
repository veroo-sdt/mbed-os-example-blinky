#include "SDT_Event_Handler.h"
#include <typeinfo>

SDT_Event_Handler::SDT_Event_Handler()
{
    _classname = "SDT_Event_Handler";
}

SDT_Event_Handler::~SDT_Event_Handler()
{
    _events.clear();
}

void SDT_Event_Handler::add_event_listener(SDT_EVENT_TYPE type, SDT_EVENT_CB cb)
{
    _mutex.lock();
    _events.insert(make_pair(type, cb));
    _mutex.unlock();
}

void SDT_Event_Handler::remove_event_listener(SDT_EVENT_TYPE type)
{
    _mutex.lock();
    _events.erase(type);
    _mutex.unlock();
}

int SDT_Event_Handler::send_value(SDT_EVENT_TYPE type, SDT_Event_Value_t info)
{
    send_event(type, &info);
    return info.code;
}

int SDT_Event_Handler::send_stack(SDT_EVENT_TYPE type, SDT_Event_Stack_t stack)
{
    send_event(type, &stack);
    return stack.code;
}

void SDT_Event_Handler::send_event(SDT_EVENT_TYPE type, void *event)
{
    _mutex.lock();
    auto search = _events.find(type);
    if (search != _events.end())
    {
        SDT_EVENT_CB cb = search->second;
        if(cb != NULL)
        {
            cb({type, _classname, event});
        }
    }
    _mutex.unlock();
}

void SDT_Event_Handler::send_event(SDT_Event_t event)
{
    _mutex.lock();
    auto search = _events.find(event.type);
    if (search != _events.end())
    {
        SDT_EVENT_CB cb = search->second;
        if(cb != NULL)
        {
            cb(event);
        }
    }
    _mutex.unlock();
}