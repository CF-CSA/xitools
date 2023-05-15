/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <iostream>
#include <iomanip>

#include "Utils.h"
#include "myExceptions.h"

std::string Utils::xdstemplate(const std::string root, unsigned short run, const std::string suffix) {
    if (run > 99) {
        std::cout << "*** Error: runs greater than 99 are not supported (by X-Area)\n";
        throw myExcepts::Format("Runs > 99");
    }
    std::ostringstream outp (root);
    outp << '_' << std::setw(2) << std::setfill('0') << run << "????." << suffix;
    return outp.str();
}

/**
 * Return a string containing the current date and time - 'ctime' reformatted
 * with the last carriage return removed
 */
std::string Utils::timestamp()
{
        std::time_t seconds = time(NULL);
        std::string now (std::ctime(&seconds));
        now.erase(now.end()-1);

        return now;
}
