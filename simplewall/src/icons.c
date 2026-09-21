// simplewall
// Copyright (c) 2016-2026 Henry++

#include "global.h"

#ifndef SIID_APPLICATION
#define SIID_APPLICATION 2
#endif
#ifndef SIID_SOFTWARE
#define SIID_SOFTWARE 82
#endif
#ifndef SIID_SETTINGS
#define SIID_SETTINGS 106
#endif
#ifndef SIID_SHIELD
#define SIID_SHIELD 77
#endif
#ifndef SIID_DESKTOPPC
#define SIID_DESKTOPPC 47
#endif

_Success_ (return)
BOOLEAN _app_icons_loadstock (
	_In_ INT stock_id,
	_Out_opt_ PLONG out_icon_id,
	_Out_opt_ HICON_PTR out_hicon
)
{
	SHSTOCKICONINFO sii;

	RtlZeroMemory (&sii, sizeof (sii));
	sii.cbSize = sizeof (sii);

	if (FAILED (SHGetStockIconInfo ((SHSTOCKICONID)stock_id, SHGSI_ICON | SHGSI_SYSICONINDEX | SHGSI_LARGEICON, &sii)))
		return FALSE;

	if (out_icon_id)
		*out_icon_id = sii.iSysImageIndex;

	if (out_hicon)
		*out_hicon = sii.hIcon;
	else if (sii.hIcon)
		DestroyIcon (sii.hIcon);

	return TRUE;
}

PICON_INFORMATION _app_icons_getdefault ()
{
	static R_INITONCE init_once = PR_INITONCE_INIT;
	static ICON_INFORMATION icon_info = {0};

	PR_STRING path;

	if (_r_initonce_begin (&init_once))
	{
		// Shell blank .exe glyph — used to detect iconless binaries
		if (!_app_icons_loadstock (SIID_APPLICATION, &icon_info.generic_icon_id, NULL))
			icon_info.generic_icon_id = 0;

		// Missing / iconless apps: package glyph (clearer than blank window)
		if (!_app_icons_loadstock (SIID_SOFTWARE, &icon_info.app_icon_id, &icon_info.app_hicon))
			_app_icons_loadfromfile (config.svchost_path, DATA_UNKNOWN, &icon_info.app_icon_id, &icon_info.app_hicon, FALSE);

		// Services: gear
		if (!_app_icons_loadstock (SIID_SETTINGS, &icon_info.service_icon_id, &icon_info.service_hicon))
		{
			path = _r_obj_concatstrings (2, _r_sys_getsystemdirectory ()->buffer, L"\\shell32.dll");
			_app_icons_loadfromfile (path, DATA_UNKNOWN, &icon_info.service_icon_id, &icon_info.service_hicon, FALSE);
			_r_obj_dereference (path);
		}

		// UWP: same package family as missing apps (distinct from Win32 blank)
		if (!_app_icons_loadstock (SIID_SOFTWARE, &icon_info.uwp_icon_id, &icon_info.uwp_hicon))
		{
			if (_r_sys_isosversiongreaterorequal (WINDOWS_8))
			{
				path = _r_obj_concatstrings (2, _r_sys_getsystemdirectory ()->buffer, L"\\wsreset.exe");
				_app_icons_loadfromfile (path, DATA_UNKNOWN, &icon_info.uwp_icon_id, &icon_info.uwp_hicon, FALSE);
				_r_obj_dereference (path);
			}
		}

		// Windows / system binaries: shield (falls back to desktop PC)
		if (!_app_icons_loadstock (SIID_SHIELD, &icon_info.system_icon_id, &icon_info.system_hicon) &&
			!_app_icons_loadstock (SIID_DESKTOPPC, &icon_info.system_icon_id, &icon_info.system_hicon))
		{
			path = _r_obj_concatstrings (2, _r_sys_getsystemdirectory ()->buffer, L"\\imageres.dll");
			_app_icons_loadfromfile (path, DATA_UNKNOWN, &icon_info.system_icon_id, &icon_info.system_hicon, FALSE);
			_r_obj_dereference (path);
		}

		_r_initonce_end (&init_once);
	}

	return &icon_info;
}

_Ret_maybenull_
HICON _app_icons_getdefaultapp_hicon ()
{
	PICON_INFORMATION icon_info = _app_icons_getdefault ();

	return icon_info->app_hicon ? CopyIcon (icon_info->app_hicon) : NULL;
}

_Ret_maybenull_
HICON _app_icons_getdefaulttype_hicon (
	_In_ ENUM_TYPE_DATA type,
	_In_ PICON_INFORMATION icon_info
)
{
	if (type == DATA_APP_SERVICE)
	{
		if (icon_info->service_hicon)
			return CopyIcon (icon_info->service_hicon);
	}
	else if (type == DATA_APP_UWP)
	{
		if (icon_info->uwp_hicon)
			return CopyIcon (icon_info->uwp_hicon);
	}

	return icon_info->app_hicon ? CopyIcon (icon_info->app_hicon) : NULL;
}

