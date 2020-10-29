#ifndef __SDT_IOT_CORE_DEVICE_NETWORK_H__
#define __SDT_IOT_CORE_DEVICE_NETWORK_H__

#include "mbed.h"
#include "SDT_Types.h"
#include "SDT_Event_Handler.h"

typedef struct
{
    int id;
    int type;
    int priority;
    const char *ip;
    const char *netmask;
    const char *gateway;
    union
    {
        /* wifi */
        struct
        {
            const char *ssid;
            const char *password;
            nsapi_security_t security;
        } wifi;
    };

} SDT_Network_Info_t;

typedef struct
{
    long in;
    long out;
} SDT_Network_Traffic_Usage_t;

class SDT_IoT_Core_Device_Network : public SDT_Event_Handler
{
private:
    NetworkInterface *_network;
    SDT_Network_Info_t _info;

public:
    SDT_IoT_Core_Device_Network(SDT_Network_Info_t info);
    ~SDT_IoT_Core_Device_Network();

    int connect();
    NetworkInterface *get_network();
    int get_id();
    int get_type();
    bool is_connected();
    int get_status();
    const char *get_mac_address();
    const char *get_gateway_address();
    const char *get_ip_address();
    const char *get_netmask();
    int get_bandwidth();
    void get_info_json(SDT_Device_Info_Condition_t info);

    void get_bps(SDT_Network_Traffic_Usage_t *usage);
    void get_bps_json(char *buffer);
    void get_pps(SDT_Network_Traffic_Usage_t *usage);
    void get_pps_json(char *buffer);
    void get_usage_json(SDT_Device_Info_Condition_t info);

    bool disconnect();
};

#endif