/*
 * Copyright (C) 2018 by Martin Dejean
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

#ifndef THREAD_HPP
#define THREAD_HPP

#include <pthread.h>

namespace mq
{
    class Thread
    {
        
    public:
        
        Thread() {}
        
        virtual ~Thread() {}
        
        bool startInternalThread()
        {
            return (pthread_create(&_thread, NULL, internalThreadEntryFunc, this) == 0);
        }
        
        void joinInternalThread()
        {
            (void) pthread_join(_thread, NULL);
        }
        
    protected:
        
        virtual void internalThreadEntry() = 0;
        
    private:
        
        pthread_t _thread;
        
        static void* internalThreadEntryFunc(void* This)
        {
            ((Thread*)This)->internalThreadEntry();
            
            return NULL;
        }
        
    };
}
#endif //THREAD_HPP
