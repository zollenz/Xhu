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

#include "xhu_queue.h"
#include <stdio.h>
/*
 initialize queue pointers
 */
void xhu_queue_init(int *head, int *tail)
{
    *head = *tail = 0;
}

/*
 enqueue an element
 precondition: the queue is not full
 */
void xhu_queue_enqueue(int *q,int *tail, int element)
{
    q[(*tail)++] = element;
}

/*
 dequeue an element
 precondition: queue is not empty
 */
int xhu_queue_dequeue(int *q,int *head)
{
    return q[(*head)++];
}

/*
 return 1 if queue is full, otherwise return 0
 */
bool xhu_queue_full(int tail,const int size)
{
    return tail == size;
}

/*
 return 1 if the queue is empty, otherwise return 0
 */
bool xhu_queue_empty(int head, int tail)
{
    return tail == head;
}

/*
 display queue content
 */
void xhu_queue_display(int *q,int head,int tail)
{
    int i = tail - 1;
    while(i >= head)
        printf("%d ",q[i--]);
    printf("\n");
}
