/**      
 * @author Batto1 with using system_off sample
*/

#include <stddef.h>
#include <stdbool.h>
#include <zephyr/sys/util.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/crc.h>
#include <hal/nrf_power.h>

#include "ram_retention_utils.h"



/* nRF52 RAM (really, RAM AHB slaves) are partitioned as:
 * * Up to 8 blocks of two 4 KiBy byte "small" sections
 * * A 9th block of with 32 KiBy "large" sections
 *
 * At time of writing the maximum number of large sections is 6, all
 * within the first large block.  Theoretically there could be more
 * sections in the 9th block, and possibly more blocks.
 */

/* Inclusive address of RAM start */
#define SRAM_BEGIN (uintptr_t)DT_REG_ADDR(DT_NODELABEL(sram0))

/* Exclusive address of RAM end */
#define SRAM_END (SRAM_BEGIN + (uintptr_t)DT_REG_SIZE(DT_NODELABEL(sram0)))

/* Size of a controllable RAM section in the small blocks */
#define SMALL_SECTION_SIZE 4096

/* Number of controllable RAM sections in each of the lower blocks */
#define SMALL_SECTIONS_PER_BLOCK 2

/* Span of a small block */
#define SMALL_BLOCK_SIZE (SMALL_SECTIONS_PER_BLOCK * SMALL_SECTION_SIZE)

/* Number of small blocks */
#define SMALL_BLOCK_COUNT 8

/* Span of the SRAM area covered by small sections */
#define SMALL_SECTION_SPAN (SMALL_BLOCK_COUNT * SMALL_BLOCK_SIZE)

/* Inclusive address of the RAM range covered by large sections */
#define LARGE_SECTION_BEGIN (SRAM_BEGIN + SMALL_SECTION_SPAN)

/* Size of a controllable RAM section in large blocks */
#define LARGE_SECTION_SIZE 32768




bool RR_Init_Variable_Ram_Retention(void * a_retained_var_ptr, size_t a_retained_var_size, size_t a_retained_crc_offset, size_t a_sizeof_retained_crc)
{
	bool valid = false;
	valid = ram_retained_validate(a_retained_var_ptr, a_retained_var_size, a_retained_crc_offset, a_sizeof_retained_crc);
	return valid;
}

/** 
 *  @param [in] a_retained_var_ptr 	Pointer to the struct type variable where the RAM block it resides will be retained. i.e. &my_struct
 *  @param [in] a_retained_var_size 	Size of the struct type variable where the RAM block it resides will be retained. i.e. sizeof(my_struct)
 *  @param [in] a_retained_crc_offset  	offsetof the crc varible of the struct type variable where the RAM block it resides will be retained i.e. offsetof(struct my_struct, crc)
 *  @param [in] a_sizeof_retained_crc 	sizeof the crc varible of the struct type variable where the RAM block it resides will be retained i.e. sizeof(your_struct_var.crc).
 *  @note  struct type variable where the RAM block it resides is to be retained must have uint32_t crc variable as the last element. 
*/
bool ram_retained_validate(void * a_retained_var_ptr, size_t a_retained_var_size, size_t a_retained_crc_offset, size_t a_sizeof_retained_crc)
{
	/* The residue of a CRC is what you get from the CRC over the
	 * message catenated with its CRC.  This is the post-final-xor
	 * residue for CRC-32 (CRC-32/ISO-HDLC) which Zephyr calls
	 * crc32_ieee.
	 */
	const uint32_t residue = 0x2144df1c;
     size_t retained_checked_size = a_retained_crc_offset + a_sizeof_retained_crc;
	uint32_t crc = crc32_ieee((const uint8_t *)a_retained_var_ptr, retained_checked_size);
	
	bool valid = (crc == residue);
	/* If the CRC isn't valid, reset the retained data. */
	if (!valid) {
		memset(a_retained_var_ptr, 0, a_retained_var_size);
	}

	/* Reconfigure to retain the state during system off, regardless of
	 * whether validation succeeded.  Although these values can sometimes
	 * be observed to be preserved across System OFF, the product
	 * specification states they are not retained in that situation, and
	 * that can also be observed.
	 */
	(void)ram_range_retain(a_retained_var_ptr, retained_checked_size, true);

	return valid;
}

/*
 * @brief update the retained variable.
 */
void ram_retained_update(void * a_p_retained_var, size_t a_retained_crc_offset)
{
	uint32_t crc = crc32_ieee((const uint8_t *)a_p_retained_var, a_retained_crc_offset);

     uint32_t* crc_ptr = (uint32_t*)((char*)a_p_retained_var + a_retained_crc_offset);
     * crc_ptr = sys_cpu_to_le32(crc);
}

/* Set or clear RAM retention in SYSTEM_OFF for the provided object.
 *
 * @note This only works for nRF52 with the POWER module.  The other
 * Nordic chips use a different low-level API, which is not currently
 * used by this function.
 *
 * @param ptr pointer to the start of the retainable object
 *
 * @param len length of the retainable object
 *
 * @param enable true to enable retention, false to clear retention
 */
int ram_range_retain(const void *ptr,
			    size_t len,
			    bool enable)
{
	uintptr_t addr = (uintptr_t)ptr;
	uintptr_t addr_end = addr + len;

	/* Error if the provided range is empty or doesn't lie
	 * entirely within the SRAM address space.
	 */
	if ((len == 0U)
	    || (addr < SRAM_BEGIN)
	    || (addr > (SRAM_END - len))) {
		return -EINVAL;
	}

	/* Iterate over each section covered by the range, setting the
	 * corresponding RAM OFF retention bit in the parent block.
	 */
	do {
		uintptr_t block_base = SRAM_BEGIN;
		uint32_t section_size = SMALL_SECTION_SIZE;
		uint32_t sections_per_block = SMALL_SECTIONS_PER_BLOCK;
		bool is_large = (addr >= LARGE_SECTION_BEGIN);
		uint8_t block = 0;

		if (is_large) {
			block = 8;
			block_base = LARGE_SECTION_BEGIN;
			section_size = LARGE_SECTION_SIZE;

			/* RAM[x] supports only 16 sections, each its own bit
			 * for POWER (0..15) and RETENTION (16..31).  We don't
			 * know directly how many sections are present, so
			 * assume they all are; the true limit will be
			 * determined by the SRAM size.
			 */
			sections_per_block = 16;
		}

		uint32_t section = (addr - block_base) / section_size;

		if (section >= sections_per_block) {
			block += section / sections_per_block;
			section %= sections_per_block;
		}

		uint32_t section_mask =
			(POWER_RAM_POWERSET_S0RETENTION_On
					 << (section + POWER_RAM_POWERSET_S0RETENTION_Pos));

		if (enable) {
			nrf_power_rampower_mask_on(NRF_POWER, block, section_mask);
		} else {
			nrf_power_rampower_mask_off(NRF_POWER, block, section_mask);
		}

		/* Move to the first address in the next section. */
		addr += section_size - (addr % section_size);
	} while (addr < addr_end);

	return 0;
}





