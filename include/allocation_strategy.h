
#if !defined(ALLOCATION_STRATEGY_HEADER_INCLUDED_STRANGE_SECRETS)
#define ALLOCATION_STRATEGY_HEADER_INCLUDED_STRANGE_SECRETS

////////////////////////////////////////////////////////////////////////////

namespace ngen::memory {
    //! \brief  Enumeration defining the various free-block selection algorithms that are supported.
    //!
    //! When an allocation request is made, the heap searches its list of available memory blocks to find a suitable
    //! place where the requested memory can be allocated from. This enumeration specifies which type of selection
    //! heuristic is applied when searching.
    enum class kAllocationStrategy {
        Invalid,

        //! \brief  Chooses the first free block encountered that has the requested number of bytes available.
        First,

        //! \brief  Scans all free blocks and selects the smallest free block that has the number of requested bytes available.
        Smallest
    };
}

////////////////////////////////////////////////////////////////////////////

#endif //!defined(ALLOCATION_STRATEGY_HEADER_INCLUDED_STRANGE_SECRETS)