LONG _app_icons_getdefaultapp_id (
	_In_ ENUM_TYPE_DATA type
)
{
	PICON_INFORMATION icon_info = _app_icons_getdefault ();

	if (type == DATA_APP_SERVICE)
		return icon_info->service_icon_id;

	if (type == DATA_APP_UWP)
		return icon_info->uwp_icon_id;

	return icon_info->app_icon_id;
}

LONG _app_icons_getdefaultsystem_id ()
{
	PICON_INFORMATION icon_info = _app_icons_getdefault ();

	return icon_info->system_icon_id ? icon_info->system_icon_id : icon_info->app_icon_id;
}

BOOLEAN _app_icons_isgenericid (
	_In_ LONG icon_id,
	_In_ PICON_INFORMATION icon_info
)
{
	if (!icon_id)
		return TRUE;

	if (icon_info->generic_icon_id && icon_id == icon_info->generic_icon_id)
		return TRUE;

	return FALSE;
}

_Ret_maybenull_
HICON _app_icons_getsafeapp_hicon (
	_In_ ULONG app_hash
)
{
	PICON_INFORMATION icon_info;
	PITEM_APP ptr_app;
	HICON hicon;
	LONG icon_id;
	BOOLEAN is_system;

	icon_info = _app_icons_getdefault ();
	ptr_app = _app_getappitem (app_hash);

	if (!ptr_app)
		return icon_info->app_hicon ? CopyIcon (icon_info->app_hicon) : NULL;

	if (_r_obj_isstringempty (ptr_app->real_path))
	{
		_r_obj_dereference (ptr_app);
		return NULL;
	}

	is_system = _app_isappfromsystem (ptr_app->real_path, app_hash);

	if (_r_config_getboolean (L"IsIconsHidden", FALSE, NULL) || !_app_isappvalidbinary (ptr_app->real_path))
	{
		if (is_system && icon_info->system_hicon)
			hicon = CopyIcon (icon_info->system_hicon);
		else
			hicon = _app_icons_getdefaulttype_hicon (ptr_app->type, icon_info);

		_r_obj_dereference (ptr_app);

		return hicon;
	}

	_app_icons_loadfromfile (ptr_app->real_path, ptr_app->type, &icon_id, &hicon, TRUE);

	if (_app_icons_isgenericid (icon_id, icon_info) ||
		((ptr_app->type == DATA_APP_SERVICE || ptr_app->type == DATA_APP_UWP) && icon_id == icon_info->app_icon_id))
	{
		if (hicon)
			DestroyIcon (hicon);

		if (is_system && icon_info->system_hicon)
			hicon = CopyIcon (icon_info->system_hicon);
		else
			hicon = _app_icons_getdefaulttype_hicon (ptr_app->type, icon_info);
	}

	_r_obj_dereference (ptr_app);

	return hicon;
}

VOID _app_icons_loaddefaults (
	_In_ ENUM_TYPE_DATA type,
	_Inout_opt_ HICON_PTR out_hicon,
	_Inout_opt_ PLONG out_icon_id
)
{
	PICON_INFORMATION icon_info;

	icon_info = _app_icons_getdefault ();

	if (out_hicon)
	{
		if (*out_hicon == NULL)
		{
			if (type == DATA_APP_UWP && icon_info->uwp_hicon)
				*out_hicon = CopyIcon (icon_info->uwp_hicon);
			else if (type == DATA_APP_SERVICE && icon_info->service_hicon)
				*out_hicon = CopyIcon (icon_info->service_hicon);
			else if (icon_info->app_hicon)
				*out_hicon = CopyIcon (icon_info->app_hicon);
		}
	}

	if (out_icon_id)
	{
		if (_app_icons_isgenericid (*out_icon_id, icon_info) ||
			((type == DATA_APP_SERVICE || type == DATA_APP_UWP) && *out_icon_id == icon_info->app_icon_id))
		{
			if (type == DATA_APP_SERVICE)
				*out_icon_id = icon_info->service_icon_id;
			else if (type == DATA_APP_UWP)
				*out_icon_id = icon_info->uwp_icon_id;
			else
				*out_icon_id = icon_info->app_icon_id;
		}
	}
}

VOID _app_icons_loadfromfile (
	_In_opt_ PR_STRING path,
	_In_ ENUM_TYPE_DATA type,
	_Out_opt_ PLONG out_icon_id,
	_Out_opt_ HICON_PTR out_hicon,
	_In_ BOOLEAN is_loaddefaults
)
{
	if (!out_icon_id && !out_hicon)
		return; // STATUS_INVALID_PARAMETER_MIX

	if (out_icon_id)
		*out_icon_id = 0;

	if (out_hicon)
		*out_hicon = NULL;

	if (!_r_obj_isstringempty (path))
		_r_path_geticon (&path->sr, out_hicon, out_icon_id);

	if (is_loaddefaults)
		_app_icons_loaddefaults (type, out_hicon, out_icon_id);
}
