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

int main(int argc, const char * argv[])
{
//    pugi::xml_document doc;
//    std::string str = doc.path();
//    modiqus::CsoundWrapper wrapper;
//    wrapper.start();
    
    modiqus::Core* test = new modiqus::Core();
    test->start(modiqus::ENGINE_MODE_GAME);
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

