#include "SDT_IoT_Core_Device.h"
#include "SDT_IoT_Core_Device_Network.h"
#include "SDT_IoT_Core_Device_Spec.h"

#define THREAD_NAME_EVENTQUEUE "SDT_IoT_Core_Device: _thread_eventqueue"
#define SDT_EQUEUE_EVENT_SIZE (sizeof(struct equeue_event) + 2 * sizeof(void *))
#define SDT_EVENT_SIZE (SDT_EQUEUE_EVENT_SIZE - 2 * sizeof(void *) + sizeof(SDT_EVENT_CB))

SDT_IoT_Core_Device::~SDT_IoT_Core_Device()
{
    if (_eventqueue != NULL)
    {
        delete _eventqueue;
        _eventqueue = NULL;
    }
    if (_thread_eventqueue != NULL)
    {
        delete _thread_eventqueue;
        _thread_eventqueue = NULL;
    }
}

SDT_IoT_Core_Device *SDT_IoT_Core_Device::get_instance()
{
    static SDT_IoT_Core_Device device;
    return &device;
}

bool SDT_IoT_Core_Device::init()
{
    SDT_IoT_Core_Device_Spec::get_instance()->add_event_listener(SDT_EVENT_UPDATE, mbed::callback(this, &SDT_IoT_Core_Device::update));
    SDT_IoT_Core_Device_Spec::get_instance()->add_event_listener(SDT_EVENT_ERROR, mbed::callback(this, &SDT_IoT_Core_Device::error));
    SDT_IoT_Core_Device_Spec::get_instance()->add_event_listener(SDT_EVENT_INFO, mbed::callback(this, &SDT_IoT_Core_Device::info));
    SDT_IoT_Core_Device_Spec::get_instance()->init();

    return init_networks();
    //return true;
}

bool SDT_IoT_Core_Device::init_networks()
{
    // mbed_app.json 에 작성된 network 종류에 맞춰서 네트워크를 셋하여 가지고 있는다.
    // network 종류를 추가할때, id, 종류, 접속 정보, 우선순위, 방향 등을 넣는다.
    // network ID는 내부에서 특정 서비스나 특정 cloud에 접속시에 network를 점유하기 위해 필요하다.
    // 해당 정보는
    // int network_count = 0;
    int network_count = 1;
    SDT_Network_Info_t info;
    info.id = 0;
    info.type = 1;
    info.priority = 0;

    info.wifi.ssid = "sigdeltec";
    info.wifi.password = "251327412287";
    // info.wifi.ssid = "12618032G";
    // info.wifi.password = "brdk062019";
    info.wifi.security = NSAPI_SECURITY_WPA_WPA2;

    //send_stack(SDT_EVENT_INFO, {__FUNCTION__, __LINE__, 0, "Network initialize start"});
    
    SEND_EVENT_INFO(0, "Network initialize start");
    for (int i = 0; i < network_count; i++)
    {
        SDT_IoT_Core_Device_Network *network = new SDT_IoT_Core_Device_Network(info);
        network->add_event_listener(SDT_EVENT_UPDATE, mbed::callback(this, &SDT_IoT_Core_Device::update));
        network->add_event_listener(SDT_EVENT_ERROR, mbed::callback(this, &SDT_IoT_Core_Device::error));
        network->add_event_listener(SDT_EVENT_INFO, mbed::callback(this, &SDT_IoT_Core_Device::info));
        if (network->connect() == 0)
        {
            _networks.insert(make_pair(info.id, network));
        }
    }

    // send_stack(SDT_EVENT_INFO, {__FUNCTION__, __LINE__, 0, "Network initialize end"});
    
    SEND_EVENT_INFO(0, "Network initialize end");
    return _networks.size() == (size_t)network_count;
}

bool SDT_IoT_Core_Device::init_event_queue(int size)
{
    size += _networks.size() + 1;
    _eventqueue = new EventQueue(size * SDT_EVENT_SIZE);
    if (_eventqueue == NULL)
    {
        // send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, -1, "Eventqueue is not created"});
        SEND_EVENT_ERROR(-1, "Eventqueue is not created");
        return false;
    }
    /* Thread */
    _thread_eventqueue = new Thread(osPriorityNormal, OS_STACK_SIZE, 0, THREAD_NAME_EVENTQUEUE);
    if (_thread_eventqueue == NULL)
    {
        // send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, -1, "Thread is not created"});
        SEND_EVENT_ERROR(-1, "Thread is not created");
        return false;
    }

    if (_thread_eventqueue)
    {
        _thread_eventqueue->start(callback(_eventqueue, &EventQueue::dispatch_forever));
    }

    return true;
}

