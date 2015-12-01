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

#ifndef __MQ_CORE_EVENTS_H__
#define __MQ_CORE_EVENTS_H__

#include "mq_event.h"

namespace mq
{
    #define AudioEvents CoreEvents::Instance()
    
    class CoreEvents
    {
    public:
        Event<void, const F32* const> OutputDataReady;
        Event<void, const String&> OutputSilent;
        
        static CoreEvents& Instance()
        {
            static CoreEvents singleton;
            return singleton;
        }
        
    protected:
        
        CoreEvents() :
        OutputDataReady(this),
        OutputSilent(this)
        {}
        
        ~CoreEvents() {};
        
        CoreEvents(const CoreEvents&);              // Prevent copy-construction
        
        CoreEvents& operator=(const CoreEvents&);   // Prevent assignment        
    };
}
#endif //__MQ_CORE_EVENTS_H__
