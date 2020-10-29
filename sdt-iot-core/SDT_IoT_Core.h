#ifndef __SDT_IOT_CORE_H__
#define __SDT_IOT_CORE_H__

#include "mbed.h"
#include "SDT_Event_Handler.h"
// #include "communication/HTTP/SDT_HTTP_Client.h"
// #include "communication/HTTP/SDT_HTTPS_Client.h"

class SDT_IoT_Core : public SDT_Event_Handler
{
private:
    // SDT_HTTP_Client* _httpClient;
    // SDT_HTTPS_Client* _httpsClient;

    bool init_authentication();
    bool init_cloud();
    bool init_time();
    bool init_device_condition();
    // void receive(const char *message, uint32_t length, int status);
    void update(SDT_Event_t info);
    void error(SDT_Event_t info);
    void info(SDT_Event_t info);

public:
    ~SDT_IoT_Core();
    static SDT_IoT_Core *get_instance();
    bool init();
    void* get_network_interface(int id);
};

#endif