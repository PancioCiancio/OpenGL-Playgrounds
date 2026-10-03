/// 

#pragma once

#include <stdlib.h>

typedef struct BuddyAllocator* BuddyAllocatorHandle;

/// @brief 
/// @param bufferSize 
/// @param blockSize 
/// @param pAllocator 
void BuddyCreate(size_t bufferSize, size_t blockSize, BuddyAllocatorHandle* ppAllocator);

/// @brief 
/// @param size 
/// @param alignment 
/// @return 
size_t BuddyAlloc(BuddyAllocatorHandle pAllocator, size_t size, size_t alignment);

/// @brief 
/// @param offset 
void BuddyFree(BuddyAllocatorHandle pAllocator, size_t offset);

/// @brief 
/// @param pAllocator 
void BuddyDestroy(BuddyAllocatorHandle pAllocator);