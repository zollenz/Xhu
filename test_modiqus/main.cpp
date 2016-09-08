//
//  main.cpp
//  test_modiq
//
//  Created by Martin Dejean on 23/02/14.
//  Copyright (c) 2014 Modicus. All rights reserved.
//

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

