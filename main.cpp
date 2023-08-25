// main.cpp
#include <string>
#include <iostream>

#include <stdio.h>
#include <assert.h>


#include "pim.h"
// dpu specific 
#include <dpu>

int main(int argc, char* argv[]) {
	srand(SEED);

	// START UPMEM CODE 
	// prepare structs for dpu sets
	struct dpu_set_t dpu_set, dpu;
	uint32_t nr_of_dpus;

	// allocate and load DPU binaries
	DPU_ASSERT(dpu_alloc(NR_DPUS, BACKEND, &dpu_set));

	DPU_ASSERT(dpu_load(dpu_set, DPU_BINARY, NULL));


	DPU_ASSERT(dpu_get_nr_dpus(dpu_set, &nr_of_dpus));
	std::cout << nr_of_dpus << " DPUS in use" << std::endl;

	// number of test per DPU
	const uint64_t nr_elements = 1<<10;
	const uint64_t elems_per_dpu = divceil(nr_elements, nr_of_dpus);

	char* elements = (char*)malloc(nr_elements * sizeof(char));
	memset(elements, nr_elements, sizeof(char));

	// bytes per DPU 
	const uint64_t total_mram_size = elems_per_dpu * sizeof(char);

	// Input arguments
	dpu_arguments_t input_arguments[nr_of_dpus];
	for (uint32_t i = 0; i < nr_of_dpus; i++) {
		input_arguments[i].size = elems_per_dpu;
		input_arguments[i].transfer_size = total_mram_size;
	}


	uint32_t idx_dpu = 0;
	// transfer input arguments 
	DPU_FOREACH(dpu_set, dpu, idx_dpu) {
		DPU_ASSERT(dpu_prepare_xfer(dpu, &input_arguments[idx_dpu]));
	}
	DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, "DPU_INPUT_ARGUMENTS", 0, sizeof(input_arguments[0]), DPU_XFER_DEFAULT));

	idx_dpu = 0; //reset idx

	
	uint32_t total_heap_loc = 0;
	uint32_t txf_size = total_mram_size;

	DPU_FOREACH(dpu_set, dpu, idx_dpu) {
		void* temp_loc = &elements[elems_per_dpu * idx_dpu];
		DPU_ASSERT(dpu_prepare_xfer(dpu, temp_loc));
	}
	DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_TO_DPU, DPU_MRAM_HEAP_POINTER_NAME, total_heap_loc, txf_size, DPU_XFER_DEFAULT));

	// launch DPUs in synchronous mode (wait until complete)
	DPU_ASSERT(dpu_launch(dpu_set, DPU_SYNCHRONOUS));


	// read the print results 
	DPU_FOREACH(dpu_set, dpu) {
		DPU_ASSERT(dpu_log_read(dpu, stdout));
	}

	// PARALLEL RETRIEVE TRANSFER
	idx_dpu = 0; // reset idx 

	/*
	// set output
	uint32_t dpu_output_size = sizeof(uint64_t) * NR_TASKLETS;
	uint32_t results_size = dpu_output_size * nr_of_dpus;

	uint64_t* results = (uint64_t*)malloc(results_size);
	memset(results, 0, results_size);

	DPU_FOREACH(dpu_set, dpu, idx_dpu) {
		DPU_ASSERT(dpu_prepare_xfer(dpu, &results[idx_dpu * NR_TASKLETS]));
	}

	// total_mram_size is used as the offset for storing return in MRAM
	DPU_ASSERT(dpu_push_xfer(dpu_set, DPU_XFER_FROM_DPU, DPU_MRAM_HEAP_POINTER_NAME, total_mram_size, dpu_output_size, DPU_XFER_DEFAULT));


	std::cout << "Transfer results for each DPU" << std::endl;
	uint16_t dpu_count = 0;
	for (uint32_t j = 0; j < nr_of_dpus * NR_TASKLETS; j++) {
		std::cout << "DPU " << dpu_count << " tasklet "
			<< j % NR_TASKLETS << " \t--> " << results[j] << std::endl;
		if ((j % NR_TASKLETS == 0) && (j >= NR_TASKLETS)) { dpu_count++; }
	}
	*/

	DPU_ASSERT(dpu_free(dpu_set));
	return 0;
}
