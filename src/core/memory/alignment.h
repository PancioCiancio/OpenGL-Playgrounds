///

#pragma once

#include <cstdint>
#include <cassert>
#include <algorithm>

// My Version ================================

inline uintptr_t AlignPtr(uintptr_t ptr, size_t alignment)
{
    const size_t mask = alignment - 1;
    assert((alignment & mask) == 0);    // Power of 2
    return (ptr + mask) & ~mask;
}

// inline uintptr_t AlignAddress(uintptr_t addr, size_t align)
// {
//     const size_t mask = align - 1;
//     assert((align & mask) == 0);    // pwr of 2
//     return (addr + mask) & ~mask;
// }

// inline uintptr_t AlignAdressAlways(uintptr_t addr, size_t align)
// {
//     assert((align & (align - 1)) == 0);
//     return (addr + align) & ~(align - 1);
// }

// template<typename T>
// inline T* AlignPtr(T* ptr, size_t align)
// {
//     const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
//     const uintptr_t addrAligned = AlignAddress(addr, align);
//     return reinterpret_cast<T*>(addrALigned);
// }

// void* AllocAligned(size_t bytes, size_t align)
// {
//     // Determine worst case bymber of bytes we'll need.
//     size_t actualBytes = bytes + align;

//     // Allocate unligned block
//     uint8_t* pRawMem = new uint8_t[actualBytes];

//     // Align the block. If no alignment occurred,
//     // shift it up the full 'align' bytes so we
//     // always have room to store the shift
//     uint8_t* pAlignedMem = AlignPtr(pRawMem, align);
//     if (pAlignedMem == pRawMem)
//     {
//         pAlignedMem += align;
//     }

//     // Determine the shift, and store it.
//     // (This works for up to 256-bytes alignment)
//     ptrdiff_t shift = pAlignedMem - pRawMem;
//     assert(shift > 0 && shift <= 256);
//     pAlignedMem[-1] = static_cast<uint8_t>(shift & 0xFF);

//     return pAlignedMem;
// }

// void FreeAligned(void* pMem)
// {
//     assert(pMem);

//     uint8_t* pAlignedMem = reinterpret_cast<uint8_t*>(pMem);

//     ptrdiff_t shift = pAlignedMem[-1];
//     shift = ((shift - 1) & 0xFF) + 1;

//     uint8_t* pRawMem = pAlignedMem - shift;
//     delete[] pRawMem;
// }

// // =============================================================

// inline void* AllocAligned(const size_t size, const size_t alignment)
// {
//     // Enforce minimum alignment to safely store our metadata pointer
//     const size_t newAlignment = std::max(alignment, sizeof(void*));

//     // Ensure alignment is power of 2
//     assert((newAlignment & (newAlignment - 1)) == 0);

//     // Total space = 
//     // required size + max possible padding + space for the stored pointer
//     size_t totalBytes = size + newAlignment + sizeof(void*);

//     void* pRawMem = malloc(totalBytes);
//     assert(pRawMem);

//     // 1. Start with the raw address
//     uintptr_t rawAddress = reinterpret_cast<uintptr_t>(pRawMem);

//     // 2. Add enough space to ensure we can store the original pointer behind the data
//     uintptr_t dataAddress = rawAddress + sizeof(void*);

//     // 3. Push the address forward to the next alignment boudary
//     uintptr_t alignedAddress = (dataAddress + newAlignment - 1) & ~(newAlignment - 1);

//     // 4. Step backward by exactly one pointer size and store the raw address
//     void** pMetaData = reinterpret_cast<void**>(alignedAddress) - 1;
//     *pMetaData = pRawMem;

//     // 5. Return the aligned memory block
//     return reinterpret_cast<void*>(alignedAddress);
// }

// inline void FreeAligned(void* pAlignedMem)
// {
//     assert(pAlignedMem);

//     // 1. Step backward by one pointer size to find the metadata
//     void** pMetaData = reinterpret_cast<void**>(pAlignedMem) - 1;

//     // 2. Read the original raw pointer
//     void* pRawMem = *pMetaData;

//     // 3. Free the raw memory using the matching allocator
//     free(pRawMem);
// }