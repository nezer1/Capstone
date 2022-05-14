/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */
#include "SD.h"
/* Definitions of physical drive number for each drive */
//#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
//#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
//#define USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{

	  if(pdrv)
    {
        return STA_NOINIT;  
    }
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat;


  stat=init_MicroSDcard(); //SDka
	    if(stat == STA_NODISK)
    {
        return STA_NODISK;
    }
    else if(stat != 0)
    {
        return STA_NOINIT;  //????:?????
    }
    else
    {
        return 0;           //?????
    }
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res;



	 if (pdrv || !count)
    {    
        return RES_PARERR;  //????????,count????0,????????
    }
		
	if (count == 1)  // ????
	{
		res = readSingleBlock(sector,buff);
	}
	else             // ????
	{
		res = readMultipleBlock(sector,buff,count);
	}

    //?????,?SD.c??????ff.c????
    if(res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
	

}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res;


	  if (pdrv || !count)
    {    
        return RES_PARERR;  //????????,count????0,????????
    }

		    // ????
    if(count == 1)
    {
        res = writeSingleBlock(sector,buff);
    }
    else
    {
        res = writeMultipleBlocks(sector,buff,count);
    }
    // ?????
    if(res == 0)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR;
    }
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	BYTE n, csd[16];
	DWORD csize;

	  if (pdrv)
    {    
        return RES_PARERR;  //????????,????????
    }
    

	res = RES_ERROR;

	switch (cmd)
	{
	    case CTRL_SYNC       : res = RES_OK; break;
	    case GET_SECTOR_COUNT: /* Get number of sectors on the disk (WORD) */
							   if((MicroSDcard_command(0x49,0x00,0x95) == 0) && Read_CSD_CID(0x49,csd))
							   {
							       if((csd[0] >> 6) == 1) /* SDC ver 2.00 */
							       {
							           csize = csd[9] + ((WORD)csd[8] << 8) + 1;
							           *(DWORD*)buff = (DWORD)csize << 10;
							       }
							       else /* MMC or SDC ver 1.XX */
							       {
							           n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
							           csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
							           *(DWORD*)buff = (DWORD)csize << (n - 9);
							       }
							       res = RES_OK;
							   }
							   break;

	    case GET_SECTOR_SIZE : /* Get sectors on the disk (WORD) */
			                   *(WORD*)buff = 512;
			                   res = RES_OK;
			                   break;

	    case GET_BLOCK_SIZE  : if ((MicroSDcard_command(0x49,0x00,0x95) == 0) &&  Read_CSD_CID(0x49,csd)) /* Read CSD */
		                       {
			                       *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
			                       res = RES_OK;
		                       }
	                           break;

	    default              : res = RES_PARERR; break;
	}

	return res;
}

DWORD get_fattime (void)
{
return 0;
}