void *SDT_IoT_Core_Device::get_network_interface(int id)
{
    auto search = _networks.find(id);
    if (search != _networks.end())
    {
        SDT_IoT_Core_Device_Network *network = static_cast<SDT_IoT_Core_Device_Network *>(search->second);
        return network->get_network(); //->get_network();
    }
    else
    {
        return NULL;
    }
}

bool SDT_IoT_Core_Device::add_jobs(list<SDT_Device_Info_Condition_t> infos)
{
    list<SDT_Device_Info_Condition_t>::iterator iter;
    if (!init_event_queue(infos.size()))
    {
        return false;
    }

    for (iter = infos.begin(); iter != infos.end(); ++iter)
    {
        if (iter->type == SDT_DEVICE_SYS_INFO)
        {
            if (iter->duration > 0)
            {
                _eventqueue->call_every(iter->duration, SDT_IoT_Core_Device_Spec::get_instance(), &SDT_IoT_Core_Device_Spec::get_info_json, *iter);
            }
            else
            {
                _eventqueue->call(SDT_IoT_Core_Device_Spec::get_instance(), &SDT_IoT_Core_Device_Spec::get_info_json, *iter);
            }
        }
        else if (iter->type == SDT_DEVICE_NETWORK_INFO)
        {
            for (auto it = _networks.begin(); it != _networks.end(); it++)
            {
                if (iter->duration > 0)
                {
                    _eventqueue->call_every(iter->duration, (SDT_IoT_Core_Device_Network *)it->second, &SDT_IoT_Core_Device_Network::get_info_json, *iter);
                }
                else
                {
                    _eventqueue->call((SDT_IoT_Core_Device_Network *)it->second, &SDT_IoT_Core_Device_Network::get_info_json, *iter);
                }
            }
        }
        else if (iter->type == SDT_DEVICE_NETWORK_USAGE_INFO)
        {
            for (auto it = _networks.begin(); it != _networks.end(); it++)
            {
                if (iter->duration > 0)
                {
                    _eventqueue->call_every(iter->duration, (SDT_IoT_Core_Device_Network *)it->second, &SDT_IoT_Core_Device_Network::get_usage_json, *iter);
                }
                else
                {
                    _eventqueue->call((SDT_IoT_Core_Device_Network *)it->second, &SDT_IoT_Core_Device_Network::get_usage_json, *iter);
                }
            }
        }
        else if (iter->type == SDT_DEVICE_SYS_MEM_USAGE_INFO)
        {
            if (iter->duration > 0)
            {
                _eventqueue->call_every(iter->duration, SDT_IoT_Core_Device_Spec::get_instance(), &SDT_IoT_Core_Device_Spec::get_memory_usage_json, *iter);
            }
            else
            {
                _eventqueue->call(SDT_IoT_Core_Device_Spec::get_instance(), &SDT_IoT_Core_Device_Spec::get_memory_usage_json, *iter);
            }
        }
        else if (iter->type == SDT_DEVICE_SYS_CPU_USAGE_INFO)
        {
            if (iter->duration > 0)
            {
                _eventqueue->call_every(iter->duration, SDT_IoT_Core_Device_Spec::get_instance(), &SDT_IoT_Core_Device_Spec::get_cpu_usage_json, *iter);
            }
            else
            {
                _eventqueue->call(SDT_IoT_Core_Device_Spec::get_instance(), &SDT_IoT_Core_Device_Spec::get_cpu_usage_json, *iter);
            }
        }
    }

    return true;
}

void SDT_IoT_Core_Device::update(SDT_Event_t info)
{
    send_event(info);
}

void SDT_IoT_Core_Device::error(SDT_Event_t info)
{
    send_event(info);
}

void SDT_IoT_Core_Device::info(SDT_Event_t info)
{
    send_event(info);
}
