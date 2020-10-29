#ifndef __SDT_IOT_CORE_DEVICE_H__
#define __SDT_IOT_CORE_DEVICE_H__

#include "mbed.h"
#include "SDT_Types.h"
#include "map"
#include "list"
#include "SDT_Event_Handler.h"

using namespace std;

class SDT_IoT_Core_Device : public SDT_Event_Handler
{
private:
    map<int, void *> _networks;
    EventQueue *_eventqueue;
    Thread *_thread_eventqueue;

    bool init_networks();
    bool init_event_queue(int size);
    
    void update(SDT_Event_t info);
    void error(SDT_Event_t info);
    void info(SDT_Event_t info);

public:
    // SDT_IoT_Core_Device(/* args */);
    ~SDT_IoT_Core_Device();

    static SDT_IoT_Core_Device *get_instance();
    bool init();
    void* get_network_interface(int id);
    bool add_jobs(list<SDT_Device_Info_Condition_t> infos);
};

#endif