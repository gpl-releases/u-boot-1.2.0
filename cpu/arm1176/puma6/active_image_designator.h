
/*
 *  active_image_designator.h
 *
 *  GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2012 Intel Corporation. All rights reserved.
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *  The full GNU General Public License is included in this distribution
 *  in the file called LICENSE.GPL.
 *
 *  Contact Information:
 *    Intel Corporation
 *    2200 Mission College Blvd.
 *    Santa Clara, CA  97052
 *
 */

#ifndef _ACTIVE_IMAGE_DESIGNATOR_H_
#define _ACTIVE_IMAGE_DESIGNATOR_H_


#define AID_IA_KERNEL     0
#define AID_IA_ROOT_FS    1
#define AID_IA_VGW_FS     2
#define AID_ARM_KERNEL    3
#define AID_ARM_ROOT_FS   4
#define AID_ARM_GW_FS     5
#define AID_RSVD_6        6
#define AID_RSVD_7        7
#define AID_RSVD_8        8
#define AID_RSVD_9        9
#define AID_RSVD_10      10
#define AID_RSVD_11      11
#define AID_RSVD_12      12
#define AID_RSVD_13      13
#define AID_RSVD_14      14
#define AID_RSVD_15      15

#define AID_MAX_IMGAGES 16

typedef struct _active_image_designator {
    unsigned int crc32;    /* crc32 of 'valid' and 'actimage' */
    unsigned int not_valid;    /* state if this AID structure is valid or not. Valid=0  */
    unsigned int actimage[AID_MAX_IMGAGES]; /* the active image 0 or 1 */
}active_image_designator;


#endif /* _ACTIVE_IMAGE_DESIGNATOR_H_ */
