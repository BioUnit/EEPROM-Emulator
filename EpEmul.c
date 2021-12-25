#include "main.h"
#include "EpEmul.h"
// Enjoy! Douche.


FLASH_EraseInitTypeDef erase = {FLASH_TYPEERASE_PAGES, FLASH_BANK_2, 0, 1};		// structure for erasing routine
uint32_t fault = 0;																// fault status for erase procedure (not used)


//-----------------------------------------------------------------------------------------------
/*
 * This function searches free address to write in
 * Parameters	: none
 * Return value : valid flash address to write in or zero (0) if there is no free space left
 */
uint32_t epemul_search_valid_4word_addr(void)
{
	uint32_t searcher = 0;
	uint8_t status = 0;
	for( uint32_t a = MEMORY_START_ADDR; a < MEMORY_END_ADDR; a = a+0x10 )	// searching free space in whole memory (0x10 is a 4 word step)
	{
		searcher = *(__IO uint32_t*) a;
		if( searcher == 0xFFFFFFFF )
		{
			searcher = a;
			status = 1;
			break;									// step out from cycle
		}
	}
	if( status == 0 )
	{
		searcher = 0;
	}
	return searcher;
}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
/*
 * This function searches last key value and reads 6-byte array of data after it (2 words)
 * Parameters:
 * 		key     	  : a 16-bit virtual address
 * 		data_to_read  : 6-byte array to write in
 * Return value 	  : valid flash address to write in
 */
uint32_t epemul_read_2word_array(uint16_t key, uint8_t *data_to_read)
{
	uint32_t searcher = 0;
	for( uint32_t a = MEMORY_END_ADDR; a > MEMORY_START_ADDR; a = a-0x08 )	// searching the key in whole page starting from end of page (0x08 is a 2 word step)
	{
		searcher = *(__IO uint32_t*) a;
		if( (searcher >> 16) == key)
		{
			data_to_read[0] = searcher >> 8;
			data_to_read[1] = searcher;
			searcher = *(__IO uint32_t*) (a+4);		// read next word
			data_to_read[2] = searcher >> 24;
			data_to_read[3] = searcher >> 16;
			data_to_read[4] = searcher >> 8;
			data_to_read[5] = searcher;
			searcher = a + 8;						// this will next valid address to write in
			break;									// step out from cycle
		}
	}
	return searcher;
}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
/*
 * This function searches last key value and reads 14-byte array of data after it (4 words)
 * Parameters:
 * 		key     	  : a 16-bit virtual address
 * 		data_to_read  : array to write in
 * Return value 	  : valid flash address to write in
 */
uint32_t epemul_read_4word_array(uint16_t key, uint8_t *data_to_read)
{
	uint32_t searcher = 0;
	for( uint32_t a=MEMORY_END_ADDR; a>=MEMORY_START_ADDR; a=a-0x08)	// searching the key in whole page starting from end of page (0x08 is a 2 word step)
	{
		searcher = *(__IO uint32_t*) a;
		if( (searcher >> 16) == key)				// checking if there is a key value in this address
		{
			data_to_read[0] = searcher >> 8;
			data_to_read[1] = searcher;
			searcher = *(__IO uint32_t*) (a+4);		// reading 2nd word
			data_to_read[2] = searcher >> 24;
			data_to_read[3] = searcher >> 16;
			data_to_read[4] = searcher >> 8;
			data_to_read[5] = searcher;
			searcher = *(__IO uint32_t*) (a+8);		// 3d word
			data_to_read[6] = searcher >> 24;
			data_to_read[7] = searcher >> 16;
			data_to_read[8] = searcher >> 8;
			data_to_read[9] = searcher;
			searcher = *(__IO uint32_t*) (a+12);	// 4th word
			data_to_read[10] = searcher >> 24;
			data_to_read[11] = searcher >> 16;
			data_to_read[12] = searcher >> 8;
			data_to_read[13] = searcher;
			searcher = a + 16;						// this will next valid address to write in
			break;
		}
	}
	return searcher;								// valid address after last stored data
}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
/*
 * This function writes a key and 14-byte array in a specific address
 * Parameters:
 * 		key     	  : a 16-bit virtual address
 * 		address		  : 32bit flash register address
 * 		data_to_store : array to write
 * Return value 	  : valid flash address to write in
 */
