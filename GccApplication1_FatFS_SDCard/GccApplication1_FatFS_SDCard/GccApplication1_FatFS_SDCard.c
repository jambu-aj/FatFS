/*
 * GccApplication1_FatFS_SDCard.c
 *
 * Created: 9/14/2015 7:16:05 PM
 *  Author: andyjambu
 */ 

#include <avr/io.h>
#include <string.h>
#include "uart.h"
#include "ff.h"
#include "diskio.h"

FIL file;                                               /* Opened file object */
FATFS fatfs;                                            /* File system object */
DIR dir;                                                /* Directory object   */
FRESULT errCode;                                        /* Error code object  */
FRESULT res;                                            /* Result object      */
UINT bytesRead;                                         /* Bytes read object  */
UINT read;                                              /* Read bytes object  */

int result=1;
char buff[] = "helloworld";
int buff_stat;

int main(void)
{
    if(1)
    {
        //TODO:: Please write your application code 
		fat_init(); // Will Hang if SD not inserted
		errCode = f_write(&file, buff, 10, &bytesRead); // Will attempt to write string 'helloworld' to file (data.txt)
		errCode = f_close(&file);
    }
}

void fat_init(void){
    errCode = -1;

    while (errCode != FR_OK){                               //go until f_open returns FR_OK (function successful)
        errCode = f_mount(0, &fatfs);                       //mount drive number 0
        errCode = f_opendir(&dir, "/");				    	//root directory

        errCode = f_open(&file, "/data.txt", FA_CREATE_ALWAYS | FA_WRITE);
        if(errCode != FR_OK){
            result=0; //used as a debugging flag
		}                                       
    }
}
