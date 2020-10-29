#ifndef __SDT_TYPES_H__
#define __SDT_TYPES_H__

typedef struct
{
    int type;
    const char *name;
    int duration;
    bool state = true;
} SDT_Device_Info_Condition_t;

enum SDT_DEVICE_INFO_TYPE
{
    SDT_DEVICE_SYS_INFO = 100,
    SDT_DEVICE_SYS_CPU_USAGE_INFO,
    SDT_DEVICE_SYS_MEM_USAGE_INFO,
    SDT_DEVICE_NETWORK_INFO,
    SDT_DEVICE_NETWORK_USAGE_INFO,
};

#endif