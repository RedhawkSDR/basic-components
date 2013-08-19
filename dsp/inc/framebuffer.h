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

#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <vector>
#include <boost/thread/mutex.hpp>

template <typename iterT>
class framebuffer {
	//Class to buffer data for you and frame it up
	//you get consistant frame sizes out no matter how you push input in
	//furthermore - you can choose to overlap or skip data for the frames
	//if you set overlap>0 then you overlap output data
	//if you set overlap<0 then you throw data away
	//this class works with iterators so that data copies
	//are reduced to an absolute minimium

public:
	framebuffer(size_t frameSize, long overlap=0);
	virtual ~framebuffer();

	typedef typename iterT::value_type valueT;
	typedef typename std::vector<valueT> vectorT;

	struct frame
	{
		iterT begin;
		iterT end;
	};
	//call this method in a loop to frame up your data
	//output is a vector of data frames for you to operate on
	void newData(frame input,std::vector<frame>& output);

	//convienience function for vectors - but the same as
	//the other newData call
	void newData(vectorT& input,std::vector<frame>& output);

	//modify the frame parameters
	void setFrameSize(size_t frameSize);
	void setOverlap(long overlap);

private:
	void updateInternals();
	size_t getNumFrames(size_t dataElements);
	vectorT vecA_;
	vectorT vecB_;
	vectorT* last_;
	vectorT* next_;

	size_t frameSize_;
	long overlap_;

	size_t stride_;
	size_t throwAwayIndex_;

	boost::mutex boostLock_;

};

#endif /* FRAMEBUFFER_H_ */
