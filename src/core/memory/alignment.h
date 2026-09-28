#pragma once

/// Expected API
/// AlignPtr(uintptr_t addr, func -> AlignAdress : x -> size_t)

#include <cstdint>
#include <cassert>


/// Shift the given address upwards if/as necessary to
/// ensure it is aligned to the given number of bytes.
inline uintptr_t AlignAddress(uintptr_t addr, size_t align)
{
    const size_t mask = align - 1;
    assert((align & mask) == 0);    // pwr of 2
    return (addr + mask) & ~mask;
}

inline uintptr_t ALignAdressAlways(uintptr_t addr, size_t align)
{
    assert((align & (align - 1)) == 0);
    return (addr + align) & ~(align - 1);
}

/// Shif the given pointer upwards if/as necessary to
// ensure it is aligned to the given number of bytes.
template<typename T>
inline T* AlignPtr(T* ptr, size_t align)
{
    const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
    const uintptr_t addrAligned = AlignAddress(addr, align);
    return reinterpret_cast<T*>(addrALigned);
}

/// Aligned allocation function
/// @note 'align' must be a pwoer of 2 (typically 4, 8, 16)
void* AllocAligned(size_t bytes, size_t align)
{
    // Determine worst case bymber of bytes we'll need.
    size_t actualBytes = bytes + align;

    // Allocate unligned block
    uint8_t* pRawMem = new uint8_t[actualBytes];

    // Align the block. If no alignment occurred,
    // shift it up the full 'align' bytes so we
    // always have room to store the shift
    uint8_t* pAlignedMem = AlignPtr(pRawMem, align);
    if (pAlignedMem == pRawMem)
    {
        pAlignedMem += align;
    }

    // Determine the shift, and store it.
    // (This works for up to 256-bytes alignment)
    ptrdiff_t shift = pAlignedMem - pRawMem;
    assert(shift > 0 && shift <= 256);
    pAlignedMem[-1] = static_cast<uint8_t>(shift & 0xFF);

    return pAlignedMem;
}

void FreeAligned(void* pMem)
{
    assert(pMem);

    uint8_t* pAlignedMem = reinterpret_cast<uint8_t*>(pMem);

    ptrdiff_t shift = pAlignedMem[-1];
    shift = ((shift - 1) & 0xFF) + 1;

    uint8_t* pRawMem = pAlignedMem - shift;
    delete[] pRawMem;
}