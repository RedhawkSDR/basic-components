/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
 * source distribution.
 * 
 * This file is part of REDHAWK Basic Components sourcesocket.
 * 
 * REDHAWK Basic Components sourcesocket is free software: you can redistribute it and/or modify it under the terms of 
 * the GNU Lesser General Public License as published by the Free Software Foundation, either 
 * version 3 of the License, or (at your option) any later version.
 * 
 * REDHAWK Basic Components sourcesocket is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License along with this 
 * program.  If not, see http://www.gnu.org/licenses/.
 */

#ifndef VECTORSWAP_H_
#define VECTORSWAP_H_

#include <byteswap.h>
#include <vector>
#include <algorithm>

//in place byte swap
template<typename T, typename U> void vectorSwap(std::vector<T, U>& dataVec, const unsigned char numBytes)
{
	if (numBytes>1)
	{
		size_t totalBytes = dataVec.size()*sizeof(T);
		assert(totalBytes%numBytes==0);
		size_t numSwap = totalBytes/numBytes;
		if (numBytes==2)
		{
			unsigned short* p = reinterpret_cast< unsigned short* >(&dataVec[0]);
			for (size_t i=0; i!=numSwap; i++)
			{
				*p = bswap_16(*p);
				p++;
			}
		} else if (numBytes==4)
		{
			unsigned int* p = reinterpret_cast< unsigned int* >(&dataVec[0]);
			for (size_t i=0; i!=numSwap; i++)
			{
				*p = bswap_32(*p);
				p++;
			}
		} else if(numBytes==8)
		{
			unsigned long* p = reinterpret_cast< unsigned long* >(&dataVec[0]);
			for (size_t i=0; i!=numSwap; i++)
			{
				*p = bswap_64(*p);
				p++;
			}
		}else
		{
			//explicitly swap all the bytes out by hand if we don't have a good optimized macro available for us
			char* next = reinterpret_cast< char* >(&dataVec[0]);
			char* first;
			char* second;
			for (size_t i=0; i!=numSwap; i++)
			{
				first = next;
				next = first+numBytes;
				second = next;
				std::reverse(first, second);
			}
		}
	}
}

//non in place vector byte swap
template<typename T, typename U> void vectorSwap(char* data, std::vector<T, U>& outVec, const unsigned char numBytes)
{
	if (numBytes>1)
	{
		size_t totalBytes = outVec.size()*sizeof(T);
		assert(totalBytes%numBytes==0);
		size_t numSwap = totalBytes/numBytes;
		if (numBytes==2)
		{
			unsigned short* from = reinterpret_cast< unsigned short* >(data);
			unsigned short* to = reinterpret_cast< unsigned short* >(&outVec[0]);
			for (size_t i=0; i!=numSwap; i++)
			{
				*to = bswap_16(*from);
				to++;
				from++;
			}
		} else if (numBytes==4)
		{
			unsigned int* from = reinterpret_cast< unsigned int* >(data);
			unsigned int* to = reinterpret_cast< unsigned int* >(&outVec[0]);
			for (size_t i=0; i!=numSwap; i++)
			{
				*to = bswap_32(*from);
				to++;
				from++;
			}
		} else if(numBytes==8)
		{
			unsigned long* from = reinterpret_cast< unsigned long* >(data);
			unsigned long* to = reinterpret_cast< unsigned long* >(&outVec[0]);
			for (size_t i=0; i!=numSwap; i++)
			{
				*to = bswap_64(*from);
				to++;
				from++;
			}
		}else
		{
			//explicitly swap all the bytes out by hand if we don't have a good optimized macro available for us
			char* next = reinterpret_cast< char* >(data);
			char* first;
			char* to = reinterpret_cast< char* >(&outVec[0]);
			for (size_t i=0; i!=numSwap; i++)
			{
				first = next;
				next+=numBytes;
				std::reverse_copy(first, next, to);
				to+=numBytes;
			}
		}
	}
}


#endif /* VECTORSWAP_H_ */
