/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Mat33.cpp
 * Author: tg
 * 
 * Created on July 25, 2023, 9:59 PM
 */

#include "Mat33.h"

Mat33::Mat33() {
    for (int i=0; i <9; ++i) {
        matrix_[i] = 0.0;
    }
}


Mat33::~Mat33() {
}

