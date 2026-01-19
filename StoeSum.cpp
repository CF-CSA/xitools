/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   StoeSum.cpp
 * Author: tg
 * 
 * Created on July 28, 2021, 5:14 PM
 */

#include "StoeSum.h"

#include "myExceptions.h"
#include <iostream>
#include <iomanip>
#include <sstream>

/**
 * provide the SUM file and extract runs from it
 * @param sumfile
 * @param verbosity
 * @param fromoptimiser if true, read run list from Run Optimizer
 */
StoeSum::StoeSum(const std::string& sumfile, unsigned char verbosity, bool fromoptimiser) :
sumfile_(sumfile), verbosity_(verbosity) {
    inp_.open(sumfile_);
    if (!inp_.is_open()) {
        std::cout << "*** Error: Cannot open STOE sum file " << sumfile_
                << std::endl;
        throw myExcepts::FileMissing(sumfile_);
    }

    // if in recovery mode, read runs from Optimizer list
    if (fromoptimiser == true) {
        findOptimiser();
        runs_ = getRunsFromOptimiserList();
        
    }
    else {
        findLastBlock();
        runs_ = getRuns();
    }
}

StoeSum::~StoeSum() {
    if (inp_.is_open()) {
        inp_.close();
    }
}

/**
 * go through sum-file, find the last occurence of 'Begin of measurement'
 * and rewind inp_ to that position.
 * @return position of measurement block
 */
void StoeSum::findLastBlock() {
    std::streampos mblock(0);
    std::string dummy;
    while (!inp_.eof()) {
        inp_ >> dummy;
        if (inp_.fail()) {
            break;
        }
        if (dummy != "Begin") {
            continue;
        } else { // check if this begin is 'Begin of measurement block 
            std::string kw2, kw3;
            inp_ >> kw2 >> kw3;
            if (inp_.fail() || inp_.eof()) {
                std::cout << "*** Error: Cannot find line 'Begin of measurement'\n."
                        << "      Is this a STOE .sum-file?\n";
                throw myExcepts::Format("Begin of measurement line missing");
            }
            if (kw2 == "of" && kw3 == "measurement") { // store this block
                mblock = inp_.tellg();
            }
        }
    }
    if (mblock == 0) {
        std::cout << "*** Error: Cannot find line 'Begin of measurement'\n."
                << "      Is this a STOE .sum-file?\n";
        throw myExcepts::Format("Begin of measurement line missing");
    }
    inp_.clear();
    inp_.seekg(mblock);

}
/**
 * got through sum-file and find 'Run Optimizer STADIVARI' as start of optimiser block
 */
void StoeSum::findOptimiser() {
    std::streampos mblock(0);
    std::string dummy;
    while (!inp_.eof()) {
        inp_ >> dummy;
        if (inp_.fail()) {
            break;
        }
        if (dummy != "Run") {
            continue;
        } else { // check if this begin is 'Begin of measurement block 
            std::string kw2, kw3;
            inp_ >> kw2 >> kw3;
            if (inp_.fail() || inp_.eof()) {
                std::cout << "*** Error: Cannot find line 'Run Frames S.'\n."
                        << "      Is this a STOE .sum-file?\n";
                throw myExcepts::Format("Begin of optimised run list missing");
            }
            if (kw2 == "Optimizer" && kw3 == "STADIVARI") { // store this block
                mblock = inp_.tellg();
            }
        }
    }
    if (mblock == 0) {
        std::cout << "*** Error: Cannot find line 'Begin of measurement'\n."
                << "      Is this a STOE .sum-file?\n";
        throw myExcepts::Format("Begin of measurement line missing");
    }
    inp_.clear();
    inp_.seekg(mblock);

    
}

/**
 * Get the number of Runs from sum file. Currently, only one clean measurement is supported,
 * i.e. only one line "Begin of measurement"
 * @return 
 */
