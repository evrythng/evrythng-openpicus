/* **************************************************************************																					
 *                                OpenPicus                 www.openpicus.com
 *                                                            italian concept
 * 
 *            openSource wireless Platform for sensors and Internet of Things	
 * **************************************************************************
 *  FileName:        Regs.c
 *  Dependencies:    Microchip configs files
 *  Module:          FlyPort WI-FI
 *  Compiler:        Microchip C30 v3.12 or higher
 *
  *  Author               Rev.    Date              Comment
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Gabriele Allegria    1.0     1/20/2011		   First release  (core team)
 *  
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Software License Agreement
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  This is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by 
 *  the Free Software Foundation AND MODIFIED BY OpenPicus team.
 *  
 *  ***NOTE*** The exception to the GPL is included to allow you to distribute
 *  a combined work that includes OpenPicus code without being obliged to 
 *  provide the source code for proprietary components outside of the OpenPicus
 *  code. 
 *  OpenPicus software is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details. 
 * 
 * 
 * Warranty
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * WE ARE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 **************************************************************************/
#include "GenericTypeDefs.h"
#include "HWmap.h"

#if defined (FLYPORT_WF)

int *LATs[] 	= 	{	(int*) 0x2F4 , (int*) 0x2EC , (int*) 0x2F4 , 
						(int*) 0x2EC , (int*) 0x2EC , (int*) 0x2EC , 
						(int*) 0x2EC , (int*) 0x2DC , (int*) 0x2DC , 
						(int*) 0x2DC , (int*) 0x2DC , (int*) 0x2DC , 
						(int*) 0x2DC , (int*) 0x2DC , (int*) 0x2DC , 
						(int*) 0x000 , (int*) 0x2CC , (int*) 0x2CC ,
						(int*) 0x2CC , (int*) 0x2CC , (int*) 0x2CC ,
						(int*) 0x000 , (int*) 0x2CC , (int*) 0x000 ,
						(int*) 0x2CC , (int*) 0x000 };

int *TRISs[] 	= 	{	(int*) 0x2F0 , (int*) 0x2E8 , (int*) 0x2F0 , 
						(int*) 0x2E8 , (int*) 0x2E8 , (int*) 0x2E8 , 
						(int*) 0x2E8 , (int*) 0x2D8 , (int*) 0x2D8 , 
						(int*) 0x2D8 , (int*) 0x2D8 , (int*) 0x2D8 , 
						(int*) 0x2D8 , (int*) 0x2D8 , (int*) 0x2D8 , 
						(int*) 0x000 , (int*) 0x2C8 , (int*) 0x2C8 ,
						(int*) 0x2C8 , (int*) 0x2C8 , (int*) 0x2C8 ,
						(int*) 0x000 , (int*) 0x2C8 , (int*) 0x000 ,
						(int*) 0x2C8 , (int*) 0x000 };
						
int *PORTs[] 	= 	{	(int*) 0x2F2 , (int*) 0x2EA , (int*) 0x2F2 , 
						(int*) 0x2EA , (int*) 0x2EA , (int*) 0x2EA, 
						(int*) 0x2EA , (int*) 0x2DA , (int*) 0x2DA , 
						(int*) 0x2DA , (int*) 0x2DA , (int*) 0x2DA , 
						(int*) 0x2DA , (int*) 0x2DA , (int*) 0x2DA , 
						(int*) 0x000 , (int*) 0x2CA , (int*) 0x2CA ,
						(int*) 0x2CA , (int*) 0x2CA , (int*) 0x2CA ,
						(int*) 0x000 , (int*) 0x2CA , (int*) 0x000 ,
						(int*) 0x2CA , (int*) 0x000 };

int *CNPUs[]	=	{	(int*) 0x76 , (int*) 0x6E , (int*) 0x76 , 
						(int*) 0x6E , (int*) 0x74 , (int*) 0x74, 
						(int*) 0x74 , (int*) 0x72 , (int*) 0x72 , 
						(int*) 0x72 , (int*) 0x72 , (int*) 0x72 , 
						(int*) 0x72 , (int*) 0x72 , (int*) 0x72 , 
						(int*) 0x00 , (int*) 0x6E , (int*) 0x6E ,
						(int*) 0x6E , (int*) 0x6E , (int*) 0x6E ,
						(int*) 0x00 , (int*) 0x70 , (int*) 0x00 ,
						(int*) 0x6C , (int*) 0x00 };
						
