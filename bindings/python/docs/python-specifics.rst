Python-specific Notes
==============================

The following are brief notes on Python-specific binding features and how the Libaudioverse API maps to Python.

Comparing Objects
--------------------

Libaudioverse objects are thin proxies over handles, with their global state stored elsewhere in the Libaudioverse module.
Consequently, you need to use `==`, not `is`.
`is` will work sometimes, but no guarantee is made that two node objects in different variables are the same proxy instance.

Properties
--------------------

Properties are bound as Python properties, set up so that you can simply set them as normal: `sine.frequency = 32.5`.
In addition, the objects representing numeric properties overload all operators expected for their types.
The `value` attribute also exists on all properties.
It is most useful to get the value of other property types (namely string and integer properties set from enums).

Some old code uses the `.value` suffix for all operations on the right-hand side of assignment, as well as on the left-hand side of compound assignment operators.
For example, `sine.frequency = sine.frequency.value + 2` or `sine.frequency.value += 2`.
This is no longer necessary because numeric properties now behave like numbers.

Note the following important points:

- Properties do not implement `__hash__` because there is no sensible definition that can satisfy all cases.

- Storing the property proxy object keeps the node it came from alive.  If you need to store values of properties, it is much more efficient to extract the value.  `x = sine.frequency` keeps the sine node alive at least until `x` dies, but `x = sine.frequency.value` doesn't.

- If you store a property object, changing it (i.e. assignment, `*=`, calling `reset`) will affect the node.

Callbacks
--------------------

Callbacks are bound as `node.set_XXX_callback` for setting and `node.get_XXX_callback` from getting.
The getter will always return the same callable as passed to the setter.

The signature of callbacks is specified in the reference.
Python matches the C signature, save for the userdata argument.
Instead, you can pass any number of extra arguments to the setter.
Note that the first `n` positional arguments need to match what Libaudioverse expects, otherwise the C bindings will crash and burn inside the callback.

Python holds a strong reference to the last set callback object until Libaudioverse deletes the handle.
You can therefore use any callable safely.


Atomicity and Simulation Locking
----------------------------------------

instead of binding `Lav_simulationLock` and `Lav_simulationUnlock` directly, the simulation is a context manager.
Using a with statement will make everything inside it inaudible until the with statement ends.
They may be nested safely.

For example::

   with my_simulation:
       # some stuff

Note that all the concerns in the language-agnostic manual apply.
If you use the simulation as a context manager on more than one thread, it behaves exactly like a lock; furthermore, all Libaudioverse functions that use the simulation or objects created from it will block on any thread but the one currently inside the critical section.

name Conversion
----------------

The names for Libaudioverse objects can be seen in the API reference.
This section is provided in order to aid in translating examples in C to Python.
This module follows PEP8 in the external API.
The code decidedly does not, as it is generated from custom tools.

Python class names  are generated by taking the `Lav_OBJTYPE_FOOBAR` enum constants, stripping the `Lav_OBJTYPE_` prefix, and converting the rest to camelcase.

Enumerations are generated in a similar manner as objects: strip the `Lav_` prefix and convert the rest to camelcase.
The members have their common prefix stripped and are then converted to lowercase, in accordance with the PEP8 class member standards.