int StoeSum::getRuns() {
    RunInfo myrun;
    std::string dummy;
    char colon;
    try {
        advanceKeyword("Wavelength");
    }
    catch (myExcepts::Format& e) {
        inp_.close();
        throw;
    }
    inp_ >> dummy >> colon >> myrun.lambda_;

    try {
        advanceKeyword("distance");
    }
    catch (myExcepts::Format& e) {
        inp_.close();
        throw;
    }
    inp_ >> dummy >> colon >> myrun.delta_;

    try {
        advanceKeyword("Pixelsize");
    }
    catch (myExcepts::Format& e) {
        inp_.close();
        throw;
    }
    inp_ >> dummy >> colon >> myrun.pixelsize_;

    try {
        advanceKeyword("Run");
    }
    catch (myExcepts::Format& e) {
        inp_.close();
        throw;
    }
    char c; int run(-1);
    
    runs_ = 0;
    do {
        inp_ >> c >> run;
        extractRun(run, myrun);
    } while (runs_ != run); 
    
    std::cout << "!---> Total number of runs: " << runs_ << '\n';
    
    std::cout << "!---> Found " << runs_info_.size() << " Runs in total\n";
    return runs_;
}


/**
 * read word by word from sum-file until @c keyword is found
 * @param keyword
 */
void StoeSum::advanceKeyword(const std::string& keyword) {
    std::string dummy;
    while (dummy != keyword) {
            inp_ >> dummy;
            if (verbosity_ > 1) {
                std::cout << "---> Keyword: " << keyword << ", found: " << dummy << std::endl;
            }
                    if (inp_.fail() || inp_.eof()) {
            std::cout << "*** Error: Cannot find keyword " << keyword << "\n."
                    << "      Is this a STOE .sum-file?\n";
            throw myExcepts::Format("Keyword " + keyword + " missing");
        }
    }
}
/**
 * expects inp_ to be at the correct position at
 * '2Theta : float' inside .sum file
 * @param run
 * @param myrun
 */
void StoeSum::extractRun(const int& run, RunInfo& myrun) {
    std::string dummy;
    char c;
    float number;
    myrun.run_number_ = run;
    inp_ >> dummy >> c >> myrun.two_theta_; // this should be '2Theta', otherwise unexpected format
    if (dummy != "2Theta") {
        std::cout << "*** Error: unexpected format of .sum file\n";
        throw myExcepts::Format("Unexpected .sum format");
    }
    inp_ >> dummy >> dummy >> c >> number >> c; 
    myrun.omega_[0] = number;
    // if c == '-': range, if 'C': chi value
    if (c == '-') {
        inp_ >> number;
        myrun.omega_[1] = number;
        myrun.omegascan_ = true;
    }
    inp_ >> dummy >> c >> myrun.chi_;
    inp_ >> dummy >> c >> number >> c;
    myrun.phi_[0] = number;
    if (c == '-') {
        inp_ >> number;
        myrun.phi_[1] = number;
        myrun.phiscan_ = true;
    }
    inp_ >> dummy >> dummy >> c >> myrun.deltaAngle_;
    inp_ >> dummy >> dummy >> c >> myrun.t_;
    inp_ >> dummy >> c >> myrun.nFrames_;
    inp_ >> dummy >> dummy >> c >> myrun.two_theta_range_[0] >> c >> myrun.two_theta_range_[1];
    inp_ >> dummy >> dummy >> dummy >> c >> myrun.d_range_[0] >> c >> myrun.d_range_[1];

    inp_ >> dummy; // should be 'Run' for next run, or 'First'
    if (inp_.fail() || inp_.eof()) {
        throw myExcepts::Format("Unexpected Format for sum file. Please contact author.\n");
    }
    
    runs_info_.push_back(myrun);
    
    // end of runs description
    if (dummy != "Run") {
        runs_ = run;
    }
    if (inp_.eof() && inp_.fail()) {
        std::cout << "*** Error: Unexpected 'Run'-block in sum file. Please contact the authors\n";
        throw myExcepts::Format("Run block fomrat");
    }
}

