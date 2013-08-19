/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
 * source distribution.
 * 
 * This file is part of REDHAWK Basic Components dsp library.
 * 
 * REDHAWK Basic Components dsp library is free software: you can redistribute it and/or modify it under the terms of 
 * the GNU Lesser General Public License as published by the Free Software Foundation, either 
 * version 3 of the License, or (at your option) any later version.
 * 
 * REDHAWK Basic Components dsp library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with this 
 * program.  If not, see http://www.gnu.org/licenses/.
 */
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
// Description:
//   This file contains the Tuner class implementation.
//
//   TO DO:
//      1. *** WORK IN PROGRESS ***
//      2. Filter bandwidth needs to be added
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#include <cmath>
#include <numeric>
#include "Tuner.h"

#ifdef TUNER_DEBUG
#include <fstream>
#include <iostream>
#endif

using namespace std;


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
// Description:
//   A phasor class *** WORK IN PROGRESS ***
//
// Arguments:
//  None.
//
// Return Value:
//  None.
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

template<class T> class Phasor
{
public:
    Phasor (T normFc = 0)
    {
        init(normFc);
    }

    void init (T normFc)
    {
        _dphasor = T(cos(2*M_PI*normFc), sin(-2*M_PI*normFc));
    }

    T operator()(T op)
    {
        T retval = op * _phasor;
        _phasor *= _dphasor;    // This may have to be recalculated using exp()
        return retval;
    }

private:
    T _phasor;
    T _dphasor;
};


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
// Description:
//   Tuner's constructor.  Use parameters to construct lowpass filter.
//
// Parameters:
//   input - reference to an array of samples
//   output - reference to output array of samples
//   normFc - normalized (ie, Fc/Fs) beat frequency
//
// Return Value:
//   None.
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

Tuner::Tuner(ComplexArray &input, ComplexArray &output, const Real normFc) :
    _input(input),
    _output(output)
{
    _output.resize(_input.size());
    reset();
    retune(normFc);
}


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
// Description:
//   Tuner's destructor.
//
// Parameters:
//   None.
//
// Return Value:
//   None.
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

Tuner::~Tuner()
{
    //
    // Hook
    //
}


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
// Description:
//   This method resets the phasor used for frequency-shifting the input
//   samples.
//
// Parameters:
//   normFc - normalized (ie, Fc/Fs) tune frequency w.r.t. 0
//
// Return Value:
//   None.
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

void Tuner::retune(Real normFc)
{
    _dphasor = Complex(cos(2*M_PI*normFc), sin(-2*M_PI*normFc));
}


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
// Description:
//   This method resets the phasor back to its initial value.
//
// Parameters:
//   None.
//
// Return Value:
//   None.
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

void Tuner::reset(void)
{
    _phasor = Complex(1,0);
}


//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
// Description:
//   Tuner's main processing method.  It shifts the input samples by multiplying
//   them by a phasor.
//
// Parameters:
//   input - the input samples
//   output - the input samples shifted in frequency
//
// Return Value:
//   None.
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

bool Tuner::run(void)
{
	//transform(&_input[0], &_input[_input.size()], &_output[0], Phasor<ComplexArray::value_type>());
    for (Complex *x= &_input[0],
                 *xend = &_input[_input.size()],
                 *y    = &_output[0];
                 x != xend; ++x, ++y)
    {
        *y = *x * _phasor;
#ifdef TUNER_DEBUG
    	phasorVec.push_back(_phasor);
#endif
    	_phasor *= _dphasor;
    }

#ifdef TUNER_DEBUG
    std::ofstream tunerFile("tunersinusoid.dat", ios_base::out|ios_base::binary);
	tunerFile.write((char *)&phasorVec[0], phasorVec.size()*sizeof(phasorVec[0]));
	tunerFile.close();
#endif

    return true;
}
