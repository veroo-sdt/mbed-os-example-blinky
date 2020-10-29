#ifndef __SDT_IOT_CORE_DEVICE_SPEC_H__
#define __SDT_IOT_CORE_DEVICE_SPEC_H__

#include "mbed.h"
#include "SDT_Types.h"
#include "SDT_Event_Handler.h"

typedef struct {
    uint8_t idle;         /**< Time spent in the idle thread since the system has started */
    uint8_t usage;        /**< Time spent in sleep since the system has started */
    int time;
} sdt_cpu_usages_t;

class SDT_IoT_Core_Device_Spec : public SDT_Event_Handler
{
private:
    uint64_t _prev_idle_time = 0;
    
public:
    // SDT_IoT_Core_Device_Spec(/* args */);
    ~SDT_IoT_Core_Device_Spec();

    static SDT_IoT_Core_Device_Spec *get_instance();
    bool init();
    const char *get_target_name();
    void get_stats(mbed_stats_sys_t *stats);
    const char *get_som_version();
    const char *get_iot_core_version();
    const char *get_modules_info();

    void get_cpu_usage(sdt_cpu_usages_t *usage);
    void get_cpu_usage_json(SDT_Device_Info_Condition_t info);

    void get_stack_memory(mbed_stats_stack_t *stats);
    void get_stack_memory_json(char *buffer);
    void get_heap_memory(mbed_stats_heap_t *stats);
    void get_heap_memory_json(char *buffer);
    void get_memory_usage_json(SDT_Device_Info_Condition_t info);

    void get_info_json(SDT_Device_Info_Condition_t info);
};

#endif