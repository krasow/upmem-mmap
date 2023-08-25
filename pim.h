#ifndef PIM_H 
#define PIM_H

#include <stdlib.h>
#include <stdint.h>

#define DPU_BINARY "dpu_binary"


/* SET DEFAULT DEFINE VALUES */
#ifndef SIMULATOR
#define SIMULATOR	1
#endif

#if SIMULATOR == 0
#undef SIMULATOR
#endif

#ifndef NR_TASKLETS
#define NR_TASKLETS 16
#endif

#ifndef NR_DPUS
#define NR_DPUS		32
#endif

#define BLOCK_SIZE_LOG2 5 
#define BLOCK_SIZE      (1 << BLOCK_SIZE_LOG2)

/* END OF SETTING DEFAULTS */

#define __BIGINT_LOG2_SIZE    3
#define __DOUBLE_LOG2_SIZE    3
#define __DATE_LOG2_SIZE      2


#if SIMULATOR
	#define BACKEND "backend=simulator"
#else
	#define BACKEND "backend=hw"
#endif


typedef struct {
    uint32_t size;
    uint32_t transfer_size;
} dpu_arguments_t;

#define __BIGINT    uint64_t
#define __DOUBLE    uint64_t
#define __CHAR      uchar_t
#define __DATE      uint32_t
#define __DBSTRING  uchar_t

#define SEED			72
#define PAGE_SIZE       4096

#define divceil(n, m)   (((n)-1) / (m) + 1)
#define roundup(n, m)   ((n / m) * m + m)


typedef unsigned char uchar_t;
typedef uint64_t addr_t;



#endif
