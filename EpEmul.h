/*
 * EpEmul.h
 *
 *  Created on: 5 июл. 2021 г.
 *      Author: Bio_Unit
 *
 *      Description:
 *      This library made for G4 series, that means it can read not less than 32-bit values from and write not less than
 *      64-bit in a flash (2 words). It combines all pages in one memory stack.
 *      It can manage only 9 variables each 14-bit long.
 */

#ifndef SRC_EPEMUL_H_
#define SRC_EPEMUL_H_

// User setting -------------------------------------------------------------------------
#define PAGE_LENGHT			0x800											// 2kB
#define PAGE_NUMBER			2												// 2 pages in use
#define MEMORY_START_ADDR	0x08040000										// 2-nd bank
#define MEMORY_END_ADDR		MEMORY_START_ADDR + (PAGE_LENGHT * PAGE_NUMBER)
//---------------------------------------------------------------------------------------

#define ARRAY_KEY_1			0x1111											// virtual address for locating
#define ARRAY_KEY_2			0x2222
#define ARRAY_KEY_3			0x3333
#define ARRAY_KEY_4			0x4444
#define ARRAY_KEY_5			0x5555
#define ARRAY_KEY_6			0x6666
#define ARRAY_KEY_7			0x7777
#define ARRAY_KEY_8			0x8888
#define ARRAY_KEY_9			0x9999

// API ---------------------------------------------------------------------------------
uint32_t epemul_read_2word_array(uint16_t key, uint8_t *data_to_read);
uint32_t epemul_read_4word_array(uint16_t key, uint8_t *data_to_read);
uint32_t epemul_write_4word_array(uint16_t key, uint32_t address, uint8_t *data_to_store);
uint32_t epemul_write_2word_array(uint16_t key, uint32_t address, uint8_t *data_to_store);
uint32_t epemul_rewrite(uint8_t number_of_vars);
uint32_t epemul_search_valid_4word_addr(void);
//--------------------------------------------------------------------------------------
#endif /* SRC_EPEMUL_H_ */
