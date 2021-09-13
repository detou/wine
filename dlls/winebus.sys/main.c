/*
 * WINE Platform native bus driver
 *
 * Copyright 2016 Aric Stewart
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
#include "config.h"
#include <stdarg.h>

#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "winternl.h"
#include "winioctl.h"
#include "hidusage.h"
#include "ddk/wdm.h"
#include "ddk/hidport.h"
#include "ddk/hidtypes.h"
#include "wine/asm.h"
#include "wine/debug.h"
#include "wine/unicode.h"
#include "wine/list.h"
#include "wine/unixlib.h"

#include "bus.h"
#include "unixlib.h"

WINE_DEFAULT_DEBUG_CHANNEL(plugplay);
WINE_DECLARE_DEBUG_CHANNEL(hid_report);

#if defined(__i386__) && !defined(_WIN32)

extern void * WINAPI wrap_fastcall_func1( void *func, const void *a );
__ASM_STDCALL_FUNC( wrap_fastcall_func1, 8,
                   "popl %ecx\n\t"
                   "popl %eax\n\t"
                   "xchgl (%esp),%ecx\n\t"
                   "jmp *%eax" );

#define call_fastcall_func1(func,a) wrap_fastcall_func1(func,a)

#else

#define call_fastcall_func1(func,a) func(a)

#endif

struct product_desc
{
    WORD vid;
    WORD pid;
};

#define VID_MICROSOFT 0x045e

static const struct product_desc XBOX_CONTROLLERS[] =
{
    {VID_MICROSOFT, 0x0202}, /* Xbox Controller */
    {VID_MICROSOFT, 0x0285}, /* Xbox Controller S */
    {VID_MICROSOFT, 0x0289}, /* Xbox Controller S */
    {VID_MICROSOFT, 0x028e}, /* Xbox360 Controller */
    {VID_MICROSOFT, 0x028f}, /* Xbox360 Wireless Controller */
    {VID_MICROSOFT, 0x02d1}, /* Xbox One Controller */
    {VID_MICROSOFT, 0x02dd}, /* Xbox One Controller (Covert Forces/Firmware 2015) */
    {VID_MICROSOFT, 0x02e0}, /* Xbox One X Controller */
    {VID_MICROSOFT, 0x02e3}, /* Xbox One Elite Controller */
    {VID_MICROSOFT, 0x02e6}, /* Wireless XBox Controller Dongle */
    {VID_MICROSOFT, 0x02ea}, /* Xbox One S Controller */
    {VID_MICROSOFT, 0x02fd}, /* Xbox One S Controller (Firmware 2017) */
    {VID_MICROSOFT, 0x0719}, /* Xbox 360 Wireless Adapter */
};

static DRIVER_OBJECT *driver_obj;

static DEVICE_OBJECT *mouse_obj;
static DEVICE_OBJECT *keyboard_obj;

/* The root-enumerated device stack. */
static DEVICE_OBJECT *bus_pdo;
static DEVICE_OBJECT *bus_fdo;

HANDLE driver_key;

enum device_state
{
    DEVICE_STATE_STOPPED,
    DEVICE_STATE_STARTED,
    DEVICE_STATE_REMOVED,
};

struct device_extension
{
    struct list entry;
    DEVICE_OBJECT *device;

    CRITICAL_SECTION cs;
    enum device_state state;

    struct device_desc desc;
    DWORD index;

    WCHAR manufacturer[MAX_PATH];
    WCHAR product[MAX_PATH];
    WCHAR serialnumber[MAX_PATH];

    BYTE *last_report;
    DWORD last_report_size;
    BOOL last_report_read;
    DWORD buffer_size;
    LIST_ENTRY irp_queue;

    struct unix_device *unix_device;
};

static CRITICAL_SECTION device_list_cs;
static CRITICAL_SECTION_DEBUG critsect_debug =
{
    0, 0, &device_list_cs,
    { &critsect_debug.ProcessLocksList, &critsect_debug.ProcessLocksList },
      0, 0, { (DWORD_PTR)(__FILE__ ": device_list_cs") }
};
static CRITICAL_SECTION device_list_cs = { &critsect_debug, -1, 0, 0, 0, 0 };

static struct list device_list = LIST_INIT(device_list);

static NTSTATUS winebus_call(unsigned int code, void *args)
{
    return __wine_unix_call_funcs[code]( args );
}

static void unix_device_remove(DEVICE_OBJECT *device)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    winebus_call(device_remove, ext->unix_device);
}

static int unix_device_compare(DEVICE_OBJECT *device, void *context)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    struct device_compare_params params =
    {
        .iface = ext->unix_device,
        .context = context
    };
    return winebus_call(device_compare, &params);
}

static NTSTATUS unix_device_start(DEVICE_OBJECT *device)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    struct device_start_params params =
    {
        .iface = ext->unix_device,
        .device = device
    };
    return winebus_call(device_start, &params);
}

static NTSTATUS unix_device_get_report_descriptor(DEVICE_OBJECT *device, BYTE *buffer, DWORD length, DWORD *out_length)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    struct device_descriptor_params params =
    {
        .iface = ext->unix_device,
        .buffer = buffer,
        .length = length,
        .out_length = out_length
    };
    return winebus_call(device_get_report_descriptor, &params);
}

