/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
 * source distribution.
 * 
 * This file is part of REDHAWK Basic Components.
 * 
 * REDHAWK Basic Components is free software: you can redistribute it and/or modify it under the terms of 
 * the GNU Lesser General Public License as published by the Free Software Foundation, either 
 * version 3 of the License, or (at your option) any later version.
 * 
 * REDHAWK Basic Components is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with this 
 * program.  If not, see http://www.gnu.org/licenses/.
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
// Description:
//   This file contains the Tuner class declaration.
//
//   TO DO:
//      1. *** WORK IN PROGRESS ***
//      2. Filter bandwidth needs to be added
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef __TUNER_H__
#define __TUNER_H__

//#define TUNER_DEBUG // Comment out to disable

#include "DataTypes.h"


class Tuner
{
public:
    Tuner(ComplexArray &input, ComplexArray &output, const Real normFc);
    virtual ~Tuner();

    bool run(void);
    void retune(Real normFc);
    void reset(void);

private:
    ComplexArray    &_input;               // Reference to input buffer
    ComplexArray    &_output;              // Reference to output buffer

    Complex          _phasor;              // Current phase
    Complex          _dphasor;             // Phase increment

#ifdef TUNER_DEBUG
    ComplexVector phasorVec;
#endif
};

#endif  // __TUNER_H__
