#include "SDT_MQTT_Client.h"
#include "SDT_IoT_Core_Device_Network.h"

#define TRACE_GROUP "SDT_MQTT_Client"

namespace SDT_MQTT
{
    Client::Client(void *network, MQTTVersion version, const char *host, uint16_t port, const char *username, const char *password)
        : _network(network),
          _version(version),
          _host(host),
          _port(port),
          _username(username),
          _password(password)
    {
    }
    Client::Client(void *network, MQTTVersion version, const char *host, const char *username, const char *password)
        : Client(network, version, host, 1883, username, password)
    {
    }

    Client::Client(void *network, const char *host, uint16_t port, const char *username, const char *password)
        : Client(network, SDT_MQTT::MQTT_VER_3_1_1, host, port, username, password)
    {
    }

    Client::Client(void *network, const char *host, const char *username, const char *password)
        : Client(network, SDT_MQTT::MQTT_VER_3_1_1, host, 1883, username, password)
    {
    }

    Client::~Client()
    {
        if (_username != NULL)
        {
            delete _username;
            _username = NULL;
        }

        if (_password != NULL)
        {
            delete _password;
            _password = NULL;
        }

        disconnect();
    }

    bool Client::is_connected()
    {
        if (_network == NULL)
        {
            return false;
        }
        SDT_IoT_Core_Device_Network *network = static_cast<SDT_IoT_Core_Device_Network *>(_network);
        return network->is_connected();
    }

    bool Client::publish(char *topic, MQTTQoS qos, char *message, MQTT_PUBLISH_CB cb, bool is_async)
    {
        if (!is_connected())
        {
            return false;
        }
        return true;
    }

    bool Client::add_subscribe(char *topic, MQTTQoS qos, MQTT_SUBSCRIBE_CB cb)
    {
        if (!is_connected())
        {
            return false;
        }
        return true;
    }

    bool Client::remove_subscribe(char *topic)
    {
        return true;
    }

    bool Client::disconnect()
    {
        return true;
    }

} // namespace MQTT