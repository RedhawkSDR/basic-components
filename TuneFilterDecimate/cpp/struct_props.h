#ifndef STRUCTPROPS_H
#define STRUCTPROPS_H

/*******************************************************************************************

    AUTO-GENERATED CODE. DO NOT MODIFY

*******************************************************************************************/

#include <ossie/CorbaUtils.h>
#include <ossie/PropertyInterface.h>

struct filterProps_struct {
    filterProps_struct ()
    {
        FFT_size = 128;
        TransitionWidth = 800;
        Ripple = 0.01;
    };

    static std::string getId() {
        return std::string("filterProps");
    };

    CORBA::ULong FFT_size;
    double TransitionWidth;
    double Ripple;
};

inline bool operator>>= (const CORBA::Any& a, filterProps_struct& s) {
    CF::Properties* temp;
    if (!(a >>= temp)) return false;
    CF::Properties& props = *temp;
    for (unsigned int idx = 0; idx < props.length(); idx++) {
        if (!strcmp("FFT_size", props[idx].id)) {
            if (!(props[idx].value >>= s.FFT_size)) return false;
        }
        if (!strcmp("TransitionWidth", props[idx].id)) {
            if (!(props[idx].value >>= s.TransitionWidth)) return false;
        }
        if (!strcmp("Ripple", props[idx].id)) {
            if (!(props[idx].value >>= s.Ripple)) return false;
        }
    }
    return true;
};

inline void operator<<= (CORBA::Any& a, const filterProps_struct& s) {
    CF::Properties props;
    props.length(3);
    props[0].id = CORBA::string_dup("FFT_size");
    props[0].value <<= s.FFT_size;
    props[1].id = CORBA::string_dup("TransitionWidth");
    props[1].value <<= s.TransitionWidth;
    props[2].id = CORBA::string_dup("Ripple");
    props[2].value <<= s.Ripple;
    a <<= props;
};

inline bool operator== (const filterProps_struct& s1, const filterProps_struct& s2) {
    if (s1.FFT_size!=s2.FFT_size)
        return false;
    if (s1.TransitionWidth!=s2.TransitionWidth)
        return false;
    if (s1.Ripple!=s2.Ripple)
        return false;
    return true;
};

inline bool operator!= (const filterProps_struct& s1, const filterProps_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<filterProps_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    filterProps_struct tmp;
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
