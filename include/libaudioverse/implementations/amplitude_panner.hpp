/**Copyright (C) Austin Hicks, 2014-2016
This file is part of Libaudioverse, a library for realtime audio applications.
This code is dual-licensed.  It is released under the terms of the Mozilla Public License version 2.0 or the Gnu General Public License version 3 or later.
You may use this code under the terms of either license at your option.
A copy of both licenses may be found in license.gpl and license.mpl at the root of this repository.
If these files are unavailable to you, see either http://www.gnu.org/licenses/ (GPL V3 or later) or https://www.mozilla.org/en-US/MPL/2.0/ (MPL 2.0).*/
#pragma once
#include <vector>

namespace libaudioverse_implementation {

//An n-channel panner.
//very very private. This does almost no error checking.

struct AmplitudePannerEntry {
	AmplitudePannerEntry(float _angle, unsigned int _c): angle(_angle), channel(_c) {}
	float angle;
	int channel;
};

class AmplitudePanner {
	public:
	AmplitudePanner(int _block_size, float _sr);
	void clearMap();
	void addEntry(float angle, int channel);
	void pan(float* input, float** outputs);
	void readMap(int entries, float* map);
	float getAzimuth();
	void setAzimuth(float a);
	float getElevation();
	void setElevation(float e);
	private:
	std::vector<AmplitudePannerEntry> channels;
	float azimuth = 0.0f, elevation = 0.0f;
	float sr;
	int block_size;
};

}