static NTSTATUS unix_device_get_string(DEVICE_OBJECT *device, DWORD index, WCHAR *buffer, DWORD length)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    struct device_string_params params =
    {
        .iface = ext->unix_device,
        .index = index,
        .buffer = buffer,
        .length = length,
    };
    return winebus_call(device_get_string, &params);
}

static void unix_device_set_output_report(DEVICE_OBJECT *device, HID_XFER_PACKET *packet, IO_STATUS_BLOCK *io)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    struct device_report_params params =
    {
        .iface = ext->unix_device,
        .packet = packet,
        .io = io,
    };
    winebus_call(device_set_output_report, &params);
}

static void unix_device_get_feature_report(DEVICE_OBJECT *device, HID_XFER_PACKET *packet, IO_STATUS_BLOCK *io)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    struct device_report_params params =
    {
        .iface = ext->unix_device,
        .packet = packet,
        .io = io,
    };
    winebus_call(device_get_feature_report, &params);
}

static void unix_device_set_feature_report(DEVICE_OBJECT *device, HID_XFER_PACKET *packet, IO_STATUS_BLOCK *io)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    struct device_report_params params =
    {
        .iface = ext->unix_device,
        .packet = packet,
        .io = io,
    };
    winebus_call(device_set_feature_report, &params);
}

struct unix_device *get_unix_device(DEVICE_OBJECT *device)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    return ext->unix_device;
}

static DWORD get_device_index(struct device_desc *desc)
{
    struct device_extension *ext;
    DWORD index = 0;

    LIST_FOR_EACH_ENTRY(ext, &device_list, struct device_extension, entry)
    {
        if (ext->desc.vid == desc->vid && ext->desc.pid == desc->pid && ext->desc.input == desc->input)
            index = max(ext->index + 1, index);
    }

    return index;
}

static WCHAR *get_instance_id(DEVICE_OBJECT *device)
{
    static const WCHAR formatW[] =  {'%','i','&','%','s','&','%','x','&','%','i',0};
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    DWORD len = strlenW(ext->serialnumber) + 33;
    WCHAR *dst;

    if ((dst = ExAllocatePool(PagedPool, len * sizeof(WCHAR))))
        sprintfW(dst, formatW, ext->desc.version, ext->serialnumber, ext->desc.uid, ext->index);

    return dst;
}

static WCHAR *get_device_id(DEVICE_OBJECT *device)
{
    static const WCHAR input_formatW[] = {'&','M','I','_','%','0','2','u',0};
    static const WCHAR formatW[] = {'%','s','\\','v','i','d','_','%','0','4','x',
            '&','p','i','d','_','%','0','4','x',0};
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    DWORD len = strlenW(ext->desc.busid) + 34;
    WCHAR *dst, *tmp;

    if ((dst = ExAllocatePool(PagedPool, len * sizeof(WCHAR))))
    {
        tmp = dst + sprintfW(dst, formatW, ext->desc.busid, ext->desc.vid, ext->desc.pid);
        if (ext->desc.input != -1) sprintfW(tmp, input_formatW, ext->desc.input);
    }

    return dst;
}

static WCHAR *get_hardware_ids(DEVICE_OBJECT *device)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    WCHAR *dst;

    if ((dst = ExAllocatePool(PagedPool, (strlenW(ext->desc.busid) + 2) * sizeof(WCHAR))))
    {
        strcpyW(dst, ext->desc.busid);
        dst[strlenW(dst) + 1] = 0;
    }

    return dst;
}

static WCHAR *get_compatible_ids(DEVICE_OBJECT *device)
{
    static const WCHAR xinput_compat[] =
    {
        'W','I','N','E','B','U','S','\\','W','I','N','E','_','C','O','M','P','_','X','I','N','P','U','T',0
    };
    static const WCHAR hid_compat[] =
    {
        'W','I','N','E','B','U','S','\\','W','I','N','E','_','C','O','M','P','_','H','I','D',0
    };
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    DWORD size = sizeof(hid_compat);
    WCHAR *dst;

    if (ext->desc.is_gamepad) size += sizeof(xinput_compat);

    if ((dst = ExAllocatePool(PagedPool, size + sizeof(WCHAR))))
    {
        if (ext->desc.is_gamepad) memcpy(dst, xinput_compat, sizeof(xinput_compat));
        memcpy((char *)dst + size - sizeof(hid_compat), hid_compat, sizeof(hid_compat));
        dst[size / sizeof(WCHAR)] = 0;
    }

    return dst;
}

static void remove_pending_irps(DEVICE_OBJECT *device)
{
    struct device_extension *ext = device->DeviceExtension;
    LIST_ENTRY *entry;

    while ((entry = RemoveHeadList(&ext->irp_queue)) != &ext->irp_queue)
    {
        IRP *queued_irp = CONTAINING_RECORD(entry, IRP, Tail.Overlay.ListEntry);
        queued_irp->IoStatus.Status = STATUS_DELETE_PENDING;
        queued_irp->IoStatus.Information = 0;
        IoCompleteRequest(queued_irp, IO_NO_INCREMENT);
    }
}

