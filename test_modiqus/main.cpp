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

#include "modiqus.h"

int main(int argc, const char * argv[])
{
    
    mq::mqCsoundWrapper* csound = new mq::mqCsoundWrapper();
//    csound->set_log_level(MQ_LOG_LEVEL_DEBUG);
    
    if (!csound->start(false))
    {
        MQ_LOG_FATAL("Modiqus engine failed initialization")
        csound->stop();
        exit(EXIT_FAILURE);
    }
    else
    {
        MQ_LOG_INFO("Modiqus engine initialized")
    }

    MQ_LOG_INFO("Hello, World!\n")
    csound->sendMessage("i1 0 1");
    while(1);
    csound->stop();
    return 0;
}