int *CNPDs[]	=	{	(int*) 0x5E , (int*) 0x56 , (int*) 0x5E , 
						(int*) 0x56 , (int*) 0x5C , (int*) 0x5C, 
						(int*) 0x5C , (int*) 0x5A , (int*) 0x5A , 
						(int*) 0x5A , (int*) 0x5A , (int*) 0x5A , 
						(int*) 0x5A , (int*) 0x5A , (int*) 0x5A , 
						(int*) 0x00 , (int*) 0x56 , (int*) 0x56 ,
						(int*) 0x56 , (int*) 0x56 , (int*) 0x56 ,
						(int*) 0x00 , (int*) 0x58 , (int*) 0x00 ,
						(int*) 0x54 , (int*) 0x00 };
						
int IOMode[26];

int CNPos[]		=	{	3 , 1 , 4 , 2 , 7 , 6 , 8 , 5 , 6 , 7 , 8 , 1 , 
						2 , 3 , 4 , -1 , 10 , 8 , 11 , 9 , 12 , -1 , 
						0 , -1 , 12 , -1};

int IOPos[] 	= 	{	2 , 4 , 3 , 5 , 3 , 2 , 6 , 8 , 9 , 10 , 11 , 0 , 
						1 , 2 , 3 , -1 , 8 , 6 , 9 , 7 , 10 , -1 , 14 , -1 , 15 , -1};

int an[] = {0 , 0xE , 0xF , 0x7 , 0x6};

int *UMODEs[]	=	{	(int*) 0x220 , (int*) 0x230 , (int*) 0x250 , (int*) 0x2B0};
int *USTAs[]	=	{	(int*) 0x222 , (int*) 0x232 , (int*) 0x252 , (int*) 0x2B2};
int *UBRGs[]	=	{	(int*) 0x228 , (int*) 0x238 , (int*) 0x258 , (int*) 0x2B8};
int *UIFSs[]	=	{	(int*) 0x084 , (int*) 0x086 , (int*) 0x08E , (int*) 0x08E};
int *UIECs[]	=	{	(int*) 0x094 , (int*) 0x096 , (int*) 0x09E , (int*) 0x09E};
int *UTXREGs[]	=	{	(int*) 0x224 , (int*) 0x234 , (int*) 0x254 , (int*) 0x2B4};
int *URXREGs[]	=	{	(int*) 0x226 , (int*) 0x236 , (int*) 0x256 , (int*) 0x2B6};

int UTXIPos[]	=	{	4096 , 32768 , 8 , 512};
int URXIPos[]	=	{	2048 , 16384 , 4 , 256};						

int *AD1CFGL  	=	(int*) 0x032C;
int *AD1CFGH  	=	(int*) 0x032A;
int *AD1CONF1 	=	(int*) 0x0320;
int *AD1CONF2 	=	(int*) 0x0322;
int *AD1CONF3 	=	(int*) 0x0324;
int *AD1CH 	  	=	(int*) 0x0328;
int *AD1CSL   	=	(int*) 0x0330;

int *OCCON1s[]  =	{  	(int*) 0x0190, (int*) 0x019A, (int*) 0x01A4,
						(int*) 0x01AE, (int*) 0x01B8, (int*) 0x01C2,
						(int*) 0x01CC, (int*) 0x01D6, (int*) 0x01E0  };   


int *OCCON2s[]  =	{  	(int*) 0x0192, (int*) 0x019C, (int*) 0x01A6,
						(int*) 0x01B0, (int*) 0x01BA, (int*) 0x01C4,
						(int*) 0x01CE, (int*) 0x01D8, (int*) 0x01E2  };


int *OCRs[]     = 	{  	(int*) 0x0196, (int*) 0x01A0, (int*) 0x01AA,
						(int*) 0x01B4, (int*) 0x01BE, (int*) 0x01C8,
						(int*) 0x01D2, (int*) 0x01DC, (int*) 0x01E6  };


int *OCRSs[]    = 	{  	(int*) 0x0194, (int*) 0x019E, (int*) 0x01A8,
						(int*) 0x01B2, (int*) 0x01BC, (int*) 0x01C6,
						(int*) 0x01D0, (int*) 0x01DA, (int*) 0x01E4  };