static DEVICE_OBJECT *bus_create_hid_device(struct device_desc *desc, struct unix_device *unix_device)
{
    static const WCHAR device_name_fmtW[] = {'\\','D','e','v','i','c','e','\\','%','s','#','%','p',0};
    struct device_extension *ext;
    DEVICE_OBJECT *device;
    UNICODE_STRING nameW;
    WCHAR dev_name[256];
    NTSTATUS status;

    TRACE("desc %s, unix_device %p\n", debugstr_device_desc(desc), unix_device);

    sprintfW(dev_name, device_name_fmtW, desc->busid, unix_device);
    RtlInitUnicodeString(&nameW, dev_name);
    status = IoCreateDevice(driver_obj, sizeof(struct device_extension), &nameW, 0, 0, FALSE, &device);
    if (status)
    {
        FIXME("failed to create device error %x\n", status);
        return NULL;
    }

    EnterCriticalSection(&device_list_cs);

    /* fill out device_extension struct */
    ext = (struct device_extension *)device->DeviceExtension;
    ext->device             = device;
    ext->desc               = *desc;
    ext->index              = get_device_index(desc);
    ext->last_report        = NULL;
    ext->last_report_size   = 0;
    ext->last_report_read   = TRUE;
    ext->buffer_size        = 0;
    ext->unix_device        = unix_device;

    MultiByteToWideChar(CP_UNIXCP, 0, ext->desc.manufacturer, -1, ext->manufacturer, MAX_PATH);
    MultiByteToWideChar(CP_UNIXCP, 0, ext->desc.product, -1, ext->product, MAX_PATH);
    MultiByteToWideChar(CP_UNIXCP, 0, ext->desc.serialnumber, -1, ext->serialnumber, MAX_PATH);

    InitializeListHead(&ext->irp_queue);
    InitializeCriticalSection(&ext->cs);
    ext->cs.DebugInfo->Spare[0] = (DWORD_PTR)(__FILE__ ": cs");

    /* add to list of pnp devices */
    list_add_tail(&device_list, &ext->entry);

    LeaveCriticalSection(&device_list_cs);
    return device;
}

DEVICE_OBJECT *bus_find_hid_device(const WCHAR *bus_id, void *platform_dev)
{
    struct device_extension *ext;
    DEVICE_OBJECT *ret = NULL;

    TRACE("bus_id %s, platform_dev %p\n", debugstr_w(bus_id), platform_dev);

    EnterCriticalSection(&device_list_cs);
    LIST_FOR_EACH_ENTRY(ext, &device_list, struct device_extension, entry)
    {
        if (strcmpW(ext->desc.busid, bus_id)) continue;
        if (unix_device_compare(ext->device, platform_dev) == 0)
        {
            ret = ext->device;
            break;
        }
    }
    LeaveCriticalSection(&device_list_cs);

    TRACE("returning %p\n", ret);
    return ret;
}

static void bus_unlink_hid_device(DEVICE_OBJECT *device)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;

    EnterCriticalSection(&device_list_cs);
    list_remove(&ext->entry);
    LeaveCriticalSection(&device_list_cs);
}

static NTSTATUS build_device_relations(DEVICE_RELATIONS **devices)
{
    struct device_extension *ext;
    int i;

    EnterCriticalSection(&device_list_cs);
    *devices = ExAllocatePool(PagedPool, offsetof(DEVICE_RELATIONS, Objects[list_count(&device_list)]));

    if (!*devices)
    {
        LeaveCriticalSection(&device_list_cs);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    i = 0;
    LIST_FOR_EACH_ENTRY(ext, &device_list, struct device_extension, entry)
    {
        (*devices)->Objects[i] = ext->device;
        call_fastcall_func1(ObfReferenceObject, ext->device);
        i++;
    }
    LeaveCriticalSection(&device_list_cs);
    (*devices)->Count = i;
    return STATUS_SUCCESS;
}

static DWORD check_bus_option(const UNICODE_STRING *option, DWORD default_value)
{
    char buffer[FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data[sizeof(DWORD)])];
    KEY_VALUE_PARTIAL_INFORMATION *info = (KEY_VALUE_PARTIAL_INFORMATION *)buffer;
    DWORD size;

    if (NtQueryValueKey(driver_key, option, KeyValuePartialInformation, info, sizeof(buffer), &size) == STATUS_SUCCESS)
    {
        if (info->Type == REG_DWORD) return *(DWORD *)info->Data;
    }

    return default_value;
}

static NTSTATUS handle_IRP_MN_QUERY_DEVICE_RELATIONS(IRP *irp)
{
    NTSTATUS status = irp->IoStatus.Status;
    IO_STACK_LOCATION *irpsp = IoGetCurrentIrpStackLocation( irp );

    TRACE("IRP_MN_QUERY_DEVICE_RELATIONS\n");
    switch (irpsp->Parameters.QueryDeviceRelations.Type)
    {
        case EjectionRelations:
        case RemovalRelations:
        case TargetDeviceRelation:
        case PowerRelations:
            FIXME("Unhandled Device Relation %x\n",irpsp->Parameters.QueryDeviceRelations.Type);
            break;
        case BusRelations:
            status = build_device_relations((DEVICE_RELATIONS**)&irp->IoStatus.Information);
            break;
        default:
            FIXME("Unknown Device Relation %x\n",irpsp->Parameters.QueryDeviceRelations.Type);
            break;
    }

    return status;
}

