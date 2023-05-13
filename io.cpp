/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "io.h"
#include "myExceptions.h"
#include "defines.h"
//#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <complex>
#include <iomanip>

void io::out::xdsout(const Params& params) {
        std::cout << " DETECTOR= EIGERR500\n"
                << " GAIN= 1.0\n"
                << " DATA_RANGE_FIXED_SCALE_FACTOR= 1 1800 1.0\n"
            << " OVERLOAD= " << (1<<20)-1
            << " MINIMUM_VALID_PIXEL_VALUE= " << 0
            << '\n'
            << " NX=" << " 1030" << " NY=" << " 514\n"
            << " QX= 0.075 QY= 0.075 \n"
            << " ORGX= " << 0.5*1030 + params.delta_orgx() << " ORGY= " << 514/2 + params.delta_orgy() << '\n'
            << " TRUSTED_REGION=  0.00  1.5\n" 
            // refine defaults for IDXREF, CORRECT, but nothing for INTEGRATIO
            << "! REFINE(IDXREF)=  POSITION BEAM AXIS ORIENTATION CELL\n"
            << "  REFINE(INTEGRATE)=  !POSITION BEAM AXIS ORIENTATION CELL\n"
            << "! REFINE(CORRECT)=  POSITION BEAM AXIS ORIENTATION CELL\n"
            << "! *** warning ***: overload and minimum_valid_pixel_value hardcoded\n";

        std::cout << "! JOB= XYCORR INIT COLSPOT IDXREF DEFPIX INTEGRATE CORRECT\n";
        std::cout << " JOB= XYCORR INIT COLSPOT IDXREF DEFPIX INTEGRATE CORRECT\n";
        std::cout << " SNRC= 50\n\n";
        std::cout << " DELPHI= 25\n";
        std::cout << " INCIDENT_BEAM_DIRECTION=0.0 0.0 1.0\n";
        std::cout << " FRACTION_OF_POLARIZATION=0.50\n";

        
}
void io::out::xdsout(const RunInfo& run) {
        // assumes detector swing axis always vertical

    /*
     some comments
     */
    std::cout << '\n';
    std::cout << "! Data extracted for run number " << run.run_number_ << "\n";
    std::cout << "! 2theta = " << std::setprecision(1) << std::fixed << std::setw(6) 
                << run.two_theta_ << "deg, assuming vertical swing axis\n";
    if (run.omegascan_) {
        std::cout << "! Rotation about omega axis\n";
    }
    else if (run.phiscan_) {
        std::cout << "! Rotation about phi axis\n";
    }
    std::cout << std::endl;
    /*
     Detector information, assuming hybrid pixel detector
     */
    std::cout << " DETECTOR= " << run.detector_ << '\n'
            << " OVERLOAD= " << (1<<20)-1
            << " MINIMUM_VALID_PIXEL_VALUE= " << 0
            << '\n'
            << " TRUSTED_REGION=  0.00  1.5\n" 
            // refine defaults for IDXREF, CORRECT, but nothing for INTEGRATIO
            << "! REFINE(IDXREF)=  POSITION BEAM AXIS ORIENTATION CELL\n"
            << "  REFINE(INTEGRATE)=  !POSITION BEAM AXIS ORIENTATION CELL\n"
            << "! REFINE(CORRECT)=  POSITION BEAM AXIS ORIENTATION CELL\n"
            << "! *** warning ***: overload and minimum_valid_pixel_value hardcoded\n";
    // at CuKa (lambda = 1.54A) gain should be 1.0
    if (run.lambda_ > 1.5) {
        std::cout << " GAIN= 1.0\n";
    }
    // at MoKa (lambda = 0.713A) gain is about 0.7A, but not determined!
    else if (run.lambda_ < 0.8) {
        std::cout << " GAIN= 0.7\n";
    }
    // otherwise leave gain unset
    
    std::cout << " NX=" << " 1031" << " NY=" << " 515";
        std::cout << " QX=" << run.pixelsize_ << " QY=" << run.pixelsize_
            << "\n";

    std::cout << "! JOB= XYCORR INIT COLSPOT IDXREF DEFPIX INTEGRATE CORRECT\n";
    std::cout << " JOB= XYCORR INIT COLSPOT IDXREF DEFPIX INTEGRATE CORRECT\n";
    std::cout << " SNRC= 50\n\n";
    std::cout << " DELPHI= 25\n";
    std::cout << " INCIDENT_BEAM_DIRECTION=0.0 0.0 1.0\n";
    std::cout << " FRACTION_OF_POLARIZATION=0.50\n";

    std::cout << " DIRECTION_OF_DETECTOR_X-AXIS= "
            << std::fixed
            << std::setprecision(6)
            << std::setw(9)
            << std::cos(M_PI/180.*run.two_theta_) << " 0.0 " 
                        << std::fixed
            << std::setprecision(6)
            << std::setw(9)
            << std::sin(M_PI/180.*run.two_theta_) << "\n";
    std::cout << " DIRECTION_OF_DETECTOR_Y-AXIS= "
            << "0.0 1.0 0.0\n";
    std::cout << " X-RAY_WAVELENGTH= " << run.lambda_ << "   !Angstroem\n";
    std::cout << " OSCILLATION_RANGE= " << std::abs(run.deltaAngle_) << " \n";

    std::cout << " ROTATION_AXIS= " << run.rotation_x_ << " " << run.rotation_y_
            << " " << run.rotation_z_ << "\n";

    std::cout << " ORGX= " << 515 << " ORGY= " << 257 << "\n";
    std::cout << " DETECTOR_DISTANCE= " << std::setprecision(3) << run.delta_ << " !(mm)\n";
    
    std::cout << '\n';
    //! STOE tuns seem to  always start at 1?
    std::cout << " DATA_RANGE= " << 1 << " " << run.numframes() << "\n";
    std::cout << '\n';
    
}

