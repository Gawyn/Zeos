/*
 * segment.h - Constants de segment per a les entrades de la GDT
 */

#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include <mm_address.h>

/* Memory distribution */
/***********************/

#define KERNEL_ESP       (DWord) &task[0].stack[KERNEL_STACK_SIZE]

#define KERNEL_START     0x10000
#define L_USER_START        0x100000
#define PH_USER_START       0x100000
#define USER_ESP	L_USER_START+(NUM_PAG_CODE+NUM_PAG_DATA)*0x1000-16


/* Segment Selectors */
/*********************/

#define __KERNEL_CS     0x10  /* 2 */
#define __KERNEL_DS     0x18  /* 3 */

#define __USER_CS       0x23  /* 4 */
#define __USER_DS       0x2B  /* 5 */

#define KERNEL_TSS   0x30  /* Entry  6 on GDT (TI = 0) with RPL = 00 */

#define GDT_START  0x901b3  /* bootsect + gdt */

#endif  /* __SEGMENT_H__ */