/**
 * extract run number from a 'master.h5' file and return the XDS template 
 * (the term 'master' replaced with question marks ?)
 * @param masterh5file
 * @return 
 */
std::string StoeSum::xdstemplate(const std::string& masterh5file, int& run) {
    size_t u1(0), u2(0);
    std::vector<std::string> words;
    std::string myrun;
    while ( (u2 = masterh5file.find('_', u1)) != std::string::npos) {
        std::string word (masterh5file.substr(u1, u2-u1));
        words.push_back (word);
        if (word.substr(0, 3) == "run") myrun = word;
        u1 = u2+1;
    }
    std::istringstream conv (myrun.substr(3));
    conv >> run;
    if (conv.fail()) {
        std::cout << "*** Error: extraction of run number from term " << myrun 
                << " failed\n";
        throw std::logic_error ("run extraction");
    }
    std::ostringstream xdstempl;
    for (auto it = words.begin(); it != words.end(); ++it) {
            xdstempl << *it << "_";
    }
    // the end, 'master.h5' is not found, will be added
    xdstempl << "??????.h5";
    return xdstempl.str();
}

/**
 * Use stem of path/to/project.sum to create xdstemplate
 * project_0r????.cbf
 * @param run
 * @return 
 */
std::string StoeSum::xdstemplate(int run) const {
    size_t dot = sumfile_.rfind(".sum");
    std::string my = sumfile_.substr(0, dot);
    std::ostringstream templ;
    templ << my << '_' << std::setw(2) << std::setfill('0') << run << "????.cbf";
    if (verbosity_ > 2) {
        std::cout << "! Name template for run " << run << ": " << templ.str() 
                << std::endl;
    }
    return templ.str();
    
    
}

/**
 * expects inp_ at start of 'Run Frames S', the header list of the Run Optimizer
 * First line gets discarded, runs are read one per line, expecting 14 numbers per line
 * @return 
 */
int StoeSum::getRunsFromOptimiserList() {
    
    RunInfo myrun;
    std::string dummy;
    int numRuns;
    try {
        advanceKeyword("Wavelength:");
    }
    catch (myExcepts::Format& e) {
        inp_.close();
        throw;
    }
    inp_ >> myrun.lambda_;
    try {
        advanceKeyword("distance:");
    }
    catch (myExcepts::Format& e) {
        inp_.close();
        throw;
    }
    inp_ >> myrun.delta_;
    try {
        advanceKeyword("Cell");
    }
    catch (myExcepts::Format& e) {
        inp_.close();
        throw;
    }
    // Optmiser knows cell, currently unused
    float a, b, c, alpha, beta, gamma;
    inp_ >> dummy >> a >> b >> c >> alpha >> beta >> gamma;
    // Rotation / frame
    try {
        advanceKeyword("frame:");
    }
    catch (myExcepts::Format& e) {
        inp_.close();
        throw;
    }
    inp_ >> myrun.deltaAngle_;

    // number of runs listed in header
    try {
        advanceKeyword("found:");
    }
    catch (myExcepts::Format& e) {
        inp_.close();
        throw;
    }
    inp_ >> numRuns;

    // jump forward for list of runs
    try {
        advanceKeyword("Redund.");
    }
    catch (myExcepts::Format& e) {
        inp_.close();
        throw;
    }
    while (!inp_.eof()) {
        // unused dummy variables
        short run, setting;
        float phi1, t_tot, compl0, compl1, redund0, redund1;
        inp_ >> run >> myrun.nFrames_ >> setting >> myrun.two_theta_ 
                >> myrun.omega_[0] >> myrun.omega_[1] >> myrun.chi_ >> myrun.phi_[0]
                >> myrun.t_ >> t_tot >> compl0 >> compl1 >> redund0 >> redund1;
        if (inp_.fail()) {
            break;
        }
        // Optimiser seems to create omega scans only
        myrun.omegascan_ = true;
        runs_info_.push_back(myrun);
    }
    
    return runs_info_.size();
    
}
