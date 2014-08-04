/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/private_physical_outputs.hpp>
#include <string>
#include <vector>
#include <string>
#include <memory>
#include <utility>

/**Code common to all backends, i.e. enumeration.*/

LavPhysicalOutput::LavPhysicalOutput(unsigned int bufferSize, unsigned int mixAhead): buffer_size(bufferSize), mix_ahead(mixAhead) {
}

void LavPhysicalOutput::setCallback(std::function<void(LavPhysicalOutput*, float*)> what) {
	audio_callback = what;
};

unsigned int LavPhysicalOutput::getBufferSize() {
	return buffer_size;
}
