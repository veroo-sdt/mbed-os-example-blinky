#include "SDT_IoT_Core_Device_Network.h"
#include "EthernetInterface.h"
#include "WiFiInterface.h"

SDT_IoT_Core_Device_Network::SDT_IoT_Core_Device_Network(SDT_Network_Info_t info)
    : SDT_Event_Handler(),
      _network(NULL),
      _info(info)
{
    SEND_EVENT_INFO(0, "Device network create");
    //    connect();
}

SDT_IoT_Core_Device_Network::~SDT_IoT_Core_Device_Network()
{
    disconnect();
}

int SDT_IoT_Core_Device_Network::connect()
{
    //send_stack(SDT_EVENT_INFO, {__FUNCTION__, __LINE__, 0, "Network connecting"});
    SEND_EVENT_INFO(0, "Network connecting");

    int error_code = 0;
    // static 정보는 Connect 이전에 이루어 져야 함.
    //tr_info("Set dhcp : %d", network->set_dhcp(false));
    //tr_info("Set static ip : %d", network->set_network(SocketAddress("173.225.12.100"), SocketAddress("255.255.255.0"), SocketAddress("0.0.0.0")));
    if (_info.type == 1)
    {
        // EthInterface *net = EthInterface::get_default_instance();
        // error_code = net->connect();
        EthernetInterface *net = new EthernetInterface();
        error_code = net->connect();
        if (error_code != 0)
        {
            // return send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, error_code, "connect() Error"});
            return SEND_EVENT_ERROR(error_code, "connect() Error")
        }
        _network = (NetworkInterface *)net;
    }
    else if (_info.type == 2)
    {
        WiFiInterface *net = WiFiInterface::get_default_instance();
        //NetworkInterface *net = NetworkInterface::get_default_instance();
        if (!net)
        {
            return SEND_EVENT_ERROR(-1, "Instance not found");
        }

        error_code = net->connect(_info.wifi.ssid, _info.wifi.password, _info.wifi.security);
        if (error_code != 0)
        {
            // return send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, error_code, "connect() Error"});
            return SEND_EVENT_ERROR(error_code, "connect() Error");
        }

        _network = (NetworkInterface *)net;
    }
    else if (_info.type == 3)
    {
        //MeshInterface mesh;
    }
    else if (_info.type == 4)
    {
        // CellularInterface cell;
    }
    else
    {
        // return send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, NSAPI_ERROR_UNSUPPORTED, "Not supported network interface"});
        return SEND_EVENT_ERROR(NSAPI_ERROR_UNSUPPORTED, "Not supported network interface");
    }
    send_value(SDT_EVENT_CONNECT, {0, "Network connection success"});
    // return send_stack(SDT_EVENT_INFO, {__FUNCTION__, __LINE__, 0, "Network connection success"});
    return SEND_EVENT_INFO(0, "Network connection success");
}

bool SDT_IoT_Core_Device_Network::is_connected()
{
    return true;
}

bool SDT_IoT_Core_Device_Network::disconnect()
{
    if (_network != NULL)
    {
        int error_code = _network->disconnect();
        if (error_code != 0)
        {
            //send_stack(SDT_EVENT_ERROR, {__FUNCTION__, __LINE__, NSAPI_ERROR_UNSUPPORTED, "disconnect() fail"});
            SEND_EVENT_ERROR(error_code, "disconnect() fail");
            return false;
        }
        else
        {
            delete _network;
            _network = NULL;
        }
    }

    send_value(SDT_EVENT_DISCONNECT, {0, "Network disconnection success"});
    // send_stack(SDT_EVENT_INFO, {__FUNCTION__, __LINE__, 0, "Network disconnection success"});
    SEND_EVENT_INFO(0, "Network disconnection success");

    return true;
}

NetworkInterface *SDT_IoT_Core_Device_Network::get_network()
{
    return _network;
}

int SDT_IoT_Core_Device_Network::get_id()
{
    return _info.id;
}

int SDT_IoT_Core_Device_Network::get_type()
{
    return _info.type;
}

int SDT_IoT_Core_Device_Network::get_status()
{
    return _network->get_connection_status();
}

const char *SDT_IoT_Core_Device_Network::get_mac_address()
{
    return _network->get_mac_address() ? _network->get_mac_address() : "None";
}

