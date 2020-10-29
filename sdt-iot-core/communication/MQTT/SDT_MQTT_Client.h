#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include "MQTTTypes.h"

namespace SDT_MQTT
{
    class Client
    {
    private:
        /* data */
        void *_network;
        MQTTVersion _version;
        const char *_host;
        uint16_t _port;
        const char *_username;
        const char *_password;

    public:
        Client(void *network, MQTTVersion version, const char *host, uint16_t port, const char *username, const char *password);
        Client(void *network, MQTTVersion version, const char *host, const char *username, const char *password);
        Client(void *network, const char *host, uint16_t port, const char *username, const char *password);
        Client(void *network, const char *host, const char *username, const char *password);
        ~Client();

        bool is_connected();
        bool publish(char *topic, MQTTQoS qos, char *message, MQTT_PUBLISH_CB cb, bool is_async = false);
        bool add_subscribe(char *topic, MQTTQoS qos, MQTT_SUBSCRIBE_CB cb);
        bool remove_subscribe(char *topic);
        bool disconnect();
    };
} // namespace MQTT

#endif
