/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This file is part of REDHAWK Basic Components whitenoise.
 *
 * REDHAWK Basic Components whitenoise is free software: you can redistribute it and/or modify it under the terms of
 * the GNU Lesser General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * REDHAWK Basic Components whitenoise is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this
 * program.  If not, see http://www.gnu.org/licenses/.
 */
#ifndef STRUCTPROPS_H
#define STRUCTPROPS_H

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

*******************************************************************************************/

#include <ossie/CorbaUtils.h>
#include <ossie/PropertyInterface.h>

struct sri_struct {
    sri_struct ()
    {
        xdelta = 1.0;
        complex = false;
    };

    std::string getId() {
        return std::string("sri");
    };

    float xdelta;
    bool complex;
    std::string streamID;
};

inline bool operator>>= (const CORBA::Any& a, sri_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("xdelta", props[idx].id)) {
            if (!(props[idx].value >>= s.xdelta)) return false;
        }
        if (!strcmp("complex", props[idx].id)) {
            if (!(props[idx].value >>= s.complex)) return false;
        }
        if (!strcmp("streamID", props[idx].id)) {
            if (!(props[idx].value >>= s.streamID)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const sri_struct& s) {
    CF::Properties props;
    props.length(3);
    props[0].id = CORBA::string_dup("xdelta");
    props[0].value <<= s.xdelta;
    props[1].id = CORBA::string_dup("complex");
    props[1].value <<= s.complex;
    props[2].id = CORBA::string_dup("streamID");
    props[2].value <<= s.streamID;
    a <<= props;
};

inline bool operator== (const sri_struct& s1, const sri_struct& s2) {
    if (s1.xdelta!=s2.xdelta)
        return false;
    if (s1.complex!=s2.complex)
        return false;
    if (s1.streamID!=s2.streamID)
        return false;
    return true;
};

inline bool operator!= (const sri_struct& s1, const sri_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<sri_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    sri_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}


#endif
