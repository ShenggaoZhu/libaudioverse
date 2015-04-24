/**Copyright (C) Austin Hicks, 2014
This file is part of Libaudioverse, a library for 3D and environmental audio simulation, and is released under the terms of the Gnu General Public License Version 3 or (at your option) any later version.
A copy of the GPL, as well as other important copyright and licensing information, may be found in the file 'LICENSE' in the root of the Libaudioverse repository.  Should this file be missing or unavailable to you, see <http://www.gnu.org/licenses/>.*/
#pragma once

class LavProperty;

/**The automators.

The name for this component is borrowed from Webaudio.  This file only contains the abstract definition.

Adding new automators is similar to nodes: create a file, define the class, create a creation function.

We put the creeation functions for automators in this file.

An automator itself must be equivalent a pure function with some parameters. bound.
Put another way, multiple calls to start *must* be supported.
Furthermore, all automators must have a well-defined final value.

Subclasses should set duration in their constructors, should they need to continue past their scheduled time.*/

class LavAutomator {
	public:
	LavAutomator(LavProperty* p, double endTime);
	virtual ~LavAutomator();

	//default implementation: initial_value and initial_time are set.
	virtual void start(double initialValue, double initialTime) ;
	virtual double getValueAtTime(double time) = 0;
	virtual double getFinalValue() = 0;
	virtual double getDuration();

	protected:
	double initial_value = 0.0, initial_time = 0.0, end_time = 0.0, duration = 0.0;
	LavProperty* property = nullptr;
};
