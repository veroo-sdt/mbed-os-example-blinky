#include "SDT_IoT_Core_Device_Spec.h"
#include "inttypes.h"

SDT_IoT_Core_Device_Spec::~SDT_IoT_Core_Device_Spec()
{
}

SDT_IoT_Core_Device_Spec *SDT_IoT_Core_Device_Spec::get_instance()
{
    static SDT_IoT_Core_Device_Spec spec;
    return &spec;
}

bool SDT_IoT_Core_Device_Spec::init()
{
    //send_stack(SDT_EVENT_INFO, {__FUNCTION__, __LINE__, 0, "Complete"});
    SEND_EVENT_INFO(0, "Device spec initialize start");
    SEND_EVENT_INFO(0, "Device spec initialize end");
    return true;
}

const char *SDT_IoT_Core_Device_Spec::get_target_name()
{
    // #ifdef TARGET_NAME
    //     return TARGET_NAME
    // #else
    //     return "Unknown";
    // #endif
    return "Unknown";
}

void SDT_IoT_Core_Device_Spec::get_stats(mbed_stats_sys_t *stats)
{
    mbed_stats_sys_get(stats);
}

const char *SDT_IoT_Core_Device_Spec::get_som_version()
{
    return "SOM_VER_1.0.0.0";
}

const char *SDT_IoT_Core_Device_Spec::get_iot_core_version()
{
    return "CORE_VER_1.0.0.0";
}

const char *SDT_IoT_Core_Device_Spec::get_modules_info()
{
    return "";
}

void SDT_IoT_Core_Device_Spec::get_info_json(SDT_Device_Info_Condition_t info)
{
    mbed_stats_sys_t stats;
    get_stats(&stats);
    //char *sysinfo = (char *)malloc(1024);

    char sysinfo[1024];
    memset(sysinfo, 0, 1024);

    sprintf(sysinfo, "{\"target_name\":\"%s\",\"cpu_id\":%" PRId32 ",\"compiler_id\":%d,\"compiler_version\":%" PRId32 ",\"os_version\":\"%d.%d.%d\",\"som_version\":\"%s\",\"iot_core_version\":\"%s\",\"mem\":{\"ram\":[",
            get_target_name(), stats.cpu_id, stats.compiler_id, stats.compiler_version, MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION, get_som_version(), get_iot_core_version());

    for (size_t i = 0; i < MBED_MAX_MEM_REGIONS; i++)
    {
        if (stats.ram_size[i] == 0)
        {
            continue;
        }
        if (i > 0)
        {
            strcat(sysinfo, ",");
        }

        sprintf(sysinfo, "%s{\"index\":%d,\"start\":%" PRId32 ",\"size\":%" PRId32 "}", sysinfo, i, stats.ram_start[i], stats.ram_size[i]);
    }
    strcat(sysinfo, "],\"rom\":[");
    for (size_t i = 0; i < MBED_MAX_MEM_REGIONS; i++)
    {
        if (stats.rom_size[i] == 0)
        {
            continue;
        }
        if (i > 0)
        {
            strcat(sysinfo, ",");
        }
        sprintf(sysinfo, "%s{\"index\":%d,\"start\":%" PRId32 ",\"size\":%" PRId32 "}", sysinfo, i, stats.rom_start[i], stats.rom_size[i]);
    }
    strcat(sysinfo, "]},\"modules\":[");

    strcat(sysinfo, "]}");

    send_value(SDT_EVENT_UPDATE, {info.type, sysinfo});
}

void SDT_IoT_Core_Device_Spec::get_cpu_usage(sdt_cpu_usages_t *usage)
{
    mbed_stats_cpu_t stats;
    mbed_stats_cpu_get(&stats);

    uint64_t diff_usec = (stats.idle_time - _prev_idle_time);
    usage->idle = (diff_usec * 100) / (usage->time * 1000);
    usage->usage = 100 - ((diff_usec * 100) / (usage->time * 1000));
    _prev_idle_time = stats.idle_time;
}

void SDT_IoT_Core_Device_Spec::get_cpu_usage_json(SDT_Device_Info_Condition_t info)
{
    sdt_cpu_usages_t cpu_info = {100, 0, info.duration};
    get_cpu_usage(&cpu_info);

    char buffer[150];
    memset(buffer, 0, 150);
    sprintf(buffer, "{\"idle\":%" PRId8 ",\"usage\":%" PRId8 "}", cpu_info.idle, cpu_info.usage);

    send_value(SDT_EVENT_UPDATE, {info.type, buffer});
}

void SDT_IoT_Core_Device_Spec::get_stack_memory(mbed_stats_stack_t *stats)
{
    int cnt = osThreadGetCount();
    if (cnt == 0)
    {
        stats = NULL;
        return;
    }

    mbed_stats_stack_get_each(stats, cnt);
}

void SDT_IoT_Core_Device_Spec::get_stack_memory_json(char *buffer)
{
    int cnt = osThreadGetCount();
    if (cnt == 0)
    {
        buffer = NULL;
        return;
    }
    mbed_stats_stack_t stack_info[8];
    mbed_stats_stack_get_each(stack_info, cnt);

    if (buffer == NULL)
    {
        buffer = new char[512];
        memset(buffer, 0, 512);
    }
    strcat(buffer, "[");
    for (int i = 0; i < cnt; i++)
    {
        if (stack_info[i].thread_id == 0)
        {
            continue;
        }
        if (i > 0)
        {
            strcat(buffer, ",");
        }

        sprintf(buffer, "%s{\"id\":%" PRId32 ",\"max_size\":%" PRId32 ",\"allocated\":%" PRId32 ",\"stack_cnt\":%" PRId32 "}",
                buffer, stack_info[i].thread_id, stack_info[i].max_size, stack_info[i].reserved_size, stack_info[i].stack_cnt);
    }
    strcat(buffer, "]");
}

void SDT_IoT_Core_Device_Spec::get_heap_memory(mbed_stats_heap_t *stats)
{
    mbed_stats_heap_get(stats);
}

void SDT_IoT_Core_Device_Spec::get_heap_memory_json(char *buffer)
{
    //mbed_stats_heap_t *heap_info = get_heap_memory();
    mbed_stats_heap_t heap_info;
    mbed_stats_heap_get(&heap_info);

    if (buffer == NULL)
    {
        buffer = new char[512];
        memset(buffer, 0, 512);
    }

    sprintf(buffer, "{\"current_size\":%" PRIu32 ",\"max_size\":%" PRIu32 ",\"total_size\":%" PRIu32 ",\"reserved_size\":%" PRIu32 ",\"alloc_cnt\":%" PRIu32 ",\"alloc_fail_cnt\":%" PRIu32 ",\"overhead_size\":%" PRIu32 "}",
            heap_info.current_size, heap_info.max_size, heap_info.total_size, heap_info.reserved_size, heap_info.alloc_cnt, heap_info.alloc_fail_cnt, heap_info.overhead_size);
}

void SDT_IoT_Core_Device_Spec::get_memory_usage_json(SDT_Device_Info_Condition_t info)
{
    char data[1024];  // = (char *)malloc(512);
    char buffer[512]; // = (char *)malloc(512);
    memset(data, 0, 1024);

    memset(buffer, 0, 512);
    get_stack_memory_json(buffer);
    sprintf(data, "{\"stack\":%s", buffer);

    memset(buffer, 0, 512);
    get_heap_memory_json(buffer);
    sprintf(data, "%s,\"heap\":%s}", data, buffer);

    send_value(SDT_EVENT_UPDATE, {info.type, data});
}