static NTSTATUS handle_IRP_MN_QUERY_ID(DEVICE_OBJECT *device, IRP *irp)
{
    NTSTATUS status = irp->IoStatus.Status;
    IO_STACK_LOCATION *irpsp = IoGetCurrentIrpStackLocation( irp );
    BUS_QUERY_ID_TYPE type = irpsp->Parameters.QueryId.IdType;

    TRACE("(%p, %p)\n", device, irp);

    switch (type)
    {
        case BusQueryHardwareIDs:
            TRACE("BusQueryHardwareIDs\n");
            irp->IoStatus.Information = (ULONG_PTR)get_hardware_ids(device);
            break;
        case BusQueryCompatibleIDs:
            TRACE("BusQueryCompatibleIDs\n");
            irp->IoStatus.Information = (ULONG_PTR)get_compatible_ids(device);
            break;
        case BusQueryDeviceID:
            TRACE("BusQueryDeviceID\n");
            irp->IoStatus.Information = (ULONG_PTR)get_device_id(device);
            break;
        case BusQueryInstanceID:
            TRACE("BusQueryInstanceID\n");
            irp->IoStatus.Information = (ULONG_PTR)get_instance_id(device);
            break;
        default:
            FIXME("Unhandled type %08x\n", type);
            return status;
    }

    status = irp->IoStatus.Information ? STATUS_SUCCESS : STATUS_NO_MEMORY;
    return status;
}

static void mouse_device_create(void)
{
    struct device_create_params params = {{0}};

    if (winebus_call(mouse_create, &params)) return;
    mouse_obj = bus_create_hid_device(&params.desc, params.device);
    IoInvalidateDeviceRelations(bus_pdo, BusRelations);
}

static void keyboard_device_create(void)
{
    struct device_create_params params = {{0}};

    if (winebus_call(keyboard_create, &params)) return;
    keyboard_obj = bus_create_hid_device(&params.desc, params.device);
    IoInvalidateDeviceRelations(bus_pdo, BusRelations);
}

static DWORD bus_count;
static HANDLE bus_thread[16];

struct bus_main_params
{
    const WCHAR *name;

    void *init_args;
    HANDLE init_done;
    unsigned int init_code;
    unsigned int wait_code;
    struct bus_event *bus_event;
};

static DWORD CALLBACK bus_main_thread(void *args)
{
    struct bus_main_params bus = *(struct bus_main_params *)args;
    DEVICE_OBJECT *device;
    NTSTATUS status;

    TRACE("%s main loop starting\n", debugstr_w(bus.name));
    status = winebus_call(bus.init_code, bus.init_args);
    SetEvent(bus.init_done);
    TRACE("%s main loop started\n", debugstr_w(bus.name));

    bus.bus_event->type = BUS_EVENT_TYPE_NONE;
    if (status) WARN("%s bus init returned status %#x\n", debugstr_w(bus.name), status);
    else while ((status = winebus_call(bus.wait_code, bus.bus_event)) == STATUS_PENDING)
    {
        struct bus_event *event = bus.bus_event;
        switch (event->type)
        {
        case BUS_EVENT_TYPE_NONE: break;
        case BUS_EVENT_TYPE_DEVICE_REMOVED:
            EnterCriticalSection(&device_list_cs);
            if (!(device = bus_find_hid_device(event->device_removed.bus_id, event->device_removed.context)))
                WARN("could not find removed device matching bus %s, context %p\n",
                     debugstr_w(event->device_removed.bus_id), event->device_removed.context);
            else
                bus_unlink_hid_device(device);
            LeaveCriticalSection(&device_list_cs);
            IoInvalidateDeviceRelations(bus_pdo, BusRelations);
            break;
        case BUS_EVENT_TYPE_DEVICE_CREATED:
            device = bus_create_hid_device(&event->device_created.desc, event->device_created.device);
            if (device) IoInvalidateDeviceRelations(bus_pdo, BusRelations);
            else
            {
                WARN("failed to create device for %s bus device %p\n",
                     debugstr_w(bus.name), event->device_created.device);
                winebus_call(device_remove, event->device_created.device);
            }
            break;
        }
    }

    if (status) WARN("%s bus wait returned status %#x\n", debugstr_w(bus.name), status);
    else TRACE("%s main loop exited\n", debugstr_w(bus.name));
    HeapFree(GetProcessHeap(), 0, bus.bus_event);
    return status;
}

