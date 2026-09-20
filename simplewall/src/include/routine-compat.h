// Compatibility layer between current simplewall and the public henrypp/routine SDK.
// The published routine repo split section-aware config helpers into *_ex functions
// and dropped types that simplewall still uses. Do not include this from routine.c.
#pragma once

#ifndef PCR_STRINGREF
typedef const R_STRINGREF *PCR_STRINGREF;
#endif

#ifndef PCSID
typedef const SID *PCSID;
#endif

#ifndef PR_SIZE_BUFFER_MINIMUM
#define PR_SIZE_BUFFER_MINIMUM PR_SIZE_BUFFER_OVERFLOW
#endif

#ifndef IN6_IS_ADDR_ULA
#define IN6_IS_ADDR_ULA(address) ((((const BYTE *)(address))[0] & 0xFE) == 0xFC)
#endif

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

// Public SDK added a precision flag; simplewall still calls the one-argument form.
#define _r_format_interval(seconds) _r_format_interval ((seconds), FALSE)

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
