/*
 * include/asm-arm/arch-s3c2410/smdk2410.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *	15-July-2004 	Add ethernet definition:	Roc Wu (pwu at jadechip.com) 
 *
 */
#ifndef _INCLUDE_SMDK2410_H_
#define _INCLUDE_SMDK2410_H_

#include <linux/config.h>

#define pSMDK2410_ETH_IO      __phys_to_pfn(0x11000000)  //S3C2410_CS2 //0x11000000
#define vSMDK2410_ETH_IO      0xd0000000  //0xe0000000  //0xd0000000
#define SMDK2410_ETH_IRQ      IRQ_EINT8

#endif // _INCLUDE_SMDK2410_H_