static NTSTATUS bus_main_thread_start(struct bus_main_params *bus)
{
    DWORD i = bus_count++;

    if (!(bus->init_done = CreateEventW(NULL, FALSE, FALSE, NULL)))
    {
        ERR("failed to create %s bus init done event.\n", debugstr_w(bus->name));
        bus_count--;
        return STATUS_UNSUCCESSFUL;
    }

    if (!(bus->bus_event = HeapAlloc(GetProcessHeap(), 0, sizeof(struct bus_event))))
    {
        ERR("failed to allocate %s bus event.\n", debugstr_w(bus->name));
        CloseHandle(bus->init_done);
        bus_count--;
        return STATUS_UNSUCCESSFUL;
    }

    if (!(bus_thread[i] = CreateThread(NULL, 0, bus_main_thread, bus, 0, NULL)))
    {
        ERR("failed to create %s bus thread.\n", debugstr_w(bus->name));
        CloseHandle(bus->init_done);
        bus_count--;
        return STATUS_UNSUCCESSFUL;
    }

    WaitForSingleObject(bus->init_done, INFINITE);
    CloseHandle(bus->init_done);
    return STATUS_SUCCESS;
}

static NTSTATUS sdl_driver_init(void)
{
    static const WCHAR bus_name[] = {'S','D','L',0};
    static const WCHAR controller_modeW[] = {'M','a','p',' ','C','o','n','t','r','o','l','l','e','r','s',0};
    static const UNICODE_STRING controller_mode = {sizeof(controller_modeW) - sizeof(WCHAR), sizeof(controller_modeW), (WCHAR*)controller_modeW};
    struct sdl_bus_options bus_options;
    struct bus_main_params bus =
    {
        .name = bus_name,
        .init_args = &bus_options,
        .init_code = sdl_init,
        .wait_code = sdl_wait,
    };

    bus_options.map_controllers = check_bus_option(&controller_mode, 1);
    if (!bus_options.map_controllers) TRACE("SDL controller to XInput HID gamepad mapping disabled\n");

    return bus_main_thread_start(&bus);
}

static NTSTATUS udev_driver_init(void)
{
    static const WCHAR bus_name[] = {'U','D','E','V',0};
    static const WCHAR hidraw_disabledW[] = {'D','i','s','a','b','l','e','H','i','d','r','a','w',0};
    static const UNICODE_STRING hidraw_disabled = {sizeof(hidraw_disabledW) - sizeof(WCHAR), sizeof(hidraw_disabledW), (WCHAR*)hidraw_disabledW};
    static const WCHAR input_disabledW[] = {'D','i','s','a','b','l','e','I','n','p','u','t',0};
    static const UNICODE_STRING input_disabled = {sizeof(input_disabledW) - sizeof(WCHAR), sizeof(input_disabledW), (WCHAR*)input_disabledW};
    struct udev_bus_options bus_options;
    struct bus_main_params bus =
    {
        .name = bus_name,
        .init_args = &bus_options,
        .init_code = udev_init,
        .wait_code = udev_wait,
    };

    bus_options.disable_hidraw = check_bus_option(&hidraw_disabled, 0);
    if (bus_options.disable_hidraw) TRACE("UDEV hidraw devices disabled in registry\n");
    bus_options.disable_input = check_bus_option(&input_disabled, 0);
    if (bus_options.disable_input) TRACE("UDEV input devices disabled in registry\n");

    return bus_main_thread_start(&bus);
}

static NTSTATUS iohid_driver_init(void)
{
    static const WCHAR bus_name[] = {'I','O','H','I','D'};
    struct iohid_bus_options bus_options;
    struct bus_main_params bus =
    {
        .name = bus_name,
        .init_args = &bus_options,
        .init_code = iohid_init,
        .wait_code = iohid_wait,
    };

    return bus_main_thread_start(&bus);
}

static NTSTATUS fdo_pnp_dispatch(DEVICE_OBJECT *device, IRP *irp)
{
    static const WCHAR SDL_enabledW[] = {'E','n','a','b','l','e',' ','S','D','L',0};
    static const UNICODE_STRING SDL_enabled = {sizeof(SDL_enabledW) - sizeof(WCHAR), sizeof(SDL_enabledW), (WCHAR*)SDL_enabledW};
    IO_STACK_LOCATION *irpsp = IoGetCurrentIrpStackLocation(irp);
    NTSTATUS ret;

    switch (irpsp->MinorFunction)
    {
    case IRP_MN_QUERY_DEVICE_RELATIONS:
        irp->IoStatus.Status = handle_IRP_MN_QUERY_DEVICE_RELATIONS(irp);
        break;
    case IRP_MN_START_DEVICE:
        mouse_device_create();
        keyboard_device_create();

        if (!check_bus_option(&SDL_enabled, 1) || sdl_driver_init())
        {
            udev_driver_init();
            iohid_driver_init();
        }

        irp->IoStatus.Status = STATUS_SUCCESS;
        break;
    case IRP_MN_SURPRISE_REMOVAL:
        irp->IoStatus.Status = STATUS_SUCCESS;
        break;
    case IRP_MN_REMOVE_DEVICE:
        winebus_call(sdl_stop, NULL);
        winebus_call(udev_stop, NULL);
        winebus_call(iohid_stop, NULL);

        WaitForMultipleObjects(bus_count, bus_thread, TRUE, INFINITE);
        while (bus_count--) CloseHandle(bus_thread[bus_count]);

        irp->IoStatus.Status = STATUS_SUCCESS;
        IoSkipCurrentIrpStackLocation(irp);
        ret = IoCallDriver(bus_pdo, irp);
        IoDetachDevice(bus_pdo);
        IoDeleteDevice(device);
        return ret;
    default:
        FIXME("Unhandled minor function %#x.\n", irpsp->MinorFunction);
    }

    IoSkipCurrentIrpStackLocation(irp);
    return IoCallDriver(bus_pdo, irp);
}

