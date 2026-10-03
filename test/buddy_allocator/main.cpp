#include "core/memory/buddy_allocator.h"

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

constexpr size_t TOTAL_MEM_BUFFER = 1024;
constexpr size_t MIN_BLOCK_SIZE = 16;
constexpr size_t MAX_BLOCKS = TOTAL_MEM_BUFFER / MIN_BLOCK_SIZE;

/// Total memory buffer
std::byte memory[TOTAL_MEM_BUFFER];

struct Dummy
{
    float vector[4];    // 16 bytes
};

void Program(size_t seed)
{
    BuddyAllocatorHandle alloc = {};
    BuddyCreate(TOTAL_MEM_BUFFER, MIN_BLOCK_SIZE, &alloc);

    // Setup random number generation (seeded for deterministic debugging)
    std::mt19937 rng(seed); 
    std::uniform_int_distribution<int> action_dist(0, 1);
    
    // Track currently allocated offsets so we know what is valid to free
    std::vector<size_t> active_offsets;
    active_offsets.reserve(MAX_BLOCKS);

    const int iterations = 5000;

    for (int i = 0; i < iterations; i++)
    {
        // Decide whether to allocate or free.
        // Force allocate if list is empty; force free if memory is full.
        bool should_allocate = (action_dist(rng) == 0);
        if (active_offsets.empty()) should_allocate = true;
        if (active_offsets.size() == MAX_BLOCKS) should_allocate = false;

        if (should_allocate)
        {
            size_t offset = BuddyAlloc(alloc, sizeof(Dummy), alignof(Dummy));
            
            // Map offset to simulated GPU/system buffer and write data
            Dummy* dummy = reinterpret_cast<Dummy*>(&memory[offset]);
            dummy->vector[0] = static_cast<float>(i); 
            dummy->vector[1] = dummy->vector[2] = dummy->vector[3] = 1.0f;

            active_offsets.push_back(offset);
        }
        else
        {
            // Pick a random currently active allocation to free
            std::uniform_int_distribution<size_t> index_dist(0, active_offsets.size() - 1);
            size_t target_idx = index_dist(rng);
            size_t offset_to_free = active_offsets[target_idx];

            BuddyFree(alloc, offset_to_free);

            // Remove the freed offset from our tracker in O(1) time
            active_offsets[target_idx] = active_offsets.back();
            active_offsets.pop_back();
        }
    }

    // Clean up any remaining allocations at the end of the simulation
    for (size_t offset : active_offsets)
    {
        BuddyFree(alloc, offset);
    }

    BuddyDestroy(alloc);
}

int main()
{
    printf("Running simulations...");

    for (size_t i = 0; i < 400000; i++)
    {
        Program(i);
    }

    printf("End simulation correctly");

    return 0;
}