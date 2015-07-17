/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#pragma once
#include "../libaudioverse.h"
#include <vector>
#include <set>
#include <memory>
#include <glm/glm.hpp>

namespace libaudioverse_implementation {

class SourceNode;
class HrtfData;
class Simulation;
class Buffer;

/**This holds info on listener positions, defaults, etc.
Anything a source needs for updating, basically.*/
class EnvironmentInfo {
	public:
	glm::mat4 world_to_listener_transform;
};

class EnvironmentNode: public SubgraphNode {
	public:
	EnvironmentNode(std::shared_ptr<Simulation> simulation, std::shared_ptr<HrtfData> hrtf);
	void registerSourceForUpdates(std::shared_ptr<SourceNode> source);
	//call update on all sources.
	virtual void willProcessParents();
	std::shared_ptr<Node> createPannerNode();
	virtual void outputChannelsChanged();
	//Play buffer asynchronously at specified position, destroying the source when done.
	void playAsync(std::shared_ptr<Buffer> buffer, float x, float y, float z);
	private:
	//while these may be parents (through virtue of the panners we give out), they also have to hold a reference to us-and that reference must be strong.
	//the world is more capable of handling a source that dies than a source a world that dies.
	std::set<std::weak_ptr<SourceNode>, std::owner_less<std::weak_ptr<SourceNode>>> sources;
	std::shared_ptr<HrtfData > hrtf;
	std::shared_ptr<Node> output=nullptr;
	EnvironmentInfo environment_info;
};

}