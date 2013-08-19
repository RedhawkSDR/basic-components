/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
 * source distribution.
 * 
 * This file is part of REDHAWK Basic Components HardLimit.
 * 
 * REDHAWK Basic Components HardLimit is free software: you can redistribute it and/or modify it under the terms of 
 * the GNU Lesser General Public License as published by the Free Software Foundation, either 
 * version 3 of the License, or (at your option) any later version.
 * 
 * REDHAWK Basic Components HardLimit is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with this 
 * program.  If not, see http://www.gnu.org/licenses/.
 */
 
#ifndef HARDLIMIT_IMPL_H
#define HARDLIMIT_IMPL_H

#include "HardLimit_base.h"

class HardLimit_i;

class HardLimit_i : public HardLimit_base
{
    ENABLE_LOGGING
    public: 
        HardLimit_i(const char *uuid, const char *label);
        ~HardLimit_i();
        int serviceFunction();
};

#endif
