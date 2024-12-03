/* Author: Anish Oberoi
   Date: 2/23/2024
    */

/***
 *      ______ .___  ___. .______     _______.  ______        ____    __   __
 *     /      ||   \/   | |   _  \   /       | /      |      |___ \  /_ | /_ |
 *    |  ,----'|  \  /  | |  |_)  | |   (----`|  ,----'        __) |  | |  | |
 *    |  |     |  |\/|  | |   ___/   \   \    |  |            |__ <   | |  | |
 *    |  `----.|  |  |  | |  |   .----)   |   |  `----.       ___) |  | |  | |
 *     \______||__|  |__| | _|   |_______/     \______|      |____/   |_|  |_|
 *
 */

#include "mdadm.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "jbod.h"
#include "util.h"

int checker = 0; //initalizes a variable chekcer with the value 0 which later is used to check if the disk is mounted or not. 

uint32_t id_checker(int DiskID, int BlockID, int Command, int Reserved){ // function to help bit shifting. 

	uint32_t id_placement; // variable that would store the final value. 
	
	int disk = DiskID << 28; //variable disk is initialized that stores the value of DiskID after bit shiting 28 places.
	int block = BlockID << 20; //variable block is initialized that stores the value of BlockID after bit shiting 20 places.
	int commands = Command << 14; //variable commands is initialized that stores the value of Command after bit shiting 14 places.
	int reserves = Reserved << 0; //variable reserves is initialized that stores the value of Reserved after bit shiting 0 places.
	
	id_placement = disk|block|commands|reserves; //stores the final value of with the proper placements of the IDs.
	
	return id_placement; //returns the variable id_placement

}


int mdadm_mount(void) {
  /* YOUR CODE */
	if (checker == 0){ // checks if the disk is unmounted
		checker = 1;// changes the value of checker to 1, thus mounting it.
		int mount = jbod_operation(id_checker(0, 0, JBOD_MOUNT, 0), NULL); //initalizes the variable mount as the value of the function id_checker with given parameters including JBOD_MOUNT which makes mounts the needle on the disk.
		if (mount == -1){ //if the variable mount == -1, then it means it gives an error after mounting therefore it returns -1 
			return -1;
		}
		else{
			return 1; // if the variable mount does not equal to -1, then it means that the disk is not giving any error therefore returning 1. 
		}
	}
	
	return -1;	
}

int mdadm_unmount(void) {
  /* YOUR CODE */
	if (checker == 1){ // checks if the disk is mounted
		checker = 0;// changes the value of checker to 0, thus unmounting it.
		int unmount = jbod_operation(id_checker(0, 0, JBOD_UNMOUNT, 0), NULL); //initalizes the variable unmount as the value of the function id_checker with given parameters including JBOD_UNMOUNT which makes unmounts the needle on the disk.
		if (unmount == -1){ //if the variable unmount == -1, then it means it gives an error after unmounting therefore it returns -1 
			return -1;
		}
		else{
			return 1; // if the variable unmount does not equal to -1, then it means that the disk is not giving any error therefore returning 1.
		}
	}

	return -1;
	
}

int seek_to_disk(uint32_t disk_num){ // Function to seek the specified disk
	uint32_t disk_p = id_checker(disk_num,0,JBOD_SEEK_TO_DISK,0); //Seeks the specified disk and stores it in a variable.
	if(jbod_operation(disk_p,NULL) == -1){
		return -1; //returns -1 if seeking the disk fails
	}
	return 0; //returns 0 if seeking the disk succeeds
}

int seek_to_block(uint32_t block_num){ // Function to seek the specified block
	uint32_t block_p = id_checker(0,block_num,JBOD_SEEK_TO_BLOCK,0); //Seeks the specified block and stores it in a variable.
	if(jbod_operation(block_p,NULL) == -1){
		return -1; //returns -1 if seeking the blockk fails
	}
	return 0; //returns 0 if seeking the block succeeds
}


int mdadm_read(uint32_t addr, uint32_t len, uint8_t *buf) {
  /* YOUR CODE */
 	if (checker == 0){ //checks if the disk is unmounted or mounted. 
		return -1;
	}
  
  	if(addr + len > 1048576){ //if the address is outside the disk capacity, it shows an error.
  		return -1;
  	}
  
  	if(len > 1024){ // checks if the len is larger than 1024, if yes then it should fail. 
  		return -1;
  	}
  
  	if(len > 0 && buf == NULL){ //checks if the len is greater than 0, and if the buf value is == NULL. If it is then it fails. 
  		return -1;
  	}

  	int bytes_read = 0;
  	while(bytes_read < len){
  		uint32_t curr_disk = addr / 65536; //calculates the current disk 
  		uint32_t curr_block= (addr % 65536) / 256; //calculates the block disk 
  		uint32_t off_block = (addr % 256); //calculates the offset in disk 
  	
  	
  		if(seek_to_disk(curr_disk) == -1){ // Calls the function seek_to_disk and checks for errors while seeking to disk .
  			return -1;
  		}  
  		
  		
  		if(seek_to_block(curr_block) == -1){ // Calls the function seek_to_block and checks for errors while seeking to block.
  			return -1;
  		}  
  		
  		uint8_t temp_buf[256]; //variable temp buffer to store block data
  		
  		if(jbod_operation(id_checker(0,0,JBOD_READ_BLOCK, 0), temp_buf) == -1){ //Reads the block data 
  			return -1;
  		}  
  	
  		for(int i = off_block; i < 256 && bytes_read < len; i++){ //Copies data from temp buff to the output buff
  			buf[bytes_read++] = temp_buf[i]; 
  			addr++; 
  		}
  	}
  
  	return bytes_read; //returns the number of bytes read.
}

/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/sp24-lab2-AnishOberoi */
/*CWD /home/anishoberoi/lab2Finalpart */
/*CWD /home/anishoberoi/lab2Finalpart */
