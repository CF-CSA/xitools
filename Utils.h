/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Utils.h
 * Author: tg
 *
 * Created on May 13, 2023, 9:31 PM
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {
    std::string xdstemplate (const std::string root, unsigned short run, const std::string suffix="cbf");
    std::string timestamp();
}

#endif /* UTILS_H */

