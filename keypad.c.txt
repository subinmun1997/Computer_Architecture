#include <stdio.h>				
#include <string.h>				
#include <stdlib.h>				
#include <unistd.h>				
#include <sys/mman.h>				
#include <fcntl.h>				
				
#define FPGA_BASEADDRESS        0x05000000				
#define PIEZO_OFFSET		0x50		
#define KEY_COL_OFFSET		0x70		
#define KEY_ROW_OFFSET		0x72		
				
int main(void)				
{				
	short value;			
	unsigned short *addr_fpga;			
	unsigned short *keypad_row_addr, *keypad_col_addr, *piezo_addr;			
	int fd;			
	int i,quit=1;			
	if ((fd=open("/dev/mem",O_RDWR|O_SYNC)) < 0) {			
		perror("mem open fail\n");		
		exit(1);		
	}			
				
	addr_fpga= (unsigned short *)mmap(NULL, 4096,			
PROT_WRITE|PROT_READ, MAP_SHARED, fd, FPGA_BASEADDRESS);				
	keypad_col_addr = addr_fpga + 			
KEY_COL_OFFSET/sizeof(unsigned short);				
	keypad_row_addr = addr_fpga + 			
KEY_ROW_OFFSET/sizeof(unsigned short);				
	piezo_addr = addr_fpga + PIEZO_OFFSET/sizeof(unsigned short);			
				
	if(*keypad_row_addr ==(unsigned short)-1 || 			
*keypad_col_addr ==(unsigned short)-1 )  				
{				
		close(fd);		
		printf("mmap error\n");		
		exit(1);		
	}			
				
	printf("- Keypad\n");			
	printf("press the key button!\n");			
	printf("press the key 16 to exit!\n");			
				
	while(quit) {			
		*keypad_row_addr =  0x01;		
		usleep(1000);		
		value =(*keypad_col_addr & 0x0f);		
		*keypad_row_addr = 0x00;		
		switch(value) {		
			case 0x01 :	value = 0x01; break;
			case 0x02 :	value = 0x02; break;
			case 0x04 :	value = 0x03; break;
			case 0x08 :	value = 0x04; break;
		}		
		if(value != 0x00) goto stop_poll;		
				
		*keypad_row_addr = 0x02;		
		for(i=0;i<2000;i++);		
		value = value|(*keypad_col_addr & 0x0f);		
		*keypad_row_addr = 0x00;		
		switch(value) {		
			case 0x01 :	value = 0x05; break;
			case 0x02 :	value = 0x06; break;
			case 0x04 :	value = 0x07; break;
			case 0x08 :	value = 0x08; break;
		}		
		if(value != 0x00) goto stop_poll;		
				
		*keypad_row_addr = 0x04;		
		for(i=0;i<2000;i++);		
		value = value|(*keypad_col_addr & 0x0f);		
		*keypad_row_addr = 0x00;		
		switch(value) {		
			case 0x01 :	value = 0x09; break;
			case 0x02 :	value = 0x0a; break;
			case 0x04 :	value = 0x0b; break;
			case 0x08 :	value = 0x0c; break;
		}		
		if(value != 0x00) goto stop_poll;		
				
		*keypad_row_addr = 0x08;		
		for(i=0;i<2000;i++);		
		value = value|(*keypad_col_addr & 0x0f);		
		*keypad_row_addr = 0x00;		
		switch(value) {		
			case 0x01 :	value = 0x0d; break;
			case 0x02 :	value = 0x0e; break;
			case 0x04 :	value = 0x0f; break;
			case 0x08 :	value = 0x10; break;
		}		
				
	stop_poll:			
		if(value>0 && value!=5 && value!=6) {		
			printf("\n pressed key = %02d\n",value);	
			*piezo_addr=0x1;	
			usleep(50000);	
			*piezo_addr=0x0;	
		}		
		else if(value==5) {		
			printf("\n 201701282\nMun subin\n");	
			*piezo_addr=0x02;	
			usleep(500000);	
			*piezo_addr=0x03;	
			usleep(500000);	
			*piezo_addr=0x04;	
			usleep(500000);	
			*piezo_addr=0x0;	
		}		
		else if(value==6) {		
			printf("\n ICE HUFS\nMun subin\n");	
			*piezo_addr=0x11;	
			usleep(500000);	
			*piezo_addr=0x15;	
			usleep(500000);	
			*piezo_addr=0x16;	
			usleep(500000);	
			*piezo_addr=0x0;	
		}		
		else *keypad_row_addr = 0x00;		
		for(i=0;i<4000000;i++);		
				
		if(value == 16) {		
			printf("\nExit Program!! (key = %02d)\n\n", value);	
			*piezo_addr=0x1;	
			usleep(150000);	
			*piezo_addr=0x0;	
			quit = 0;	
		}		
	}			
				
	munmap(addr_fpga,4096);			
	close(fd);			
	return 0;			
}				
