#include <stdio.h>		
#include <string.h>	
#include <stdlib.h>
#include <unistd.h>	
#include <sys/mman.h>
#include <fcntl.h>			
 			
#define FPGA_BASEADDRESS   	0x05000000		
#define TEXTLCD_OFFSET		0x10	
			
void setcommand(unsigned short command);	
void writebyte(char ch);	
void initialize_textlcd();	
int function_set(int rows, int nfonts);	
int display_control(int display_enable, int cursor_enable, int nblink);	
int cursor_shift(int set_screen, int set_rightshit);	
int entry_mode_set(int increase, int nshift);	
int return_home();	
int clear_display();	
int set_ddram_address(int pos);	
			
unsigned short *addr_fpga, *pTextlcd;	
			
int main(int argc, char **argv)			
{			
	int fd;
	int i, len1=17,len2=17;
	char buf1[17] = "Welcome to the  ";
       char buf2[17] = "Embedded World!!";	
			
	if ((fd=open("/dev/mem", O_RDWR|O_SYNC)) < 0) {		
		printf("mem open fail\n");	
		exit(1);	
	}		
			
	addr_fpga= (unsigned short *)mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, FPGA_BASEADDRESS);
	pTextlcd = addr_fpga + TEXTLCD_OFFSET/sizeof(unsigned short); 		
			
	if(*pTextlcd == (unsigned short)-1) {		
		close(fd);	
               printf("mmap error\n");	
               exit(1);	
	}		
			
	if(argc == 2) {		
		len1 = strlen(argv[1]);	
		len2 = 0;	
		strcpy(buf1,argv[1]);	
	}else if(argc >= 3) {		
		len1 = strlen(argv[1]);	
		len2 = strlen(argv[2]);	
		strcpy(buf1,argv[1]);	
		strcpy(buf2,argv[2]);	
	}		
	initialize_textlcd();
			
	for(i=0;i<len1;i++) writebyte(buf1[i]);
	set_ddram_address(0x40);
	for(i=0;i<len2;i++) writebyte(buf2[i]);
			
	printf("- Textlcd\n");
	printf("LINE 1 :	");
	for(i=0;i<len1;i++) printf("%c", buf1[i]);
	printf("\nLINE 2 :	");
	for(i=0;i<len2;i++) printf("%c", buf2[i]);	
	printf("\n");
			
	munmap(pTextlcd,4096);
	close(fd);
	return 0;
}			
void setcommand(unsigned short command)			
{			
 	command &= 0x00FF;
	*pTextlcd = command | 0x0000;
	usleep(1000);
	*pTextlcd = command | 0x0100;
	usleep(1000);
	*pTextlcd = command | 0x0000;
	usleep(1000);
}			
void writebyte(char ch)			
{			
	unsigned short data;
	data = ch & 0x00FF;
	*pTextlcd = data&0x400;
	usleep(1000);
	*pTextlcd = data|0x500;
	usleep(1000);
	*pTextlcd = data|0x400;
	usleep(1000);
			
}			
void initialize_textlcd() {			
	function_set(2,0);
	display_control(1,0,0);
	clear_display();
	entry_mode_set(1,0);
	return_home();
}			
			
int function_set(int rows, int nfonts) {			
	unsigned short command = 0x30;
			
	if(rows == 2) command |= 0x08;
	else if(rows == 1) command &= 0xf7;
	else return -1;
			
	command = nfonts ? (command | 0x04) :	command;	
	setcommand(command);
	return 1;
}			
int display_control(int display_enable, int cursor_enable, int nblink) {			
	unsigned short command = 0x08;
	command = display_enable ? (command | 0x04) :	command;	
	command = cursor_enable ? (command | 0x02) :	command;	
	command = nblink ? (command | 0x01) :	command;	
	setcommand(command);
	return 1;
}			
			
int cursor_shift(int set_screen, int set_rightshit) {			
	unsigned short command = 0x10;
	command = set_screen ? (command | 0x08) :	command;	
	command = set_rightshit ? (command | 0x04) :	command;	
	setcommand(command);
	return 1;
}			
int entry_mode_set(int increase, int nshift) {			
	unsigned short command = 0x04;
	command = increase ? (command | 0x2) :	command;	
	command = nshift ? ( command | 0x1) :	command;	
	setcommand(command);
	return 1;
}			
			
int return_home() {			
	unsigned short command = 0x02;
	setcommand(command);
	return 1;
}			
			
int clear_display() {			
	unsigned short command = 0x01;
	setcommand(command);
	return 1;
}			
int set_ddram_address(int pos) {			
	unsigned short command = 0x80;
	command += pos;
	setcommand(command);
	return 1;
}			