int *RPORs[]    =	{  	(int*) 0x0000, (int*) 0x06CA, (int*) 0x0000,
						(int*) 0x06D0, (int*) 0x06D0, (int*) 0x06DE,
						(int*) 0x0000, (int*) 0x06C2, (int*) 0x06C4,
						(int*) 0x06C2, (int*) 0x06CC, (int*) 0x06CA,
						(int*) 0x06D8, (int*) 0x06D6, (int*) 0x06D6,
						(int*) 0x0000, (int*) 0x06C8, (int*) 0x06C6,
						(int*) 0x06C8, (int*) 0x06C6, (int*) 0x0000,
						(int*) 0x0000, (int*) 0x06CE, (int*) 0x0000,
						(int*) 0x06DC, (int*) 0x0000  	
					};

int RPFunc[]	=	{	3, 4, 5, 6, 28, 29, 30, 31, 11, 10, 12	};

int RPIORPin[]  =  	{  0, 10, 0, 17, 16, 30, -45, 2, 4, 
						3, 12, 11, 24, 23, 22, 0, 8, 6, 
						9, 7, 0, 0, 14, 0, 29, 0	
					};

int *TCONs[]	=	{	(int*) 0x0104 , (int*) 0x0110 , (int*) 0x0112 , 
						(int*) 0x011E , (int*) 0x0120	};
					  
int OCM[]       =  	{ 18, 19, 20, 21, 22, 23, 24, 25, 35 };	

int *RPIRs[]	=	{	(int*) 0x06A4, (int*) 0x06A4, (int*) 0x06A6, 
						(int*) 0x06A6, (int*) 0x06A2, (int*) 0x06AA,
						(int*) 0x06B6, (int*) 0x06B6, (int*) 0x0682,
						(int*) 0x0682, (int*) 0x0684, (int*) 0x06AC,
						(int*) 0x06AC, (int*) 0x06AE, (int*) 0x0688
					};
						

BOOL RPIRPos[]	=	{	0, 1, 0, 
						1, 1, 1,
						0, 1, 0,
						1, 0, 1,
						0, 0, 0
					};

#elif defined (FLYPORT_ETH)

int *LATs[]  = { (int*) 0x2F4 , (int*) 0x2EC , (int*) 0x2F4 ,
                 (int*) 0x2EC , (int*) 0x2EC , (int*) 0x2EC ,
                 (int*) 0x2EC , (int*) 0x2DC , (int*) 0x2DC ,
                 (int*) 0x2DC , (int*) 0x2F4 , (int*) 0x2DC ,
                 (int*) 0x2DC , (int*) 0x2DC , (int*) 0x2DC ,
                 (int*) 0x000 , (int*) 0x2CC , (int*) 0x2CC ,
                 (int*) 0x2CC , (int*) 0x2CC , (int*) 0x2CC ,
                 (int*) 0x000 , (int*) 0x2CC , (int*) 0x000 ,
                 (int*) 0x2CC , (int*) 0x000 ,
                 (int*) 0x2CC , (int*) 0x2CC , (int*) 0x2CC ,
                 (int*) 0x2CC , (int*) 0x2DC , (int*) 0x2DC ,
                 (int*) 0x2EC , (int*) 0x2EC };

int *TRISs[] = { (int*) 0x2F0 , (int*) 0x2E8 , (int*) 0x2F0 ,
                 (int*) 0x2E8 , (int*) 0x2E8 , (int*) 0x2E8 ,
                 (int*) 0x2E8 , (int*) 0x2D8 , (int*) 0x2D8 ,
                 (int*) 0x2D8 , (int*) 0x2F0 , (int*) 0x2D8 ,
                 (int*) 0x2D8 , (int*) 0x2D8 , (int*) 0x2D8 ,
                 (int*) 0x000 , (int*) 0x2C8 , (int*) 0x2C8 ,
                 (int*) 0x2C8 , (int*) 0x2C8 , (int*) 0x2C8 ,
                 (int*) 0x000 , (int*) 0x2C8 , (int*) 0x000 ,
                 (int*) 0x2C8 , (int*) 0x000 ,
                 (int*) 0x2C8 , (int*) 0x2C8 , (int*) 0x2C8 ,
                 (int*) 0x2C8 , (int*) 0x2D8 , (int*) 0x2D8 ,
                 (int*) 0x2E8 , (int*) 0x2E8 };

