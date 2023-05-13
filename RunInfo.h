/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RunInfo.h
 * Author: tg
 *
 * Created on August 2, 2021, 12:12 PM
 */

#ifndef RUNINFO_H
#define RUNINFO_H

#include <cstdlib>

#include "Params.h"


/**
 * Angles are expected in radians, not angles!
 */
class RunInfo {
public:
    int run_number_;
    std::string detector_;
        float lambda_;
        float delta_;
        float two_theta_;
        float omega_[2];
        float chi_;
        float phi_[2];
        float deltaAngle_;
        float t_;
        float nFrames_;
        float two_theta_range_[2];
        float d_range_[2];
        // flags to know what type of scan we have
        bool phiscan_, omegascan_;
        float pixelsize_; // mm assumed
        float rotation_x_, rotation_y_, rotation_z_;
        
    void rotationaxis();

public:
    RunInfo();
    ~RunInfo(){}
    
    // apply offsets from Params
    void applyOffsets(const Params& params);
    
    // return total rotation range
    float totalAngle() const;
    
    // return number of frames
    int numframes() const { return (int) totalAngle()/deltaAngle_; }
    
};

#endif /* RUNINFO_H */

