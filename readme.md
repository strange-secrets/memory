nGen Memory
===========
Cross platform C++ library for managing memory allocations.

nGen is a game library split into separate modules to allow for code to be easily reused or rewritten.
When writing games in C++ it is extremely important to maintain control over the behaviour of your applications
memory. Failure to do so can lead to fragmentation and bugs that are extremely difficult to track down.

To begin with this library provides fairly simple memory management but can be extended over time to
add more complex behaviour.


Allocation
==========
Memory is allocated using the NGEN_NEW macro, which requires the user to supply the memory heap they
wish the allocation to obtain memory from.
Using macro's in this way provides a number of benefits over directly overriding operator new.

1) Other libraries may override operator new, using the macro helps avoid conflicts.
2) The macro can be changed to easily change the allocation behaviour across the entire codebase.
3) The macro has debug and non-debug definitions to allow for file and line numbers to be logged automatically.

