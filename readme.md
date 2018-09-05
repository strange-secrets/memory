nGen Memory
===========
Cross platform C++ library for managing memory allocations.

nGen is a game library split into separate modules to allow for code to be easily reused or rewritten.
When writing games in C++ it is extremely important to maintain control over the behaviour of your applications
memory. Failure to do so can lead to fragmentation and bugs that are extremely difficult to track down.

To begin with this library provides fairly simple memory management but can be extended over time to
add more complex behaviour.

When creating a video game, it is essential to maintain control of memory and its behaviour. Even on a platform with
virtual memory we should keep a tight reign on how much memory we are allocating. Simply relying on the standard
allocator can allow memory usage to spiral out of control, or fragmentation of the memory space.

ngen::Memory is intended to provide a pooled memory allocation set to help contain memory and detect issues with
our memory usage. 

Allocation
==========
Memory is allocated using the NGEN_NEW macro, which requires the user to supply the memory heap they
wish the allocation to obtain memory from.
Using macro's in this way provides a number of benefits over directly overriding operator new.

1) Other libraries may override operator new, using the macro helps avoid conflicts.
2) The macro can be changed to easily change the allocation behaviour across the entire codebase.
3) The macro has debug and non-debug definitions to allow for file and line numbers to be logged automatically.

