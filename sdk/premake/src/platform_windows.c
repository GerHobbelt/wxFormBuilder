/**********************************************************************
 * Premake - platform_windows.h
 * Windows-specific functions.
 *
 * Copyright (c) 2002-2006 Jason Perkins and the Premake project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License in the file LICENSE.txt for details.
 **********************************************************************/

#include "os.h"
#if defined(PLATFORM_WINDOWS)

#include <stdlib.h>
#include "io.h"
#include "path.h"
#include "util.h"
#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static char buffer[8192];

struct PlatformMaskData
{
	char* maskPath;
	HANDLE handle;
	WIN32_FIND_DATAA entry;
	int isFirst;
};

static int (__stdcall *CoCreateGuid)(LPSTR) = NULL;


int platform_chdir(const char* path)
{
	return SetCurrentDirectoryA(path);
}


int platform_copyfile(const char* src, const char* dest)
{
	return CopyFileA(src, dest, FALSE);
}


int platform_findlib(const char* name, char* buffer, int len)
{
	HMODULE hDll = LoadLibraryA(name);
	if (hDll != NULL)
	{
		GetModuleFileNameA(hDll, buffer, len);
		strcpy(buffer, path_getdir(buffer));
		FreeLibrary(hDll);
		return 1;
	}
	else
	{
		return 0;
	}
}

int platform_getcwd(char* buffer, int len)
{
	GetCurrentDirectoryA(len, buffer);
	return 1;
}


void platform_getuuid(char* uuid)
{
	if (CoCreateGuid == NULL)
	{
		HMODULE hOleDll = LoadLibraryA("OLE32.DLL");
		CoCreateGuid = (int(__stdcall*)(LPSTR))GetProcAddress(hOleDll, "CoCreateGuid");
	}
	CoCreateGuid(uuid);
}


int platform_isAbsolutePath(const char* path)
{
	return (path[0] == '/' || path[0] == '\\' || (strlen(path) > 1 && path[1] == ':'));
}


int platform_mask_close(MaskHandle data)
{
	if (data->handle != INVALID_HANDLE_VALUE)
		FindClose(data->handle);
	free(data->maskPath);
	free(data);
	return 1;
}


const char* platform_mask_getname(MaskHandle data)
{
	strcpy(buffer, data->maskPath);
	if (strlen(buffer) > 0)
		strcat(buffer, "/");
	strcat(buffer, data->entry.cFileName);
	return buffer;
}


int platform_mask_getnext(MaskHandle data)
{
	if (data->handle == INVALID_HANDLE_VALUE)
		return 0;

	if (data->isFirst)
	{
		data->isFirst = 0;
		return 1;
	}
	else
	{
		return FindNextFileA(data->handle, &data->entry);
	}
}


int platform_mask_isfile(MaskHandle data)
{
	return (data->entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}


MaskHandle platform_mask_open(const char* mask)
{
	const char* path = path_getdir(mask);

	MaskHandle data = ALLOCT(struct PlatformMaskData);
	data->handle = FindFirstFileA(mask, &data->entry);
	data->maskPath = (char*)malloc(strlen(path) + 1);
	strcpy(data->maskPath, path);
	data->isFirst  = 1;
	return data;
}


int platform_mkdir(const char* path)
{
	return CreateDirectoryA(path, NULL);
}


int platform_remove(const char* path)
{
	DeleteFileA(path);
	return 1;
}


int platform_rmdir(const char* path)
{
	WIN32_FIND_DATAA data;
	HANDLE hDir;

	char* buffer = (char*)malloc(strlen(path) + 6);
	strcpy(buffer, path);
	strcat(buffer, "\\*.*");
	hDir = FindFirstFileA(buffer, &data);
	if (hDir == INVALID_HANDLE_VALUE)
		return 0;
	free(buffer);

	do
	{
		if (strcmp(data.cFileName, ".") == 0) continue;
		if (strcmp(data.cFileName, "..") == 0) continue;

		buffer = (char*)malloc(strlen(path) + strlen(data.cFileName) + 2);
		strcpy(buffer, path);
		strcat(buffer, "\\");
		strcat(buffer, data.cFileName);

		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			platform_rmdir(buffer);
		}
		else
		{
			/* In order to delete not-checked-out version controlled files I need
			 * to clear the read-only flag first */
			if (data.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
				SetFileAttributesA(buffer, data.dwFileAttributes & !FILE_ATTRIBUTE_READONLY);
			DeleteFileA(buffer);
		}

		free(buffer);
	} while (FindNextFileA(hDir, &data));
	FindClose(hDir);

	RemoveDirectoryA(path);
	return 1;
}

#endif
