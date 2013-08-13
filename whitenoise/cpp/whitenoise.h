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
#ifndef WHITENOISE_IMPL_H
#define WHITENOISE_IMPL_H

#include <vector>
#include "whitenoise_base.h"

class whitenoise_i;

class whitenoise_i : public whitenoise_base
{
    ENABLE_LOGGING
    public:
        whitenoise_i(const char *uuid, const char *label);
        ~whitenoise_i();
        int serviceFunction();
        void initialize() throw (CF::LifeCycle::InitializeError, CORBA::SystemException);
        std::vector<float> output;
        void newSri(const std::string& sriStr);
        void newXferLen(const std::string& sriStr);

        void configure (const CF::Properties& configProperties) throw (CF::PropertySet::PartialConfiguration,
                CF::PropertySet::InvalidConfiguration, CORBA::SystemException);
        boost::mutex processLock;
        std::string streamID;
        bool sriChanged;
        BULKIO::StreamSRI new_sri;
};

#endif
