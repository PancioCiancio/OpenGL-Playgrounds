# Buddy Allocator
Originally use intrusive linked list to store the available blocks. However, this approach fails when working with gpu buffer.
In order to keep everything homogeneous, this version will store the offsets and metadata in additional separate arrays.


## Sample Code
```cpp
///
///
///

#include <cassert>

#define MAX_ORDER 10    // @todo this will be set dinamically in the allocator
#define MIN_ORDER 4     // @todo this will be set dinamically in the allocator
#define MAX_SLOTS (1 << (MAX_ORDER - MIN_ORDER))

alignas(16) unsigned char mem[1 << MAX_ORDER];

/// MAX_SLOTS elements are the linked list part
/// MAX_ORDER + 1 are the sentinel elements to build a circular list for each order
int next_list[MAX_SLOTS + MAX_ORDER + 1];
int prev_list[MAX_SLOTS + MAX_ORDER + 1];
int meta_list[MAX_SLOTS];

/// Return the sentinel value associated to order
int sentinel(int order)
{
    return MAX_SLOTS + order;
}

/// Find the smallest order from the given size
int size_to_order(int n)
{
    int k = MIN_ORDER;
    while ((1 << k) < n) k++;
    assert(k <= MAX_ORDER);
    return k;
}

/// Just remove the block links from the chain.
/// Previous and next pointer of the slot removed
/// are updated to point to the next and previous
/// slot of the chain (if any). Otherwise sentinel
/// value
void remove_block(int slot)
{
    const int p     = prev_list[slot];
    const int n     = next_list[slot];
    next_list[p]    = n;
    prev_list[n]    = p;
}

/// The block freed is pushed right next to the head.
/// The new freed slot will be the first one to be
/// picked (LIFO - last in first out).
void push_block(int slot, int order)
{
    const int s             = sentinel(order);
    const int current_head  = next_list[s];

    next_list[slot]         = current_head;
    prev_list[slot]         = s;

    next_list[s]            = slot;
    prev_list[current_head] = slot; 
}

void buddy_init()
{
    for (int i = 0; i <= MAX_ORDER; i++)
    {
        int s = sentinel(i);
        next_list[s] = s;   // next value points to itself
        prev_list[s] = s;   // prev value points to itself
    }

    int s = sentinel(MAX_ORDER);
    next_list[s] = 0;       // the max order is defined and points to 0
    prev_list[s] = 0;       // the max order is defined and points to 0
    next_list[0] = s;       // the 0-slot points back xto sentinel
    prev_list[0] = s;       // the 0-slot points back to sentinel
    meta_list[0] = MAX_ORDER | 0x00F0;
}

int buddy_alloc(int size, int alignment)
{
    const int k = size_to_order(size);
    int j       = k;

    while (j <= MAX_ORDER && next_list[sentinel(j)] == sentinel(j)) j++;

    const int l = next_list[sentinel(j)];
    remove_block(l);

    while (j > k)
    {
        j--;

        const int split_index   = l + (1 << (j - MIN_ORDER));
        meta_list[split_index]  = j | 0x00F0;
        push_block(split_index, j);
    }

    meta_list[l] = k;

    return l << MIN_ORDER;
}

void buddy_free(int offset)
{
    int slot    = offset >> MIN_ORDER;
    int order   = meta_list[slot] & 0x000F;

    while (order < MAX_ORDER)
    {
        int buddy_slot      = slot ^ (1 << (order - MIN_ORDER));
        int expected_meta   = order | 0x00F0;

        if (meta_list[buddy_slot] != expected_meta) break;

        remove_block(buddy_slot);

        if (buddy_slot < slot) slot = buddy_slot;

        order++;
    }

    meta_list[slot] = order | 0x00F0;
    push_block(slot, order);
}
```