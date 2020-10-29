#include "SDT_IoT_Core.h"
#include "mbed-trace/mbed_trace.h"

#include "SDT_IoT_Core_Device.h"
#include "SDT_JSON.h"
#include "list"

#define TRACE_GROUP "SDT_IoT_Core"
using namespace std;

// SDT_IoT_Core::SDT_IoT_Core(/* args */)
// {
//     LOG_INFO("Creation.");
//     init();
// }
SDT_IoT_Core::~SDT_IoT_Core()
{
    // if (_httpClient != NULL)
    // {
    //     delete _httpClient;
    //     _httpClient = NULL;
    // }

    // if (_httpsClient != NULL)
    // {
    //     delete _httpsClient;
    //     _httpsClient = NULL;
    // }
}

SDT_IoT_Core *SDT_IoT_Core::get_instance()
{
    static SDT_IoT_Core core;
    return &core;
}

bool SDT_IoT_Core::init()
{
    SDT_IoT_Core_Device::get_instance()->add_event_listener(SDT_EVENT_UPDATE, mbed::callback(this, &SDT_IoT_Core::update));
    SDT_IoT_Core_Device::get_instance()->add_event_listener(SDT_EVENT_INFO, mbed::callback(this, &SDT_IoT_Core::info));
    SDT_IoT_Core_Device::get_instance()->add_event_listener(SDT_EVENT_ERROR, mbed::callback(this, &SDT_IoT_Core::error));
    if (!SDT_IoT_Core_Device::get_instance()->init())
    {
        return false;
    }

    init_authentication();
    init_cloud();
    init_time();

    // init_device_condition();

    // _httpClient = new SDT_HTTP_Client(SDT_IoT_Core_Device::get_instance()->get_network_interface(0));
    // _httpsClient = new SDT_HTTPS_Client(SDT_IoT_Core_Device::get_instance()->get_network_interface(0));
    //_httpsClient->set_certification("-----BEGIN CERTIFICATE-----\n");

    return true;
}

bool SDT_IoT_Core::init_authentication()
{
    // 인증서버 서버 접속
    // 인증 서버 접속을 위한 정보는 mbed_app.json 에 작성, 기본적으로 접속 URL(IP), port, id, password, ca, 등등 그때 맞춰서 작성

    // 인증 서버와 접속이 완료된 후, 해당 서버와 통신을 통해 Device가 접속 해야할 Cloud에 대한 정보를 얻어온다.

    // 접속 정보는 1개 이상일 수 있다.
    // Cloud 접속 정보를 리턴 한다.
    return true;
}

bool SDT_IoT_Core::init_cloud()
{
    return true;
}

bool SDT_IoT_Core::init_time()
{
    return true;
}

bool SDT_IoT_Core::init_device_condition()
{
    list<SDT_Device_Info_Condition_t> infos;
    infos.push_back({SDT_DEVICE_SYS_INFO, "Sys info", -1, true});
    infos.push_back({SDT_DEVICE_SYS_CPU_USAGE_INFO, "Sys cpu usage", -1, true});
    infos.push_back({SDT_DEVICE_SYS_MEM_USAGE_INFO, "Sys memory usage", -1, true});
    infos.push_back({SDT_DEVICE_NETWORK_INFO, "Network info", -1, true});
    infos.push_back({SDT_DEVICE_NETWORK_USAGE_INFO, "Network usage", -1, true});

    SDT_IoT_Core_Device::get_instance()->add_jobs(infos);

    return true;
}

void* SDT_IoT_Core::get_network_interface(int id)
{
    return SDT_IoT_Core_Device::get_instance()->get_network_interface(id);
}

void SDT_IoT_Core::update(SDT_Event_t info)
{
    send_event(info);
    // SDT_Event_Value_t *event = (SDT_Event_Value_t *)info.event;
    // tr_info("Code : %d, Message: %s", event->code, event->value);
}

void SDT_IoT_Core::error(SDT_Event_t info)
{
    send_event(info);
    // SDT_Event_Stack_t *event = (SDT_Event_Stack_t *)info.event;
    // tr_error("[%s::%s(%d)] Code : %d, Message: %s", info.classname, event->func, event->line, event->code, event->message);
}

void SDT_IoT_Core::info(SDT_Event_t info)
{
    send_event(info);
    // SDT_Event_Stack_t *event = (SDT_Event_Stack_t *)info.event;
    // tr_info("[%s.%s(%d)] Code : %d, Message: %s", info.classname, event->func, event->line, event->code, event->message);
}
