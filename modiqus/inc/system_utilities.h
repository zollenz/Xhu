/*
* Copyright (C) 2019 by Martin Dejean
*
* This file is part of Modiqus.
* Modiqus is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Modiqus is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Modiqus.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef SYSTEM_UTILITIES_H
#define SYSTEM_UTILITIES_H

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include <time.h>
#include "debug.h"
#include "defs.h"

void mq_pause(mq_array_size_t seconds)
{
    clock_t goal = clock() + seconds * CLOCKS_PER_SEC;
    while (goal > clock());
}

long mq_get_timestamp_ms()
{
    clock_t timestamp = clock() * CLK_TCK;
    
    return (long)(timestamp / 1000);
}

char *mq_get_folder_path(const char *path)
{
    if (path == NULL) {
        return NULL;
    }
    
    const char *last_slash = strrchr(path, '/');
    mq_mem_size_t folder_path_size = last_slash - path;
    char *folder_path = (char *) malloc(folder_path_size + 1);
    
    if (folder_path == NULL) {
        return NULL;
    }
    
    strncpy(folder_path, path, folder_path_size);
    folder_path[folder_path_size] = '\0';
    
    return folder_path;
}

#endif //SYSTEM_UTILITIES_H
