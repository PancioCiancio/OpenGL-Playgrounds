#pragma once


// C++ API - EXAMPLE
class BuddyAllocator
{
public:
    BuddyAllocator(size_t size, size_t min_block_size);
    
    void allocate();
    void free();

private:
    size_t max_order_;
    size_t min_order_;
    size_t* next_list;
    size_t* prev_list;
    size_t* meta_list;
};


// C API - EXAMPLE
struct BuddyAllocator
{
    size_t max_order_;
    size_t min_order_;
    size_t* next_list;
    size_t* prev_list;
    size_t* meta_list;
};

constexpr size_t align_up(size_t offset, size_t alignment)
{
    return (offset + (alignment - 1)) & ~(alignment - 1);
}

void buddy_create(BuddyAllocator* alloc)
{

    size_t required_mem = sizeof(BuddyAllocator);
    required_mem = align_up(required_mem, alignof(size_t));
    required_mem += sizeof(size_t) * 64;
    void* raw_mem = malloc(required_mem);
}