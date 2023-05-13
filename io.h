/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   io.h
 * Author: tg
 *
 * Created on May 29, 2019, 8:48 AM
 */

#ifndef IO_H
#define IO_H

#include "RunInfo.h"
#include "Params.h"

namespace io {
    namespace out {
        //! print standard settings assuming EIGER2 R500
        void xdsout(const Params& params);

        //! print parameters from a RunInfo
        void xdsout(const RunInfo& runinfo);
    }
}

#endif /* IO_H */