const char *SDT_IoT_Core_Device_Network::get_gateway_address()
{
    SocketAddress socket;
    _network->get_gateway(&socket);
    return socket.get_ip_address() ? socket.get_ip_address() : "None";
}

const char *SDT_IoT_Core_Device_Network::get_ip_address()
{
    SocketAddress socket;
    _network->get_ip_address(&socket);
    return socket.get_ip_address() ? socket.get_ip_address() : "None";
}

const char *SDT_IoT_Core_Device_Network::get_netmask()
{
    SocketAddress socket;
    _network->get_netmask(&socket);
    return socket.get_ip_address() ? socket.get_ip_address() : "None";
}

int SDT_IoT_Core_Device_Network::get_bandwidth()
{
    return 0;
}

void SDT_IoT_Core_Device_Network::get_info_json(SDT_Device_Info_Condition_t info)
{
    //tr_info("%d", get_status());
    // char *buffer = (char *)malloc(1024);
    char buffer[512];
    memset(buffer, 0, 512);

    // // sprintf(buffer, "{\"id\":%d", get_id());
    // // sprintf(buffer, "%s,\"type\":%d", buffer, get_type());
    // // sprintf(buffer, "%s,\"mac\":\"%s\"", buffer, get_mac_address());
    // // sprintf(buffer, "%s,\"gateway\":\"%s\"", buffer, get_gateway_address());
    // // sprintf(buffer, "%s,\"netmask\":\"%s\"", buffer, get_netmask());
    // // sprintf(buffer, "%s,\"ip\":\"%s\"}", buffer, get_ip_address());
    sprintf(buffer, "{\"id\":%d,\"type\":%d,\"mac\":\"%s\",\"gateway\":\"%s\",\"netmask\":\"%s\",\"ip\":\"%s\"}",
            get_id(), get_type(), get_mac_address(), get_gateway_address(), get_netmask(), get_ip_address());

    send_value(SDT_EVENT_UPDATE, {info.type, buffer});
}

void SDT_IoT_Core_Device_Network::get_bps(SDT_Network_Traffic_Usage_t *usage)
{
    if (usage == NULL)
    {
        return;
    }
    usage->in = 0;
    usage->out = 0;
}

void SDT_IoT_Core_Device_Network::get_bps_json(char *buffer)
{
    SDT_Network_Traffic_Usage_t usage;
    get_bps(&usage);

    // char *buffer = (char *)malloc(512);
    if (buffer == NULL)
    {
        buffer = new char[128];
        memset(buffer, 0, 128);
    }
    // sprintf(buffer, "{\"in\":%ld", usage.in);
    // sprintf(buffer, "%s,\"out\":%ld}", buffer, usage.out);
    sprintf(buffer, "{\"in\":%ld,\"out\":%ld}", usage.in, usage.out);
}

void SDT_IoT_Core_Device_Network::get_pps(SDT_Network_Traffic_Usage_t *usage)
{
    if (usage == NULL)
    {
        return;
    }
    usage->in = 0;
    usage->out = 0;
}

void SDT_IoT_Core_Device_Network::get_pps_json(char *buffer)
{
    SDT_Network_Traffic_Usage_t usage;
    get_pps(&usage);

    // char *buffer = (char *)malloc(512);
    if (buffer == NULL)
    {
        buffer = new char[128];
        memset(buffer, 0, 128);
    }
    // sprintf(buffer, "{\"in\":%ld", usage.in);
    // sprintf(buffer, "%s,\"out\":%ld}", buffer, usage.out);
    sprintf(buffer, "{\"in\":%ld,\"out\":%ld}", usage.in, usage.out);
}

void SDT_IoT_Core_Device_Network::get_usage_json(SDT_Device_Info_Condition_t info)
{
    char buffer[256];
    memset(buffer, 0, 256);

    char data[128];
    memset(data, 0, 128);
    get_bps_json(data);
    sprintf(buffer, "{\"id\":%d,\"bps\":%s", get_id(), data);

    memset(data, 0, 128);
    get_pps_json(data);
    sprintf(buffer, "%s,\"pps\":%s}", buffer, data);

    send_value(SDT_EVENT_UPDATE, {info.type, buffer});
}