// q6_dpu.c 
// upmem dpu binary for TPCH Q6
#include <stdio.h>
#include <string.h>

#include <defs.h>
#include <mram.h>
#include <alloc.h>
#include <perfcounter.h>
#include <handshake.h>
#include <barrier.h>
#include <assert.h>

#include "pim.h"


__host dpu_arguments_t DPU_INPUT_ARGUMENTS;

// Barrier
BARRIER_INIT(my_barrier, NR_TASKLETS);

int main() {
    perfcounter_config(COUNT_INSTRUCTIONS, true);
    uint16_t tasklet_id = me();

    // Initialize once the cycle counter
    if (tasklet_id == 0) {
        mem_reset(); // Reset the heap
    }
    barrier_wait(&my_barrier);

    uint32_t elements = DPU_INPUT_ARGUMENTS.size;

    // Address of the current processing block in MRAM
    uint32_t mram_base_addr = (uint32_t)DPU_MRAM_HEAP_POINTER;
    char* storage = (char*)mem_alloc(1 << BLOCK_SIZE_LOG2);

    for (uint32_t block_loc = tasklet_id << BLOCK_SIZE_LOG2; block_loc < elements; block_loc += (NR_TASKLETS << BLOCK_SIZE_LOG2)) {
        // Bound checking for the block
        uint32_t block_elems = (block_loc + BLOCK_SIZE >= elements) ? (elements - block_loc) : BLOCK_SIZE;


        mram_read((__mram_ptr void const*)(mram_base_addr + block_loc), storage, block_elems);

        for (uint32_t loc_j=0; loc_j < block_elems; loc_j++){
            printf("block[%d] = %d from tasklet %d\n", loc_j, storage[loc_j], tasklet_id);
        }
    }
    


    return 0;
}
