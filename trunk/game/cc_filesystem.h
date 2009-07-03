/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
This source file is contained as part of CleanCode Quake2, a project maintained
by Paril, to 'clean up' and make Quake2 an easier source base to read and work with.

You may use any part of this code to help create your own bases and own mods off
this code if you wish. It is under the same license as Quake 2 source (as above),
therefore you are free to have to fun with it. All I ask is you email me so I can
list the mod on my page for CleanCode Quake2 to help get the word around. Thanks.
*/

//
// cc_filesystem.h
// File system, for managing server files.
// From EGL
//

#ifndef __FILES_H__
#define __FILES_H__

/*
=============================================================================

	FUNCTIONS

=============================================================================
*/

#define FS_FreeFile(buffer) _FS_FreeFile ((buffer),__FILE__,__LINE__)
#define FS_FreeFileList(list,num) _FS_FreeFileList ((list),(num),__FILE__,__LINE__)

void FS_CreatePath(char *path);
void FS_CopyFile(char *src, char *dst);

int FS_FileLength(fileHandle_t fileNum);
int FS_Tell(fileHandle_t fileNum);

int FS_Read(void *buffer, const int len, fileHandle_t fileNum);
int FS_Write(void *buffer, const int size, fileHandle_t fileNum);
void FS_Seek(fileHandle_t fileNum, const int offset, const EFSSeekOrigin seekOrigin);
int FS_OpenFile(const char *fileName, fileHandle_t *fileNum, const EFSOpenMode openMode);
void FS_CloseFile(fileHandle_t fileNum);

int FS_LoadFile(const char *path, void **buffer, bool const terminate);
void _FS_FreeFile(void *buffer, const char *fileName, const int fileLine);

int FS_FileExists(const char *path);

const char *FS_Gamedir();
void FS_SetGamedir(char *dir, bool firstTime);

int FS_FindFiles(const char *path, const char *filter, const char *extension, char **fileList, int maxFiles, const bool addGameDir, const bool recurse);
void _FS_FreeFileList(char **list, int num, const char *fileName, const int fileLine);

char *FS_NextPath(char *prevPath);

void FS_Init();

#endif // __FILES_H__
