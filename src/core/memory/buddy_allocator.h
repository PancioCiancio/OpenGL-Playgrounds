/// 

#pragma once

#include <stdlib.h>

typedef struct BuddyAllocator* BuddyAllocatorHandle;

/// @brief Create and initialize buddy allocator
/// @param bufferSize   power of 2 size
/// @param blockSize    power of 2 size
/// @param ppAllocator  output allocator (&(*allocator))
void BuddyCreate(size_t bufferSize, size_t blockSize, BuddyAllocatorHandle* ppAllocator);

/// @brief Allocate memory from the allocator. Can fail and alt the program
/// @param size         size of the memory requested
/// @param alignment    alignment of type requested
/// @return The offset to a memory buffer
size_t BuddyAlloc(BuddyAllocatorHandle pAllocator, size_t size, size_t alignment);

/// @brief Free the buddy slot at given offset
/// @param offset   Valid offset previously returned from BuddyAlloc
void BuddyFree(BuddyAllocatorHandle pAllocator, size_t offset);

/// @brief Destroy a valid allocator. Can fail and alt the program
/// @param pAllocator Valid pointer to previously created allocator
void BuddyDestroy(BuddyAllocatorHandle pAllocator);