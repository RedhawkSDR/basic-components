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
#include <ossie/PropertyInterface.h>

struct Octet_struct {
	Octet_struct ()
	{
		Octet__MaxValue = 255;
		Octet__MinValue = 0;
	};

    std::string getId() {
        return std::string("Octet");
    };
	
	unsigned char Octet__MaxValue;
	unsigned char Octet__MinValue;
};

inline bool operator>>= (const CORBA::Any& a, Octet_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Octet::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= CORBA::Any::to_octet(s.Octet__MaxValue))) return false;
		}
		if (!strcmp("Octet::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= CORBA::Any::to_octet(s.Octet__MinValue))) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Octet_struct& s) {
	CF::Properties props;
	props.length(2);
	props[0].id = CORBA::string_dup("Octet::MaxValue");
	props[0].value <<= CORBA::Any::from_octet(s.Octet__MaxValue);
	props[1].id = CORBA::string_dup("Octet::MinValue");
	props[1].value <<= CORBA::Any::from_octet(s.Octet__MinValue);
	a <<= props;
};

inline bool operator== (const Octet_struct& s1, const Octet_struct& s2) {
    if (s1.Octet__MaxValue!=s2.Octet__MaxValue)
        return false;
    if (s1.Octet__MinValue!=s2.Octet__MinValue)
        return false;
    return true;
};

inline bool operator!= (const Octet_struct& s1, const Octet_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Octet_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Octet_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Ushort_struct {
	Ushort_struct ()
	{
		Ushort__MaxValue = 65535;
		Ushort__MinValue = 0;
	};

    std::string getId() {
        return std::string("Ushort");
    };
	
	unsigned short Ushort__MaxValue;
	unsigned short Ushort__MinValue;
};

inline bool operator>>= (const CORBA::Any& a, Ushort_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Ushort::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Ushort__MaxValue)) return false;
		}
		if (!strcmp("Ushort::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Ushort__MinValue)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Ushort_struct& s) {
	CF::Properties props;
	props.length(2);
	props[0].id = CORBA::string_dup("Ushort::MaxValue");
	props[0].value <<= s.Ushort__MaxValue;
	props[1].id = CORBA::string_dup("Ushort::MinValue");
	props[1].value <<= s.Ushort__MinValue;
	a <<= props;
};

inline bool operator== (const Ushort_struct& s1, const Ushort_struct& s2) {
    if (s1.Ushort__MaxValue!=s2.Ushort__MaxValue)
        return false;
    if (s1.Ushort__MinValue!=s2.Ushort__MinValue)
        return false;
    return true;
};

inline bool operator!= (const Ushort_struct& s1, const Ushort_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Ushort_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Ushort_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Short_struct {
	Short_struct ()
	{
		Short__MaxValue = 32767;
		Short__MinValue = -32768;
	};

    std::string getId() {
        return std::string("Short");
    };
	
	short Short__MaxValue;
	short Short__MinValue;
};

inline bool operator>>= (const CORBA::Any& a, Short_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Short::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Short__MaxValue)) return false;
		}
		if (!strcmp("Short::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Short__MinValue)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Short_struct& s) {
	CF::Properties props;
	props.length(2);
	props[0].id = CORBA::string_dup("Short::MaxValue");
	props[0].value <<= s.Short__MaxValue;
	props[1].id = CORBA::string_dup("Short::MinValue");
	props[1].value <<= s.Short__MinValue;
	a <<= props;
};

inline bool operator== (const Short_struct& s1, const Short_struct& s2) {
    if (s1.Short__MaxValue!=s2.Short__MaxValue)
        return false;
    if (s1.Short__MinValue!=s2.Short__MinValue)
        return false;
    return true;
};

inline bool operator!= (const Short_struct& s1, const Short_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Short_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Short_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct ULong_struct {
	ULong_struct ()
	{
		ULong__MaxValue = 4294967295;
		ULong__MinValue = 0;
	};

    std::string getId() {
        return std::string("ULong");
    };
	
	CORBA::ULong ULong__MaxValue;
	CORBA::ULong ULong__MinValue;
};

inline bool operator>>= (const CORBA::Any& a, ULong_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("ULong::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.ULong__MaxValue)) return false;
		}
		if (!strcmp("ULong::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.ULong__MinValue)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const ULong_struct& s) {
	CF::Properties props;
	props.length(2);
	props[0].id = CORBA::string_dup("ULong::MaxValue");
	props[0].value <<= s.ULong__MaxValue;
	props[1].id = CORBA::string_dup("ULong::MinValue");
	props[1].value <<= s.ULong__MinValue;
	a <<= props;
};

inline bool operator== (const ULong_struct& s1, const ULong_struct& s2) {
    if (s1.ULong__MaxValue!=s2.ULong__MaxValue)
        return false;
    if (s1.ULong__MinValue!=s2.ULong__MinValue)
        return false;
    return true;
};

inline bool operator!= (const ULong_struct& s1, const ULong_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<ULong_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    ULong_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Long_struct {
	Long_struct ()
	{
		Long__MaxValue = 2147483647;
		Long__MinValue = -2147483648;
	};

    std::string getId() {
        return std::string("Long");
    };
	
	CORBA::Long Long__MaxValue;
	CORBA::Long Long__MinValue;
};

inline bool operator>>= (const CORBA::Any& a, Long_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Long::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Long__MaxValue)) return false;
		}
		if (!strcmp("Long::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Long__MinValue)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Long_struct& s) {
	CF::Properties props;
	props.length(2);
	props[0].id = CORBA::string_dup("Long::MaxValue");
	props[0].value <<= s.Long__MaxValue;
	props[1].id = CORBA::string_dup("Long::MinValue");
	props[1].value <<= s.Long__MinValue;
	a <<= props;
};

inline bool operator== (const Long_struct& s1, const Long_struct& s2) {
    if (s1.Long__MaxValue!=s2.Long__MaxValue)
        return false;
    if (s1.Long__MinValue!=s2.Long__MinValue)
        return false;
    return true;
};

inline bool operator!= (const Long_struct& s1, const Long_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Long_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Long_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Float_struct {
	Float_struct ()
	{
		Float__MaxValue = 1e10;
		Float__MinValue = -1e10;
	};

    std::string getId() {
        return std::string("Float");
    };
	
	float Float__MaxValue;
	float Float__MinValue;
};

inline bool operator>>= (const CORBA::Any& a, Float_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Float::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Float__MaxValue)) return false;
		}
		if (!strcmp("Float::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Float__MinValue)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Float_struct& s) {
	CF::Properties props;
	props.length(2);
	props[0].id = CORBA::string_dup("Float::MaxValue");
	props[0].value <<= s.Float__MaxValue;
	props[1].id = CORBA::string_dup("Float::MinValue");
	props[1].value <<= s.Float__MinValue;
	a <<= props;
};

inline bool operator== (const Float_struct& s1, const Float_struct& s2) {
    if (s1.Float__MaxValue!=s2.Float__MaxValue)
        return false;
    if (s1.Float__MinValue!=s2.Float__MinValue)
        return false;
    return true;
};

inline bool operator!= (const Float_struct& s1, const Float_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Float_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Float_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Double_struct {
	Double_struct ()
	{
		Double__MaxValue = 1e10;
		Double__MinValue = -1e10;
	};

    std::string getId() {
        return std::string("Double");
    };
	
	double Double__MaxValue;
	double Double__MinValue;
};

inline bool operator>>= (const CORBA::Any& a, Double_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Double::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Double__MaxValue)) return false;
		}
		if (!strcmp("Double::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Double__MinValue)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Double_struct& s) {
	CF::Properties props;
	props.length(2);
	props[0].id = CORBA::string_dup("Double::MaxValue");
	props[0].value <<= s.Double__MaxValue;
	props[1].id = CORBA::string_dup("Double::MinValue");
	props[1].value <<= s.Double__MinValue;
	a <<= props;
};

inline bool operator== (const Double_struct& s1, const Double_struct& s2) {
    if (s1.Double__MaxValue!=s2.Double__MaxValue)
        return false;
    if (s1.Double__MinValue!=s2.Double__MinValue)
        return false;
    return true;
};

inline bool operator!= (const Double_struct& s1, const Double_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Double_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Double_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Octet_out_struct {
	Octet_out_struct ()
	{
		Octet_out__MaxValue = 255;
		Octet_out__MinValue = 0;
		Octet_out__scale_enabled = false;
	};

    std::string getId() {
        return std::string("Octet_out");
    };
	
	unsigned char Octet_out__MaxValue;
	unsigned char Octet_out__MinValue;
	bool Octet_out__scale_enabled;
};

inline bool operator>>= (const CORBA::Any& a, Octet_out_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Octet_out::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= CORBA::Any::to_octet(s.Octet_out__MaxValue))) return false;
		}
		if (!strcmp("Octet_out::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= CORBA::Any::to_octet(s.Octet_out__MinValue))) return false;
		}
		if (!strcmp("Octet_out::scale_enabled", props[idx].id)) {
			if (!(props[idx].value >>= s.Octet_out__scale_enabled)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Octet_out_struct& s) {
	CF::Properties props;
	props.length(3);
	props[0].id = CORBA::string_dup("Octet_out::MaxValue");
	props[0].value <<= CORBA::Any::from_octet(s.Octet_out__MaxValue);
	props[1].id = CORBA::string_dup("Octet_out::MinValue");
	props[1].value <<= CORBA::Any::from_octet(s.Octet_out__MinValue);
	props[2].id = CORBA::string_dup("Octet_out::scale_enabled");
	props[2].value <<= s.Octet_out__scale_enabled;
	a <<= props;
};

inline bool operator== (const Octet_out_struct& s1, const Octet_out_struct& s2) {
    if (s1.Octet_out__MaxValue!=s2.Octet_out__MaxValue)
        return false;
    if (s1.Octet_out__MinValue!=s2.Octet_out__MinValue)
        return false;
    if (s1.Octet_out__scale_enabled!=s2.Octet_out__scale_enabled)
        return false;
    return true;
};

inline bool operator!= (const Octet_out_struct& s1, const Octet_out_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Octet_out_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Octet_out_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Ushort_out_struct {
	Ushort_out_struct ()
	{
		Ushort_out__MaxValue = 65535;
		Ushort_out__MinValue = 0;
		Ushort_out__scale_enabled = false;
	};

    std::string getId() {
        return std::string("Ushort_out");
    };
	
	unsigned short Ushort_out__MaxValue;
	unsigned short Ushort_out__MinValue;
	bool Ushort_out__scale_enabled;
};

inline bool operator>>= (const CORBA::Any& a, Ushort_out_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Ushort_out::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Ushort_out__MaxValue)) return false;
		}
		if (!strcmp("Ushort_out::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Ushort_out__MinValue)) return false;
		}
		if (!strcmp("Ushort_out::scale_enabled", props[idx].id)) {
			if (!(props[idx].value >>= s.Ushort_out__scale_enabled)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Ushort_out_struct& s) {
	CF::Properties props;
	props.length(3);
	props[0].id = CORBA::string_dup("Ushort_out::MaxValue");
	props[0].value <<= s.Ushort_out__MaxValue;
	props[1].id = CORBA::string_dup("Ushort_out::MinValue");
	props[1].value <<= s.Ushort_out__MinValue;
	props[2].id = CORBA::string_dup("Ushort_out::scale_enabled");
	props[2].value <<= s.Ushort_out__scale_enabled;
	a <<= props;
};

inline bool operator== (const Ushort_out_struct& s1, const Ushort_out_struct& s2) {
    if (s1.Ushort_out__MaxValue!=s2.Ushort_out__MaxValue)
        return false;
    if (s1.Ushort_out__MinValue!=s2.Ushort_out__MinValue)
        return false;
    if (s1.Ushort_out__scale_enabled!=s2.Ushort_out__scale_enabled)
        return false;
    return true;
};

inline bool operator!= (const Ushort_out_struct& s1, const Ushort_out_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Ushort_out_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Ushort_out_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Short_out_struct {
	Short_out_struct ()
	{
		Short_out__MaxValue = 32767;
		Short_out__MinValue = -32768;
		Short_out__scale_enabled = false;
	};

    std::string getId() {
        return std::string("Short_out");
    };
	
	short Short_out__MaxValue;
	short Short_out__MinValue;
	bool Short_out__scale_enabled;
};

inline bool operator>>= (const CORBA::Any& a, Short_out_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Short_out::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Short_out__MaxValue)) return false;
		}
		if (!strcmp("Short_out::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Short_out__MinValue)) return false;
		}
		if (!strcmp("Short_out::scale_enabled", props[idx].id)) {
			if (!(props[idx].value >>= s.Short_out__scale_enabled)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Short_out_struct& s) {
	CF::Properties props;
	props.length(3);
	props[0].id = CORBA::string_dup("Short_out::MaxValue");
	props[0].value <<= s.Short_out__MaxValue;
	props[1].id = CORBA::string_dup("Short_out::MinValue");
	props[1].value <<= s.Short_out__MinValue;
	props[2].id = CORBA::string_dup("Short_out::scale_enabled");
	props[2].value <<= s.Short_out__scale_enabled;
	a <<= props;
};

inline bool operator== (const Short_out_struct& s1, const Short_out_struct& s2) {
    if (s1.Short_out__MaxValue!=s2.Short_out__MaxValue)
        return false;
    if (s1.Short_out__MinValue!=s2.Short_out__MinValue)
        return false;
    if (s1.Short_out__scale_enabled!=s2.Short_out__scale_enabled)
        return false;
    return true;
};

inline bool operator!= (const Short_out_struct& s1, const Short_out_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Short_out_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Short_out_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Ulong_out_struct {
	Ulong_out_struct ()
	{
		Ulong_out__MaxValue = 4294967295;
		Ulong_out__MinValue = 0;
		Ulong_out__scale_enabled = false;
	};

    std::string getId() {
        return std::string("Ulong_out");
    };
	
	CORBA::ULong Ulong_out__MaxValue;
	CORBA::ULong Ulong_out__MinValue;
	bool Ulong_out__scale_enabled;
};

inline bool operator>>= (const CORBA::Any& a, Ulong_out_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Ulong_out::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Ulong_out__MaxValue)) return false;
		}
		if (!strcmp("Ulong_out::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Ulong_out__MinValue)) return false;
		}
		if (!strcmp("Ulong_out::scale_enabled", props[idx].id)) {
			if (!(props[idx].value >>= s.Ulong_out__scale_enabled)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Ulong_out_struct& s) {
	CF::Properties props;
	props.length(3);
	props[0].id = CORBA::string_dup("Ulong_out::MaxValue");
	props[0].value <<= s.Ulong_out__MaxValue;
	props[1].id = CORBA::string_dup("Ulong_out::MinValue");
	props[1].value <<= s.Ulong_out__MinValue;
	props[2].id = CORBA::string_dup("Ulong_out::scale_enabled");
	props[2].value <<= s.Ulong_out__scale_enabled;
	a <<= props;
};

inline bool operator== (const Ulong_out_struct& s1, const Ulong_out_struct& s2) {
    if (s1.Ulong_out__MaxValue!=s2.Ulong_out__MaxValue)
        return false;
    if (s1.Ulong_out__MinValue!=s2.Ulong_out__MinValue)
        return false;
    if (s1.Ulong_out__scale_enabled!=s2.Ulong_out__scale_enabled)
        return false;
    return true;
};

inline bool operator!= (const Ulong_out_struct& s1, const Ulong_out_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Ulong_out_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Ulong_out_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Long_out_struct {
	Long_out_struct ()
	{
		Long_out__MaxValue = 2147483647;
		Long_out__MinValue = -2147483648;
		Long_out__scale_enabled = false;
	};

    std::string getId() {
        return std::string("Long_out");
    };
	
	CORBA::Long Long_out__MaxValue;
	CORBA::Long Long_out__MinValue;
	bool Long_out__scale_enabled;
};

inline bool operator>>= (const CORBA::Any& a, Long_out_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Long_out::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Long_out__MaxValue)) return false;
		}
		if (!strcmp("Long_out::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Long_out__MinValue)) return false;
		}
		if (!strcmp("Long_out::scale_enabled", props[idx].id)) {
			if (!(props[idx].value >>= s.Long_out__scale_enabled)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Long_out_struct& s) {
	CF::Properties props;
	props.length(3);
	props[0].id = CORBA::string_dup("Long_out::MaxValue");
	props[0].value <<= s.Long_out__MaxValue;
	props[1].id = CORBA::string_dup("Long_out::MinValue");
	props[1].value <<= s.Long_out__MinValue;
	props[2].id = CORBA::string_dup("Long_out::scale_enabled");
	props[2].value <<= s.Long_out__scale_enabled;
	a <<= props;
};

inline bool operator== (const Long_out_struct& s1, const Long_out_struct& s2) {
    if (s1.Long_out__MaxValue!=s2.Long_out__MaxValue)
        return false;
    if (s1.Long_out__MinValue!=s2.Long_out__MinValue)
        return false;
    if (s1.Long_out__scale_enabled!=s2.Long_out__scale_enabled)
        return false;
    return true;
};

inline bool operator!= (const Long_out_struct& s1, const Long_out_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Long_out_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Long_out_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Float_out_struct {
	Float_out_struct ()
	{
		Float_out__MaxValue = 1e10;
		Float_out__MinValue = -1e10;
		Float_out__scale_enabled = false;
	};

    std::string getId() {
        return std::string("Float_out");
    };
	
	float Float_out__MaxValue;
	float Float_out__MinValue;
	bool Float_out__scale_enabled;
};

inline bool operator>>= (const CORBA::Any& a, Float_out_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Float_out::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Float_out__MaxValue)) return false;
		}
		if (!strcmp("Float_out::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Float_out__MinValue)) return false;
		}
		if (!strcmp("Float_out::scale_enabled", props[idx].id)) {
			if (!(props[idx].value >>= s.Float_out__scale_enabled)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Float_out_struct& s) {
	CF::Properties props;
	props.length(3);
	props[0].id = CORBA::string_dup("Float_out::MaxValue");
	props[0].value <<= s.Float_out__MaxValue;
	props[1].id = CORBA::string_dup("Float_out::MinValue");
	props[1].value <<= s.Float_out__MinValue;
	props[2].id = CORBA::string_dup("Float_out::scale_enabled");
	props[2].value <<= s.Float_out__scale_enabled;
	a <<= props;
};

inline bool operator== (const Float_out_struct& s1, const Float_out_struct& s2) {
    if (s1.Float_out__MaxValue!=s2.Float_out__MaxValue)
        return false;
    if (s1.Float_out__MinValue!=s2.Float_out__MinValue)
        return false;
    if (s1.Float_out__scale_enabled!=s2.Float_out__scale_enabled)
        return false;
    return true;
};

inline bool operator!= (const Float_out_struct& s1, const Float_out_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Float_out_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Float_out_struct tmp;
    if (fromAny(a, tmp)) {
        if (tmp != this->value_) {
            return 1;
        }

        return 0;
    } else {
        return 1;
    }
}

struct Double_out_struct {
	Double_out_struct ()
	{
		Double_out__MaxValue = 1e10;
		Double_out__MinValue = -1e10;
		Double_out__scale_enabled = false;
	};

    std::string getId() {
        return std::string("Double_out");
    };
	
	double Double_out__MaxValue;
	double Double_out__MinValue;
	bool Double_out__scale_enabled;
};

inline bool operator>>= (const CORBA::Any& a, Double_out_struct& s) {
	CF::Properties* temp;
	if (!(a >>= temp)) return false;
	CF::Properties& props = *temp;
	for (unsigned int idx = 0; idx < props.length(); idx++) {
		if (!strcmp("Double_out::MaxValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Double_out__MaxValue)) return false;
		}
		if (!strcmp("Double_out::MinValue", props[idx].id)) {
			if (!(props[idx].value >>= s.Double_out__MinValue)) return false;
		}
		if (!strcmp("Double_out::scale_enabled", props[idx].id)) {
			if (!(props[idx].value >>= s.Double_out__scale_enabled)) return false;
		}
	}
	return true;
};

inline void operator<<= (CORBA::Any& a, const Double_out_struct& s) {
	CF::Properties props;
	props.length(3);
	props[0].id = CORBA::string_dup("Double_out::MaxValue");
	props[0].value <<= s.Double_out__MaxValue;
	props[1].id = CORBA::string_dup("Double_out::MinValue");
	props[1].value <<= s.Double_out__MinValue;
	props[2].id = CORBA::string_dup("Double_out::scale_enabled");
	props[2].value <<= s.Double_out__scale_enabled;
	a <<= props;
};

inline bool operator== (const Double_out_struct& s1, const Double_out_struct& s2) {
    if (s1.Double_out__MaxValue!=s2.Double_out__MaxValue)
        return false;
    if (s1.Double_out__MinValue!=s2.Double_out__MinValue)
        return false;
    if (s1.Double_out__scale_enabled!=s2.Double_out__scale_enabled)
        return false;
    return true;
};

inline bool operator!= (const Double_out_struct& s1, const Double_out_struct& s2) {
    return !(s1==s2);
};

template<> inline short StructProperty<Double_out_struct>::compare (const CORBA::Any& a) {
    if (super::isNil_) {
        if (a.type()->kind() == (CORBA::tk_null)) {
            return 0;
        }
        return 1;
    }

    Double_out_struct tmp;
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