static NTSTATUS pdo_pnp_dispatch(DEVICE_OBJECT *device, IRP *irp)
{
    struct device_extension *ext = device->DeviceExtension;
    NTSTATUS status = irp->IoStatus.Status;
    IO_STACK_LOCATION *irpsp = IoGetCurrentIrpStackLocation(irp);

    TRACE("device %p, irp %p, minor function %#x.\n", device, irp, irpsp->MinorFunction);

    switch (irpsp->MinorFunction)
    {
        case IRP_MN_QUERY_ID:
            status = handle_IRP_MN_QUERY_ID(device, irp);
            break;

        case IRP_MN_QUERY_CAPABILITIES:
            status = STATUS_SUCCESS;
            break;

        case IRP_MN_START_DEVICE:
            EnterCriticalSection(&ext->cs);
            if (ext->state != DEVICE_STATE_STOPPED) status = STATUS_SUCCESS;
            else if (ext->state == DEVICE_STATE_REMOVED) status = STATUS_DELETE_PENDING;
            else if (!(status = unix_device_start(device))) ext->state = DEVICE_STATE_STARTED;
            else ERR("failed to start device %p, status %#x\n", device, status);
            LeaveCriticalSection(&ext->cs);
            break;

        case IRP_MN_SURPRISE_REMOVAL:
            EnterCriticalSection(&ext->cs);
            remove_pending_irps(device);
            ext->state = DEVICE_STATE_REMOVED;
            LeaveCriticalSection(&ext->cs);
            status = STATUS_SUCCESS;
            break;

        case IRP_MN_REMOVE_DEVICE:
            remove_pending_irps(device);

            bus_unlink_hid_device(device);
            unix_device_remove(device);

            ext->cs.DebugInfo->Spare[0] = 0;
            DeleteCriticalSection(&ext->cs);

            HeapFree(GetProcessHeap(), 0, ext->last_report);

            irp->IoStatus.Status = STATUS_SUCCESS;
            IoCompleteRequest(irp, IO_NO_INCREMENT);

            IoDeleteDevice(device);
            return STATUS_SUCCESS;

        default:
            FIXME("Unhandled function %08x\n", irpsp->MinorFunction);
            /* fall through */

        case IRP_MN_QUERY_DEVICE_RELATIONS:
            break;
    }

    irp->IoStatus.Status = status;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return status;
}

static NTSTATUS WINAPI common_pnp_dispatch(DEVICE_OBJECT *device, IRP *irp)
{
    if (device == bus_fdo)
        return fdo_pnp_dispatch(device, irp);
    return pdo_pnp_dispatch(device, irp);
}

static NTSTATUS deliver_last_report(struct device_extension *ext, DWORD buffer_length, BYTE* buffer, ULONG_PTR *out_length)
{
    if (buffer_length < ext->last_report_size)
    {
        *out_length = 0;
        return STATUS_BUFFER_TOO_SMALL;
    }
    else
    {
        if (ext->last_report)
            memcpy(buffer, ext->last_report, ext->last_report_size);
        *out_length = ext->last_report_size;
        return STATUS_SUCCESS;
    }
}

static NTSTATUS hid_get_device_string(DEVICE_OBJECT *device, DWORD index, WCHAR *buffer, DWORD buffer_len)
{
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    DWORD len;

    switch (index)
    {
    case HID_STRING_ID_IMANUFACTURER:
        len = (strlenW(ext->manufacturer) + 1) * sizeof(WCHAR);
        if (len > buffer_len) return STATUS_BUFFER_TOO_SMALL;
        else memcpy(buffer, ext->manufacturer, len);
        return STATUS_SUCCESS;
    case HID_STRING_ID_IPRODUCT:
        len = (strlenW(ext->product) + 1) * sizeof(WCHAR);
        if (len > buffer_len) return STATUS_BUFFER_TOO_SMALL;
        else memcpy(buffer, ext->product, len);
        return STATUS_SUCCESS;
    case HID_STRING_ID_ISERIALNUMBER:
        len = (strlenW(ext->serialnumber) + 1) * sizeof(WCHAR);
        if (len > buffer_len) return STATUS_BUFFER_TOO_SMALL;
        else memcpy(buffer, ext->serialnumber, len);
        return STATUS_SUCCESS;
    }

    return STATUS_NOT_IMPLEMENTED;
}

