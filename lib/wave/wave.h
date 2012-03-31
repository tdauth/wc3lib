/*****************************************************************************/
/* Wave.h                                 Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* Header file for WAVe unplode functions                                    */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 25.04.10  1.01  Tamino Dauth  Made independent.                           */
/* 31.03.03  1.00  Lad  The first version of Wave.h                          */
/*****************************************************************************/

#ifndef __WAVE_H__
#define __WAVE_H__

//-----------------------------------------------------------------------------
// Functions

int  CompressADPCM(unsigned char * pbOutBuffer, int dwOutLength, short * pwInBuffer, int dwInLength, int nCmpType, int nChannels);
int  DecompressADPCM(unsigned char * pbOutBuffer, int dwOutLength, unsigned char * pbInBuffer, int dwInLength, int nChannels);

#endif // __WAVE_H__
