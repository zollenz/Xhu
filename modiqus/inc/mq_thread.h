/*
 * Copyright (C) 2015 by Martin Dejean
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

#ifndef __MQ_THREAD_H__
#define __MQ_THREAD_H__

#include <pthread.h>

namespace mq
{
    class mqThread
    {
    public:
        mqThread() {}
        
        virtual ~mqThread() {}
        
        bool StartInternalThread()
        {
            return (pthread_create(&_thread, NULL, InternalThreadEntryFunc, this) == 0);
        }
        
        void JoinInternalThread()
        {
            (void) pthread_join(_thread, NULL);
        }
        
    protected:
        virtual void InternalThreadEntry() = 0;
        
    private:
        pthread_t _thread;
        
        static void* InternalThreadEntryFunc(void* This)
        {
            ((mqThread*)This)->InternalThreadEntry();
            return NULL;
        }
    };
}
#endif //__MQ_THREAD_H__
