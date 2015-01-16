/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#pragma once
#include <lambdatask/threadsafe_queue.hpp>
#include <functional> //we have to use an std::function for the preprocessing hook.  There's no good way around it because worlds need to use capturing lambdas.
#include <set>
#include <vector>
#include <mutex>
#include <memory>
#include <thread>
#include <tuple>
#include <map>
#include <random>
#include "../libaudioverse.h"

class LavNode;
class LavDevice;

/*When thrown on the background thread, terminates it.*/
class LavThreadTerminationException {
};

class LavSimulation {
	public:
	LavSimulation(unsigned int sr, unsigned int blockSize, unsigned int mixahead);
	virtual ~LavSimulation();
	virtual void getBlock(float* out, unsigned int channels, bool mayApplyMixingMatrix = true);
	//this is in frames of audio data.
	virtual unsigned int getBlockSize() { return block_size;}
	virtual LavError start();
	virtual LavError stop();
	virtual LavError associateNode(std::shared_ptr<LavNode> node);
	virtual std::shared_ptr<LavNode> getOutputNode();
	virtual LavError setOutputNode(std::shared_ptr<LavNode> node);
	virtual float getSr() { return sr;}
	virtual int getTickCount() {return tick_count;}

	//this is called whenever the graph changes.
	void invalidatePlan();

	//these make us meet the basic lockable concept.
	void lock() {mutex.lock();}
	void unlock() {mutex.unlock();}

	//Tasks that need to run in the background.
	void enqueueTask(std::function<void(void)>);

	//makes this device hold a shared pointer to its output.
	void associateDevice(std::shared_ptr<LavDevice> what);

	//register a mixing matrix with this device.
	void registerMixingMatrix(unsigned int inChannels, unsigned int outChannels, float* matrix);
	void resetMixingMatrix(unsigned int inChannels, unsigned int outChannels);
	void registerDefaultMixingMatrices();
	float* getMixingMatrix(unsigned int inChannels, unsigned int outChannels);


	protected:
	//reexecute planning logic.
	void replan();
	//visit all nodes in the order they need to be visited if we were processing the graph.
	virtual void visitAllNodesInProcessOrder(std::function<void(std::shared_ptr<LavNode>)> visitor);
	//visit all nodes in the order they must be visited to prepare for and process obj for a block of audio.  This is not the same as all parents: this call respects suspended.
	virtual void visitForProcessing(std::shared_ptr<LavNode> obj, std::function<void(std::shared_ptr<LavNode>)> visitor);
	std::function<void(void)> preprocessing_hook;
	//the execution plan.
	std::vector<std::shared_ptr<LavNode>> plan;
	std::vector<std::weak_ptr<LavNode>> weak_plan;
	unsigned int block_size = 0, mixahead = 0, is_started = 0;
	float sr = 0.0f;
	//if nodes die, they automatically need to be removed.  We can do said removal on next process.
	std::set<std::weak_ptr<LavNode>, std::owner_less<std::weak_ptr<LavNode>>> nodes;
	std::shared_ptr<LavNode> output_node = nullptr;
	std::recursive_mutex mutex;

	lambdatask::ThreadsafeQueue<std::function<void(void)>>  tasks;
	std::thread backgroundTaskThread;
	void backgroundTaskThreadFunction();

	//our output, if any.
	std::shared_ptr<LavDevice> device = nullptr;

	//the field to mark plans as invalid.
	bool planInvalidated = true;

	//the registered mixing matrices for this simulation.
	std::map<std::tuple<unsigned int, unsigned int>, float*> mixing_matrices;
	unsigned int largest_seen_mixing_matrix_input = 0;
	//used to apply mixing matrices when downmixing.
	float* mixing_matrix_workspace = nullptr;
	int tick_count = 0; //counts ticks.  This is part of node processing.
};