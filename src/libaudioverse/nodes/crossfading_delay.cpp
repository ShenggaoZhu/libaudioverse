/**Copyright (C) Austin Hicks, 2014-2016
This file is part of Libaudioverse, a library for realtime audio applications.
This code is dual-licensed.  It is released under the terms of the Mozilla Public License version 2.0 or the Gnu General Public License version 3 or later.
You may use this code under the terms of either license at your option.
A copy of both licenses may be found in license.gpl and license.mpl at the root of this repository.
If these files are unavailable to you, see either http://www.gnu.org/licenses/ (GPL V3 or later) or https://www.mozilla.org/en-US/MPL/2.0/ (MPL 2.0).*/
#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/libaudioverse_properties.h>
#include <libaudioverse/nodes/crossfading_delay.hpp>
#include <libaudioverse/private/server.hpp>
#include <libaudioverse/private/node.hpp>
#include <libaudioverse/private/properties.hpp>
#include <libaudioverse/private/macros.hpp>
#include <libaudioverse/private/memory.hpp>
#include <libaudioverse/implementations/delayline.hpp>
#include <memory>

namespace libaudioverse_implementation {

CrossfadingDelayNode::CrossfadingDelayNode(std::shared_ptr<Server> server, float maxDelay, int channels): Node(Lav_OBJTYPE_CROSSFADING_DELAY_NODE, server, channels, channels) {
	if(channels <= 0) ERROR(Lav_ERROR_RANGE, "lineCount must be greater than 0.");
	this->channels = channels;
	lines = new CrossfadingDelayLine*[channels];
	for(unsigned int i = 0; i < channels; i++) lines[i] = new CrossfadingDelayLine(maxDelay, server->getSr());
	getProperty(Lav_DELAY_DELAY).setFloatRange(0.0f, maxDelay);
	getProperty(Lav_DELAY_DELAY_MAX).setFloatValue(maxDelay);
	appendInputConnection(0, channels);
	appendOutputConnection(0, channels);
}

std::shared_ptr<Node> createCrossfadingDelayNode(std::shared_ptr<Server> server, float maxDelay, unsigned int channels) {
	return standardNodeCreation<CrossfadingDelayNode>(server, maxDelay, channels);
}

CrossfadingDelayNode::~CrossfadingDelayNode() {
	for(int i=0; i < channels; i++) delete lines[i];
	delete[] lines;
}

void CrossfadingDelayNode::recomputeDelta() {
	float time = getProperty(Lav_DELAY_INTERPOLATION_TIME).getFloatValue();
	for(int i = 0; i < channels; i++) lines[i]->setInterpolationTime(time);
}

void CrossfadingDelayNode::delayChanged() {
	float newDelay = getProperty(Lav_DELAY_DELAY).getFloatValue();
	for(int i = 0; i < channels; i++) lines[i]->setDelay(newDelay);
}

void CrossfadingDelayNode::process() {
	if(werePropertiesModified(this, Lav_DELAY_DELAY)) delayChanged();
	if(werePropertiesModified(this, Lav_DELAY_INTERPOLATION_TIME)) recomputeDelta();
	float feedback = getProperty(Lav_DELAY_FEEDBACK).getFloatValue();
	//optimize the common case of not having feedback.
	//the only difference between these blocks is in the advance line.
	if(feedback == 0.0f) {
		for(unsigned int output = 0; output < num_output_buffers; output++) {
			auto &line = *lines[output];
			line.processBuffer(block_size, input_buffers[output], output_buffers[output]);
		}
	}
	else {
		for(unsigned int output = 0; output < num_output_buffers; output++) {
			auto &line = *lines[output];
			for(unsigned int i = 0; i < block_size; i++) {
				output_buffers[output][i] = line.computeSample();
				line.advance(input_buffers[output][i]+output_buffers[output][i]*feedback);
			}
		}
	}
}

//begin public api
Lav_PUBLIC_FUNCTION LavError Lav_createCrossfadingDelayNode(LavHandle serverHandle, float maxDelay, int channels, LavHandle* destination) {
	PUB_BEGIN
	auto server =incomingObject<Server>(serverHandle);
	LOCK(*server);
	auto d = createCrossfadingDelayNode(server, maxDelay, channels);
	*destination = outgoingObject(d);
	PUB_END
}

}