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
 
#ifndef STRUCTPROPS_H
#define STRUCTPROPS_H

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

*******************************************************************************************/

#include <ossie/CorbaUtils.h>

struct sri_struct {
	sri_struct ()
	{
		xstart = 0.0;
		xdelta = 1.0;
		mode = 0;
		blocking = true;
	};

    std::string getId() {
        return std::string("sri");
    };
	
	double xstart;
	double xdelta;
	short mode;
	std::string streamID;
	bool blocking;
};

inline bool operator>>= (const CORBA::Any& a, sri_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("xstart", props[idx].id)) {
			if (!(props[idx].value >>= s.xstart)) return false;
		}
		if (!strcmp("xdelta", props[idx].id)) {
			if (!(props[idx].value >>= s.xdelta)) return false;
		}
		if (!strcmp("mode", props[idx].id)) {
			if (!(props[idx].value >>= s.mode)) return false;
		}
		if (!strcmp("streamID", props[idx].id)) {
			if (!(props[idx].value >>= s.streamID)) return false;
		}
		if (!strcmp("blocking", props[idx].id)) {
			if (!(props[idx].value >>= s.blocking)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const sri_struct& s) {
	CF::Properties props;
	props.length(5);
	props[0].id = CORBA::string_dup("xstart");
	props[0].value <<= s.xstart;
	props[1].id = CORBA::string_dup("xdelta");
	props[1].value <<= s.xdelta;
	props[2].id = CORBA::string_dup("mode");
	props[2].value <<= s.mode;
	props[3].id = CORBA::string_dup("streamID");
	props[3].value <<= s.streamID;
	props[4].id = CORBA::string_dup("blocking");
	props[4].value <<= s.blocking;
	a <<= props;
};

inline bool operator== (const sri_struct& s1, const sri_struct& s2) {
    if (s1.xstart!=s2.xstart)
        return false;
    if (s1.xdelta!=s2.xdelta)
        return false;
    if (s1.mode!=s2.mode)
        return false;
    if (s1.streamID!=s2.streamID)
        return false;
    if (s1.blocking!=s2.blocking)
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
