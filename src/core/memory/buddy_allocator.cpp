#include "buddy_allocator.h"

#include <cassert>
#include <stdint.h>
#include <iostream>

#include "alignment.h"

struct BuddyAllocator
{
    size_t* listNext;
    size_t* listPrev;
    size_t* listMeta;
    uint8_t kMax;
    uint8_t kMin;
};

/// @brief Calculate the closest power of 2 of size
/// @param size
/// @return The closest power of 2 of size
uint8_t SizeToOrder(size_t size)
{
    uint8_t k = 0;
    while ((1ull << k) < size) k++;
    return k;
}

/// @brief 
/// @param pAllocator 
/// @param order 
/// @return 
size_t Sentinel(BuddyAllocatorHandle pAllocator, uint8_t order)
{
    assert(pAllocator);
    return (1 << (pAllocator->kMax - pAllocator->kMin)) + order;
}

/// @brief 
/// @param pAllocator 
/// @param slot 
void RemoveBlock(BuddyAllocatorHandle pAllocator, size_t slot)
{
    assert(pAllocator);

    const size_t p          = pAllocator->listPrev[slot];
    const size_t n          = pAllocator->listNext[slot];
    pAllocator->listNext[p] = n;
    pAllocator->listPrev[n] = p;
}

void PushBlock(BuddyAllocatorHandle pAllocator, size_t slot, uint8_t order)
{
    assert(pAllocator);

    const size_t s                      = Sentinel(pAllocator, order);
    const size_t currentHead            = pAllocator->listNext[s];

    pAllocator->listNext[slot]          = currentHead;
    pAllocator->listPrev[slot]          = s;

    pAllocator->listNext[s]             = slot;
    pAllocator->listPrev[currentHead]   = slot;
}

void BuddyCreate(size_t bufferSize, size_t blockSize, BuddyAllocatorHandle* ppAllocator)
{
    assert(ppAllocator);

    const uint8_t kMax      = SizeToOrder(bufferSize);
    const uint8_t kMin      = SizeToOrder(blockSize);
    const size_t maxSlots   = 1 << (kMax - kMin);

    // 1. Calculate byte sizes for each contiguous block
    const size_t structBytes    = sizeof(BuddyAllocator);
    const size_t nextBytes      = (maxSlots + kMax + 1) * sizeof(size_t);
    const size_t prevBytes      = (maxSlots + kMax + 1) * sizeof(size_t);
    const size_t metaBytes      = maxSlots * sizeof(size_t);
    const size_t alignBytes     = alignof(size_t) - 1;                      // Cover worst case scenario alignment

    const size_t totalMem = structBytes + nextBytes + prevBytes + metaBytes + alignBytes;

    // malloc guarantees safe base alignment for the struct
    void* totalMemBuffer = malloc(totalMem);
    assert(totalMemBuffer);

    // 3. Map the struct to the base address
    BuddyAllocator* allocator = static_cast<BuddyAllocator*>(totalMemBuffer);
    allocator->kMax = kMax;
    allocator->kMin = kMin;

    // 4. Safely offset the pointers using byte arithmetic
    const uintptr_t memoryTracker = reinterpret_cast<uintptr_t>(totalMemBuffer);
    allocator->listNext = reinterpret_cast<size_t*>(AlignPtr((memoryTracker + structBytes), alignof(size_t)));
    allocator->listPrev = reinterpret_cast<size_t*>(AlignPtr((memoryTracker + structBytes + nextBytes), alignof(size_t)));
    allocator->listMeta = reinterpret_cast<size_t*>(AlignPtr((memoryTracker + structBytes + nextBytes + prevBytes), alignof(size_t)));

    for (size_t i = 0; i <= allocator->kMax; i++)
    {
        size_t s = Sentinel(allocator, i);
        allocator->listNext[s] = s;
        allocator->listPrev[s] = s;
    }

    allocator->listMeta[0] = allocator->kMax | 0x00F0;
    PushBlock(allocator, 0, kMax);

    *ppAllocator = allocator;
}

size_t BuddyAlloc(BuddyAllocatorHandle pAllocator, size_t size, size_t alignment)
{
    assert(pAllocator);

    const uint8_t k = std::max(SizeToOrder(size), SizeToOrder(alignment));
    uint8_t j       = k;

    while (j <= pAllocator->kMax && pAllocator->listNext[Sentinel(pAllocator, j)] == Sentinel(pAllocator, j)) { j++; }
    assert(j <= pAllocator->kMax);

    const size_t l = pAllocator->listNext[Sentinel(pAllocator, j)];
    RemoveBlock(pAllocator, l);

    while (j > k)
    {
        j--;

        const size_t splitIndex             = l + (1 << (j - pAllocator->kMin));
        pAllocator->listMeta[splitIndex]    = j | 0x00F0;
        PushBlock(pAllocator, splitIndex, j);
    }

    pAllocator->listMeta[l] = k;

    return l << pAllocator->kMin;
}

void BuddyFree(BuddyAllocatorHandle pAllocator, size_t offset)
{
    assert(pAllocator);

    size_t slot     = offset >> pAllocator->kMin;
    size_t order    = pAllocator->listMeta[slot] & 0x000F;

    while (order < pAllocator->kMax)
    {
        const size_t buddySlot      = slot ^ (1 << (order - pAllocator->kMin));
        const size_t expectedMeta   = order | 0x00F0;

        // Check if the buddy is empty
        if (pAllocator->listMeta[buddySlot] != expectedMeta) break;

        RemoveBlock(pAllocator, buddySlot);

        if (buddySlot < slot) slot = buddySlot;

        order++;
    }

    pAllocator->listMeta[slot] = order | 0x00F0;
    PushBlock(pAllocator, slot, order);
}

void BuddyDestroy(BuddyAllocatorHandle pAllocator)
{
    assert(pAllocator);
    free(pAllocator);
}