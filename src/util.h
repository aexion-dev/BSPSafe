#pragma once

#include <string>
#include <iostream>
#include <cstdlib>

void fatal_error(const std::string& msg) {
    std::cerr << "fatal error: " << msg << "\n";
    std::_Exit(1);
}