int *PORTs[] = { (int*) 0x2F2 , (int*) 0x2EA , (int*) 0x2F2 ,
                 (int*) 0x2EA , (int*) 0x2EA , (int*) 0x2EA,
                 (int*) 0x2EA , (int*) 0x2DA , (int*) 0x2DA ,
                 (int*) 0x2DA , (int*) 0x2F2 , (int*) 0x2DA ,
                 (int*) 0x2DA , (int*) 0x2DA , (int*) 0x2DA ,
                 (int*) 0x000 , (int*) 0x2CA , (int*) 0x2CA ,
                 (int*) 0x2CA , (int*) 0x2CA , (int*) 0x2CA ,
                 (int*) 0x000 , (int*) 0x2CA , (int*) 0x000 ,
                 (int*) 0x2CA , (int*) 0x000 ,
                 (int*) 0x2CA , (int*) 0x2CA , (int*) 0x2CA ,
                 (int*) 0x2CA , (int*) 0x2DA , (int*) 0x2DA ,
                 (int*) 0x2EA , (int*) 0x2EA };

int *CNPUs[] = { (int*) 0x76 , (int*) 0x6E , (int*) 0x76 ,
                 (int*) 0x6E , (int*) 0x74 , (int*) 0x74,
                 (int*) 0x74 , (int*) 0x72 , (int*) 0x72 ,
                 (int*) 0x72 , (int*) 0x6C , (int*) 0x72 ,
                 (int*) 0x72 , (int*) 0x72 , (int*) 0x72 ,
                 (int*) 0x00 , (int*) 0x6E , (int*) 0x6E ,
                 (int*) 0x6E , (int*) 0x6E , (int*) 0x6E ,
                 (int*) 0x00 , (int*) 0x70 , (int*) 0x00 ,
                 (int*) 0x6C , (int*) 0x00 ,
                 (int*) 0x6C , (int*) 0x6E , (int*) 0x6E ,
                 (int*) 0x6E , (int*) 0x6C , (int*) 0x6E ,
                 (int*) 0x74 , (int*) 0x74 };

int *CNPDs[] = { (int*) 0x5E , (int*) 0x56 , (int*) 0x5E ,
                 (int*) 0x56 , (int*) 0x5C , (int*) 0x5C,
                 (int*) 0x5C , (int*) 0x5A , (int*) 0x5A ,
                 (int*) 0x5A , (int*) 0x54 , (int*) 0x5A ,
                 (int*) 0x5A , (int*) 0x5A , (int*) 0x5A ,
                 (int*) 0x00 , (int*) 0x56 , (int*) 0x56 ,
                 (int*) 0x56 , (int*) 0x56 , (int*) 0x56 ,
                 (int*) 0x00 , (int*) 0x58 , (int*) 0x00 ,
                 (int*) 0x54 , (int*) 0x00 ,
                 (int*) 0x54 , (int*) 0x56 , (int*) 0x56 ,
                 (int*) 0x56 , (int*) 0x54 , (int*) 0x56 ,
                 (int*) 0x5C , (int*) 0x5C };

int IOMode[34];

int CNPos[] = {	 3,  1,  4,  2,  7,  6,  8,  5,  6,  7,  8,  1,  2,
                 3,  4, -1, 10,  8, 11,  9, 12, -1,  0, -1,  7, -1,
                 5, 13, 14, 15, 15,  0,  4,  5 };

int IOPos[] = {	 2,  4,  3,  5,  3,  2,  6,  8,  9, 10,  6,  0,  1,
                 2,  3, -1,  8,  6,  9,  7, 10, -1, 14, -1,  5, -1,
                 3, 11, 12, 13,  6,  7,  0,  1 };

int an[] = {0 , 0xE , 0x5 , 0x7 , 0x6};

int *UMODEs[]  = { (int*) 0x220 , (int*) 0x230 , (int*) 0x250 , (int*) 0x2B0};
int *USTAs[]   = { (int*) 0x222 , (int*) 0x232 , (int*) 0x252 , (int*) 0x2B2};
int *UBRGs[]   = { (int*) 0x228 , (int*) 0x238 , (int*) 0x258 , (int*) 0x2B8};
int *UIFSs[]   = { (int*) 0x084 , (int*) 0x086 , (int*) 0x08E , (int*) 0x08E};
int *UIECs[]   = { (int*) 0x094 , (int*) 0x096 , (int*) 0x09E , (int*) 0x09E};
int *UTXREGs[] = { (int*) 0x224 , (int*) 0x234 , (int*) 0x254 , (int*) 0x2B4};
int *URXREGs[] = { (int*) 0x226 , (int*) 0x236 , (int*) 0x256 , (int*) 0x2B6};

