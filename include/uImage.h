/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2012 Intel Corporation.
 *
 *  This program is free software; you can redistribute it and/or modify 
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but 
 *  WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 *  General Public License for more details.
 *
 *
 *  You should have received a copy of the GNU General Public License 
 *  along with this program; if not, write to the Free Software 
 *  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *  The full GNU General Public License is included in this distribution 
 *  in the file called LICENSE.GPL.
 *
 *  Contact Information:
 *  Intel Corporation
 *  2200 Mission College Blvd.
 *  Santa Clara, CA  97052
 */

/***************************************************************************/

/*! \file uImage.h
 *  \brief Interface for Unified Image Header

****************************************************************************/

#ifndef _UIMAGE_H
#define _UIMAGE_H



/**************************************************************************/
/*      INCLUDES                                                          */
/**************************************************************************/

/**************************************************************************/
/*      INTERFACE  Defines and Structs                                    */
/**************************************************************************/
#define MAX_PART_NUM          10

#define MAX_NAME_LEN          20       

#define IMAGE_MAGIC  ('I' << 24 | 'M' << 16 | 'A' << 8 | 'G')

struct uImageHeader {
	uint32_t magic;
	char name[MAX_NAME_LEN];
	uint32_t date; /* UTC date (year-1900) << 16 | month << 8 | day */
	uint32_t time; /* UTC time hour << 16 | minute << 8 | second */
	uint32_t version; /*image version*/
	uint32_t imageSize;
	uint32_t partNum;
	uint32_t hCrc; /*image header CRC*/
	uint32_t dCrc; /*all data CRC*/
	uint32_t pCrc[MAX_PART_NUM]; /*partition CRC*/
	uint32_t size[MAX_PART_NUM]; /*partition size*/
	uint32_t reserve[7];
}__attribute__((packed));

#define UIMAGE_HEADER_SIZE  (sizeof(struct uImageHeader))



#endif


