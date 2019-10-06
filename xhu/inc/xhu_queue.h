/*
 * Copyright (C) 2019 by Martin Dejean
 *
 * This file is part of Xhu.
 * Xhu is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xhu is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xhu.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef XHU_QUEUE_H
#define XHU_QUEUE_H

#include <stdbool.h>

void xhu_queue_init(int *head, int *tail);
void xhu_queue_enqueue(int *queue,int *tail, int element);
int xhu_queue_dequeue(int *queue,int *head);
bool xhu_queue_empty(int head,int tail);
bool xhu_queue_full(int tail,const int size);
void xhu_queue_display(int *queue,int head,int tail);
#endif // XHU_QUEUE_H