int UTXIPos[]  = { 4096 , 32768 , 8 , 512};
int URXIPos[]  = { 2048 , 16384 , 4 , 256};

int *AD1CFGL   = (int*) 0x032C;
int *AD1CFGH   = (int*) 0x032A;
int *AD1CONF1  = (int*) 0x0320;
int *AD1CONF2  = (int*) 0x0322;
int *AD1CONF3  = (int*) 0x0324;
int *AD1CH     = (int*) 0x0328;
int *AD1CSL    = (int*) 0x0330;

int *OCCON1s[] = {  (int*) 0x0190, (int*) 0x019A, (int*) 0x01A4,
                    (int*) 0x01AE, (int*) 0x01B8, (int*) 0x01C2,
                    (int*) 0x01CC, (int*) 0x01D6, (int*) 0x01E0  };

int *OCCON2s[] = {  (int*) 0x0192, (int*) 0x019C, (int*) 0x01A6,
                    (int*) 0x01B0, (int*) 0x01BA, (int*) 0x01C4,
                    (int*) 0x01CE, (int*) 0x01D8, (int*) 0x01E2  };

int *OCRs[]    = {  (int*) 0x0196, (int*) 0x01A0, (int*) 0x01AA,
                    (int*) 0x01B4, (int*) 0x01BE, (int*) 0x01C8,
                    (int*) 0x01D2, (int*) 0x01DC, (int*) 0x01E6  };

int *OCRSs[]   = {  (int*) 0x0194, (int*) 0x019E, (int*) 0x01A8,
                    (int*) 0x01B2, (int*) 0x01BC, (int*) 0x01C6,
                    (int*) 0x01D0, (int*) 0x01DA, (int*) 0x01E4  };


int *RPORs[]   = {  (int*) 0x0000, (int*) 0x06CA, (int*) 0x0000,
                    (int*) 0x06D0, (int*) 0x06D0, (int*) 0x06DE,
                    (int*) 0x0000, (int*) 0x06C2, (int*) 0x06C4,
                    (int*) 0x06C2, (int*) 0x06D4, (int*) 0x06CA,
                    (int*) 0x06D8, (int*) 0x06D6, (int*) 0x06D6,
                    (int*) 0x0000, (int*) 0x06C8, (int*) 0x06C6,
                    (int*) 0x06C8, (int*) 0x06C6, (int*) 0x0000,
                    (int*) 0x0000, (int*) 0x06CE, (int*) 0x0000,
                    (int*) 0x06D2, (int*) 0x0000,
                    (int*) 0x0000, (int*) 0x0000, (int*) 0x0000,
                    (int*) 0x0000, (int*) 0x0000, (int*) 0x0000,
                    (int*) 0x0000, (int*) 0x0000  };

int RPFunc[]   = { 3, 4, 5, 6, 28, 29, 30, 31, 11, 10, 12 }; //?!?

int RPIORPin[] = {  0, 10,  0, 17, 16, 30, -45,  2,  4,  3, 21, 11, 24,
                   23, 22,  0,  8,  6,  9,   7,  0,  0, 14,  0, 18,  0,
                    0,  0,  0,  0,  0,  0,   0,  0 };

int *TCONs[]  = { (int*) 0x0104 , (int*) 0x0110 , (int*) 0x0112 ,
                  (int*) 0x011E , (int*) 0x0120	};

int OCM[]     = { 18, 19, 20, 21, 22, 23, 24, 25, 35 };

int *RPIRs[]  = { (int*) 0x06A4, (int*) 0x06A4, (int*) 0x06A6,
                  (int*) 0x06A6, (int*) 0x06A2, (int*) 0x06AA,
                  (int*) 0x06B6, (int*) 0x06B6, (int*) 0x0682,
                  (int*) 0x0682, (int*) 0x0684, (int*) 0x06AC,
                  (int*) 0x06AC, (int*) 0x06AE, (int*) 0x0688
                };


BOOL RPIRPos[] = {  0, 1, 0,
                    1, 1, 1,
                    0, 1, 0,
                    1, 0, 1,
                    0, 0, 0
                 };
#endif
