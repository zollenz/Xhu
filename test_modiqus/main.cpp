//
//  main.cpp
//  test_modiq
//
//  Created by Martin Dejean on 23/02/14.
//  Copyright (c) 2014 Modicus. All rights reserved.
//

#include <iostream>
#include "modiqus.h"
#include "pugixml.hpp"

mq::S32 mq::dbgLevel = MQ_LOG_LEVEL_DEBUG;

int main(int argc, const char * argv[])
{    
    mq::mqCsoundWrapper* csound = new mq::mqCsoundWrapper();
    
    if (!csound->start())
    {
        MQ_LOG_FATAL("Modiqus engine failed initialization")
        csound->stop();
        exit(EXIT_FAILURE);
    }
    else
    {
        MQ_LOG_INFO( "Modiqus engine initialized")
    }

    std::cout << "Hello, World!\n";
    csound->sendMessage("i1 0 1");
    while(1);
    csound->stop();
    return 0;
}

