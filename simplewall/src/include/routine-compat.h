// Compatibility layer between current simplewall and the public henrypp/routine SDK.
// Do not include this from routine.c / rapp.c.
#pragma once

#include <mscat.h>

#ifndef PCR_STRINGREF
typedef const R_STRINGREF *PCR_STRINGREF;
#endif

#ifndef PCSID
typedef const SID *PCSID;
#endif

#ifndef PR_SIZE_BUFFER_MINIMUM
#define PR_SIZE_BUFFER_MINIMUM PR_SIZE_BUFFER_OVERFLOW
#endif

#ifndef GENERAL_ID
#define GENERAL_ID 0x53575554U
#endif

#ifndef IN6_IS_ADDR_ULA
#define IN6_IS_ADDR_ULA(address) ((((const BYTE *)(address))[0] & 0xFE) == 0xFC)
#endif

typedef BOOL (WINAPI *CCAHFFH2) (
	HCATADMIN,
	HANDLE,
	DWORD *,
	BYTE *,
	DWORD
);

typedef BOOL (WINAPI *CCAAC2) (
	HCATADMIN *,
	const GUID *,
	LPCWSTR,
	PCCERT_STRONG_SIGN_PARA,
	DWORD
);

#define _r_config_getboolean _r_config_getboolean_ex
#define _r_config_getlong _r_config_getlong_ex
#define _r_config_getlong64 _r_config_getlong64_ex
#define _r_config_getulong _r_config_getulong_ex
#define _r_config_getulong64 _r_config_getulong64_ex
#define _r_config_getfont _r_config_getfont_ex
#define _r_config_getstringexpand _r_config_getstringexpand_ex
#define _r_config_getstring _r_config_getstring_ex
#define _r_config_setboolean _r_config_setboolean_ex
#define _r_config_setlong _r_config_setlong_ex
#define _r_config_setlong64 _r_config_setlong64_ex
#define _r_config_setulong _r_config_setulong_ex
#define _r_config_setulong64 _r_config_setulong64_ex
#define _r_config_setfont _r_config_setfont_ex
#define _r_config_setstringexpand _r_config_setstringexpand_ex
#define _r_config_setstring _r_config_setstring_ex
#define _r_obj_addlistitem _r_obj_addlistitem_ex
#define _r_fs_isexists _r_fs_exists

#define _r_format_interval(seconds) _r_format_interval ((seconds), FALSE)
#define _r_theme_initialize(hwnd) _r_theme_initialize ((hwnd), _r_theme_isenabled ())
#define _r_sys_settimer SetTimer
#define _r_sys_setprocessprivilege(hwnd, process, privileges, count, enable) \
	_r_sys_setprocessprivilege ((process), (privileges), (count), (enable))
#define _r_sys_loadlibrary2(out, name, flags) \
	_r_sys_loadlibrary2 ((LPWSTR)(name), (flags), (PVOID_PTR)(out))
#define _r_res_loadimage(out, module, type, name, format, width, height) \
	_r_res_loadimage ((module), (type), (name), (format), (width), (height), (out))
#define _r_imagelist_create(out, width, height, flags, count, grow) \
	_r_imagelist_create ((width), (height), (flags), (count), (grow), (out))
#define _r_sys_queryprocessstring(out, process, kind) \
	_r_sys_queryprocessstring ((process), (kind), (out))
#define _r_sys_querytokeninformation(out, token, kind) \
	_r_sys_querytokeninformation ((token), (kind), (out))
#define _r_str_fromsid(out, sid) _r_str_fromsid ((PSID)(sid), (out))
#define _r_edit_setselection(hwnd, id, start, end) \
	_r_wnd_sendmessage ((hwnd), (id), EM_SETSEL, (start), (end))
#define _r_obj_initializestringref(ref, string) \
	_r_obj_initializestringref ((ref), (LPWSTR)(string))
#define _r_toolbar_setbutton(hwnd, id, command, text, style, state, image) \
	_r_toolbar_setbutton ((hwnd), (id), (command), (LPWSTR)(text), (style), (state), (image))

static inline HRESULT sw_compat_imagelist_setsize (
	_In_ HIMAGELIST himg,
	_In_ LONG width,
	_In_ LONG height
)
{
	UNREFERENCED_PARAMETER (height);

	return _r_imagelist_setsize (himg, width);
}

#define _r_imagelist_setsize(list, width, height) sw_compat_imagelist_setsize ((list), (width), (height))

static inline INT sw_compat_tab_selectitem (
	_In_ HWND hwnd,
	_In_ INT ctrl_id,
	_In_ INT item_id
)
{
	_r_tab_selectitem (hwnd, ctrl_id, item_id);

	return _r_tab_getcurrentitem (hwnd, ctrl_id);
}

#define _r_tab_selectitem sw_compat_tab_selectitem

static inline BOOLEAN sw_compat_setcontext (
	_In_ HWND hwnd,
	_In_ ULONG property,
	_In_ PVOID value
)
{
	_r_wnd_setcontext (hwnd, property, value);

	return _r_wnd_getcontext (hwnd, property) == value;
}

static inline BOOLEAN sw_compat_removecontext (
	_In_ HWND hwnd,
	_In_ ULONG property
)
{
	_r_wnd_removecontext (hwnd, property);

	return _r_wnd_getcontext (hwnd, property) == NULL;
}

#define _r_wnd_setcontext sw_compat_setcontext
#define _r_wnd_removecontext sw_compat_removecontext

static inline LPWSTR sw_compat_basename (
	_In_ LPCWSTR path
)
{
	R_STRINGREF ref;

	_r_obj_initializestringref (&ref, path);

	return _r_path_getbasename (&ref);
}

#define _r_path_getbasename2 sw_compat_basename

static inline PLARGE_INTEGER sw_compat_gettimestamp (
	_Out_ PLARGE_INTEGER out_buffer
)
{
	FILETIME file_time;

	GetSystemTimeAsFileTime (&file_time);

	out_buffer->LowPart = file_time.dwLowDateTime;
	out_buffer->HighPart = (LONG)file_time.dwHighDateTime;

	return out_buffer;
}

#define _r_sys_gettimestamp sw_compat_gettimestamp

typedef VOID (NTAPI *SW_COMPAT_THREAD_CALLBACK) (
	PVOID
);

typedef struct _SW_COMPAT_THREAD
{
	SW_COMPAT_THREAD_CALLBACK callback;
	PVOID argument;
} SW_COMPAT_THREAD, *PSW_COMPAT_THREAD;

static NTSTATUS NTAPI sw_compat_thread_entry (
	_In_ PVOID parameter
)
{
	SW_COMPAT_THREAD context;

	context = *(PSW_COMPAT_THREAD)parameter;

	_r_mem_free (parameter);

	context.callback (context.argument);

	return STATUS_SUCCESS;
}

static inline NTSTATUS sw_compat_createthread (
	_Out_opt_ PHANDLE output,
	_In_ HANDLE process,
	_In_ SW_COMPAT_THREAD_CALLBACK callback,
	_In_opt_ PVOID argument,
	_In_opt_ PR_ENVIRONMENT environment,
	_In_opt_ LPCWSTR name
)
{
	PSW_COMPAT_THREAD context;
	NTSTATUS status;

	context = _r_mem_allocate (sizeof (SW_COMPAT_THREAD));
	context->callback = callback;
	context->argument = argument;

	status = _r_sys_createthread (output, process, &sw_compat_thread_entry, context, environment, name);

	if (!NT_SUCCESS (status))
		_r_mem_free (context);

	return status;
}

#define _r_sys_createthread sw_compat_createthread