static NTSTATUS WINAPI hid_internal_dispatch(DEVICE_OBJECT *device, IRP *irp)
{
    IO_STACK_LOCATION *irpsp = IoGetCurrentIrpStackLocation(irp);
    struct device_extension *ext = (struct device_extension *)device->DeviceExtension;
    ULONG code, buffer_len = irpsp->Parameters.DeviceIoControl.OutputBufferLength;
    NTSTATUS status;

    TRACE("(%p, %p)\n", device, irp);

    if (device == bus_fdo)
    {
        IoSkipCurrentIrpStackLocation(irp);
        return IoCallDriver(bus_pdo, irp);
    }

    EnterCriticalSection(&ext->cs);

    if (ext->state == DEVICE_STATE_REMOVED)
    {
        LeaveCriticalSection(&ext->cs);
        irp->IoStatus.Status = STATUS_DELETE_PENDING;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
        return STATUS_DELETE_PENDING;
    }

    switch ((code = irpsp->Parameters.DeviceIoControl.IoControlCode))
    {
        case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
        {
            HID_DEVICE_ATTRIBUTES *attr = (HID_DEVICE_ATTRIBUTES *)irp->UserBuffer;
            TRACE("IOCTL_HID_GET_DEVICE_ATTRIBUTES\n");

            if (buffer_len < sizeof(*attr))
            {
                irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            memset(attr, 0, sizeof(*attr));
            attr->Size = sizeof(*attr);
            attr->VendorID = ext->desc.vid;
            attr->ProductID = ext->desc.pid;
            attr->VersionNumber = ext->desc.version;

            irp->IoStatus.Status = STATUS_SUCCESS;
            irp->IoStatus.Information = sizeof(*attr);
            break;
        }
        case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
        {
            HID_DESCRIPTOR *descriptor = (HID_DESCRIPTOR *)irp->UserBuffer;
            DWORD length;
            TRACE("IOCTL_HID_GET_DEVICE_DESCRIPTOR\n");

            if (buffer_len < sizeof(*descriptor))
            {
                irp->IoStatus.Status = STATUS_BUFFER_TOO_SMALL;
                break;
            }

            irp->IoStatus.Status = unix_device_get_report_descriptor(device, NULL, 0, &length);
            if (irp->IoStatus.Status != STATUS_SUCCESS &&
                irp->IoStatus.Status != STATUS_BUFFER_TOO_SMALL)
            {
                WARN("Failed to get platform report descriptor length\n");
                break;
            }

            memset(descriptor, 0, sizeof(*descriptor));
            descriptor->bLength = sizeof(*descriptor);
            descriptor->bDescriptorType = HID_HID_DESCRIPTOR_TYPE;
            descriptor->bcdHID = HID_REVISION;
            descriptor->bCountry = 0;
            descriptor->bNumDescriptors = 1;
            descriptor->DescriptorList[0].bReportType = HID_REPORT_DESCRIPTOR_TYPE;
            descriptor->DescriptorList[0].wReportLength = length;

            irp->IoStatus.Status = STATUS_SUCCESS;
            irp->IoStatus.Information = sizeof(*descriptor);
            break;
        }
        case IOCTL_HID_GET_REPORT_DESCRIPTOR:
            TRACE("IOCTL_HID_GET_REPORT_DESCRIPTOR\n");
            irp->IoStatus.Status = unix_device_get_report_descriptor(device, irp->UserBuffer, buffer_len, &buffer_len);
            irp->IoStatus.Information = buffer_len;
            break;
        case IOCTL_HID_GET_STRING:
        {
            DWORD index = (ULONG_PTR)irpsp->Parameters.DeviceIoControl.Type3InputBuffer;
            TRACE("IOCTL_HID_GET_STRING[%08x]\n", index);

            irp->IoStatus.Status = hid_get_device_string(device, index, (WCHAR *)irp->UserBuffer, buffer_len);
            if (irp->IoStatus.Status != STATUS_SUCCESS)
                irp->IoStatus.Status = unix_device_get_string(device, index, (WCHAR *)irp->UserBuffer, buffer_len / sizeof(WCHAR));
            if (irp->IoStatus.Status == STATUS_SUCCESS)
                irp->IoStatus.Information = (strlenW((WCHAR *)irp->UserBuffer) + 1) * sizeof(WCHAR);
            break;
        }
        case IOCTL_HID_GET_INPUT_REPORT:
        {
            HID_XFER_PACKET *packet = (HID_XFER_PACKET*)(irp->UserBuffer);
            TRACE_(hid_report)("IOCTL_HID_GET_INPUT_REPORT\n");
            irp->IoStatus.Status = deliver_last_report(ext,
                packet->reportBufferLen, packet->reportBuffer,
                &irp->IoStatus.Information);

            if (irp->IoStatus.Status == STATUS_SUCCESS)
                packet->reportBufferLen = irp->IoStatus.Information;
            break;
        }
        case IOCTL_HID_READ_REPORT:
        {
            TRACE_(hid_report)("IOCTL_HID_READ_REPORT\n");
            if (!ext->last_report_read)
            {
                irp->IoStatus.Status = deliver_last_report(ext,
                    buffer_len, irp->UserBuffer, &irp->IoStatus.Information);
                ext->last_report_read = TRUE;
            }
            else
            {
                InsertTailList(&ext->irp_queue, &irp->Tail.Overlay.ListEntry);
                irp->IoStatus.Status = STATUS_PENDING;
            }
            break;
        }
        case IOCTL_HID_SET_OUTPUT_REPORT:
        case IOCTL_HID_WRITE_REPORT:
        {
            HID_XFER_PACKET *packet = (HID_XFER_PACKET*)(irp->UserBuffer);
            TRACE_(hid_report)("IOCTL_HID_WRITE_REPORT / IOCTL_HID_SET_OUTPUT_REPORT\n");
            unix_device_set_output_report(device, packet, &irp->IoStatus);
            break;
        }
        case IOCTL_HID_GET_FEATURE:
        {
            HID_XFER_PACKET *packet = (HID_XFER_PACKET*)(irp->UserBuffer);
            TRACE_(hid_report)("IOCTL_HID_GET_FEATURE\n");
            unix_device_get_feature_report(device, packet, &irp->IoStatus);
            break;
        }
        case IOCTL_HID_SET_FEATURE:
        {
            HID_XFER_PACKET *packet = (HID_XFER_PACKET*)(irp->UserBuffer);
            TRACE_(hid_report)("IOCTL_HID_SET_FEATURE\n");
            unix_device_set_feature_report(device, packet, &irp->IoStatus);
            break;
        }
        default:
            FIXME("Unsupported ioctl %x (device=%x access=%x func=%x method=%x)\n",
                  code, code >> 16, (code >> 14) & 3, (code >> 2) & 0xfff, code & 3);
            break;
    }

    status = irp->IoStatus.Status;
    LeaveCriticalSection(&ext->cs);

    if (status != STATUS_PENDING) IoCompleteRequest(irp, IO_NO_INCREMENT);
    return status;
}

void process_hid_report(DEVICE_OBJECT *device, BYTE *report, DWORD length)
{
    struct device_extension *ext = (struct device_extension*)device->DeviceExtension;
    IRP *irp;
    LIST_ENTRY *entry;

    if (!length || !report)
        return;

    EnterCriticalSection(&ext->cs);
    if (length > ext->buffer_size)
    {
        HeapFree(GetProcessHeap(), 0, ext->last_report);
        ext->last_report = HeapAlloc(GetProcessHeap(), 0, length);
        if (!ext->last_report)
        {
            ERR_(hid_report)("Failed to alloc last report\n");
            ext->buffer_size = 0;
            ext->last_report_size = 0;
            ext->last_report_read = TRUE;
            LeaveCriticalSection(&ext->cs);
            return;
        }
        else
            ext->buffer_size = length;
    }

    memcpy(ext->last_report, report, length);
    ext->last_report_size = length;
    ext->last_report_read = FALSE;

    while ((entry = RemoveHeadList(&ext->irp_queue)) != &ext->irp_queue)
    {
        IO_STACK_LOCATION *irpsp;
        TRACE_(hid_report)("Processing Request\n");
        irp = CONTAINING_RECORD(entry, IRP, Tail.Overlay.ListEntry);
        irpsp = IoGetCurrentIrpStackLocation(irp);
        irp->IoStatus.Status = deliver_last_report(ext,
            irpsp->Parameters.DeviceIoControl.OutputBufferLength,
            irp->UserBuffer, &irp->IoStatus.Information);
        ext->last_report_read = TRUE;
        IoCompleteRequest(irp, IO_NO_INCREMENT);
    }
    LeaveCriticalSection(&ext->cs);
}

BOOL is_xbox_gamepad(WORD vid, WORD pid)
{
    int i;

    if (vid != VID_MICROSOFT)
        return FALSE;

    for (i = 0; i < ARRAY_SIZE(XBOX_CONTROLLERS); i++)
        if (pid == XBOX_CONTROLLERS[i].pid) return TRUE;

    return FALSE;
}

static NTSTATUS WINAPI driver_add_device(DRIVER_OBJECT *driver, DEVICE_OBJECT *pdo)
{
    NTSTATUS ret;

    TRACE("driver %p, pdo %p.\n", driver, pdo);

    if ((ret = IoCreateDevice(driver, 0, NULL, FILE_DEVICE_BUS_EXTENDER, 0, FALSE, &bus_fdo)))
    {
        ERR("Failed to create FDO, status %#x.\n", ret);
        return ret;
    }

    IoAttachDeviceToDeviceStack(bus_fdo, pdo);
    bus_pdo = pdo;

    bus_fdo->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;
}

static void WINAPI driver_unload(DRIVER_OBJECT *driver)
{
    NtClose(driver_key);
}

NTSTATUS WINAPI DriverEntry( DRIVER_OBJECT *driver, UNICODE_STRING *path )
{
    OBJECT_ATTRIBUTES attr = {0};
    NTSTATUS ret;

    TRACE( "(%p, %s)\n", driver, debugstr_w(path->Buffer) );

    attr.Length = sizeof(attr);
    attr.ObjectName = path;
    attr.Attributes = OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE;
    if ((ret = NtOpenKey(&driver_key, KEY_ALL_ACCESS, &attr)) != STATUS_SUCCESS)
        ERR("Failed to open driver key, status %#x.\n", ret);

    driver_obj = driver;

    driver->MajorFunction[IRP_MJ_PNP] = common_pnp_dispatch;
    driver->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = hid_internal_dispatch;
    driver->DriverExtension->AddDevice = driver_add_device;
    driver->DriverUnload = driver_unload;

    return STATUS_SUCCESS;
}