uint32_t epemul_write_4word_array(uint16_t key, uint32_t address, uint8_t *data_to_store)
{
	// The HAL function writes Less Significant part of word first, so operation below turn it over in Most Significant word first
	uint64_t first_word = ((uint64_t)key << 16) + ((uint64_t)data_to_store[0]<<8) + ((uint64_t)data_to_store[1]) + ((uint64_t)data_to_store[2]<<56)
		 + ((uint64_t)data_to_store[3]<<48) + ((uint64_t)data_to_store[4]<<40) + ((uint64_t)data_to_store[5]<<32);
	uint64_t second_word = ((uint64_t)data_to_store[6] << 24) + ((uint64_t)data_to_store[7]<<16) + ((uint64_t)data_to_store[8]<<8)
			+ ((uint64_t)data_to_store[9]) + ((uint64_t)data_to_store[10]<<56) + ((uint64_t)data_to_store[11]<<48)
			+ ((uint64_t)data_to_store[12]<<40) + ((uint64_t)data_to_store[13]<<32);
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, first_word);			// first word with key flash programming
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address + 0x08, second_word);	// second word
	HAL_FLASH_Lock();
	return (address+16);
}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
/*
 * This function writes a key and 6-byte array in a specific flash address
 * Parameters:
 * 		key    	  	   : a 16-bit virtual address
 * 		address 	   : 32bit flash register address
 * 		ddata_to_store : array to write
 * Return value 	   : valid flash address to write in
 */
uint32_t epemul_write_2word_array(uint16_t key, uint32_t address, uint8_t *data_to_store)
{
	// The HAL function writes Less Significant part of word first, so operation below turn it over in Most Significant word first
	uint64_t data = ((uint64_t)key << 16) + ((uint64_t)data_to_store[0]<<8) + ((uint64_t)data_to_store[1]) + ((uint64_t)data_to_store[2]<<56)
		 + ((uint64_t)data_to_store[3]<<48) + ((uint64_t)data_to_store[4]<<40) + ((uint64_t)data_to_store[5]<<32);
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);
	HAL_FLASH_Lock();
	return (address+8);
}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
/*
 * This function erases 2 pages and rewrites all variables in the begging
 * For now it can rewrite only 10 variables (with next keys: 0x1111,0x2222,0x3333,0x4444,0x5555,0x6666,0x7777,0x8888,0x9999)
 * Parameters :
 * 		number_of_vars : number of variables to save (1-9)
 * Return value 	   : valid flash address to write in
 */
uint32_t epemul_rewrite(uint8_t number_of_vars)
{
	uint32_t ip_valid_address = 0;
	uint8_t var1[14] = {0};
	uint8_t var2[14] = {0};
	uint8_t var3[14] = {0};
	uint8_t var4[14] = {0};
	uint8_t var5[14] = {0};
	uint8_t var6[14] = {0};
	uint8_t var7[14] = {0};
	uint8_t var8[14] = {0};
	uint8_t var9[14] = {0};
	uint8_t *arrays[10] = {0, var1, var2, var3, var4, var5, var6, var7, var8, var9};	// variables array pointer

	for( uint8_t a=1; a<=number_of_vars; a++ )
	{
		epemul_read_4word_array(0x1111*a, arrays[a]);									// read all variables
	}
	HAL_FLASH_Unlock();
	erase.Page = 0;																		// change erase page
	HAL_FLASHEx_Erase( &erase, &fault );												// erase 1st page
	ip_valid_address = MEMORY_START_ADDR;												// start from 1st page address
	for( uint8_t a=1; a<=number_of_vars; a++ )
	{
		ip_valid_address = epemul_write_4word_array( 0x1111*a, ip_valid_address, arrays[a] );		// write all variables at the beginning
	}
	HAL_FLASH_Unlock();
	erase.Page = 1;																		// change page in erasing structure
	HAL_FLASHEx_Erase(&erase, &fault);													// erase 2nd page
	HAL_FLASH_Lock();
	return ip_valid_address;															// return valid address to write in
}
//-----------------------------------------------------------------------------------------------
