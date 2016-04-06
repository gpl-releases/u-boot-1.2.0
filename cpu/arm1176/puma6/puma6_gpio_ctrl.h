/*
 *  puma6_gpio_ctrl
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
 * The file contains the main data structure and API definitions for U-Boot GPIO driver
 *
 */

/** \file   puma6_gpio_ctrl.h
 *  \brief  GPIO config control APIs. 
 *          
 *  \author     Amihay Tabul
 *
 *  \version    0.1     Created
 */

#ifndef _PUMA6_GPIO_CTRL_H_
#define _PUMA6_GPIO_CTRL_H_


/*****************************************************************************
 * GPIO Control
 *****************************************************************************/
#define NO_LED_BIT          (0xFFFFFFFF)

/* Puma6 list of all GPIOs that are used for LED (in intel boards) */
/* GPIO numbers for Intel HarborPark and HarborPark-MG boards */
#define    PUMA6_LED_GPIO_DS           (50)   /* GPIO number 50 is used as DS LED in HP and HP-MG boards */  
#define    PUMA6_LED_GPIO_POWER        (51)   
#ifdef CONFIG_TI_BBU
#define    PUMA6_LED_GPIO_BATTERY      (53)
#else
#define    PUMA6_LED_GPIO_BATTERY      (NO_LED_BIT)
#endif
#define    PUMA6_LED_GPIO_ONLINE       (58)
#define    PUMA6_LED_GPIO_LINK         (59)
#define    PUMA6_LED_GPIO_MOCA         (60)  
#define    PUMA6_LED_GPIO_US           (61)  
#define    PUMA6_LED_GPIO_LINE2        (69)  
#define    PUMA6_LED_GPIO_LINE4        (72)  
#define    PUMA6_LED_GPIO_LINE3        (73)  
#define    PUMA6_LED_GPIO_LINE1        (98)  

//CISCO ADD BEGIN
#define    PUMA6_LED_GPIO_BATTERY      (53)
#define    PUMA6_LED_GPIO_WIFI         (59)
#define    PUMA6_LED_GPIO_WIFI_2_4     (25)
#define    PUMA6_LED_GPIO_WIFI_5_0     (27)
#define    PUMA6_LED_GPIO_DECT	       (35) 
#define    PUMA6_LED_GPIO_WPS	       (89)
#define    PUMA6_LED_GPIO_WPS_REV3     (26) 
//CISCO ADD END

/* GPIO numbers for Intel Falconmine and Catisland boards - only the diffrances from HarborPark are listed */
#define    PUMA6_LED_GPIO_POWER_FM_CI  (52)   /* GPIO number 52 is used as Power LED in FM and CI boards */       
#define    PUMA6_LED_GPIO_DS_FM_CI     (54)     
#define    PUMA6_LED_GPIO_ONLINE_FM_CI (55)
#define    PUMA6_LED_GPIO_LINK_FM_CI   (56)  

/* GPIO numbers for Intel Golden Sprins board - only the diffrances from HarborPark are listed */
#define    PUMA6_LED_GPIO_POWER_GS     (65)        
#define    PUMA6_LED_GPIO_DS_GS        (64)     
#define    PUMA6_LED_GPIO_ONLINE_GS    (44)
#define    PUMA6_LED_GPIO_LINK_GS      (45)  

/* Puma6 list of all GPIOs (that are not LED) that are used in intel boards */
#define    PUMA6_TUNER_RESET_GPIO      (97)
#define    PUMA6_ADI_RESET		       (42)


typedef enum PumaGpioTypes
{
    FUNCTIONAL_PIN  = 0,
    GPIO_PIN        = 1,
    GPIO_OUTPUT_PIN = 0,
    GPIO_INPUT_PIN  = 1,
} PumaGpioTypes_t;

int puma6_gpioInit(unsigned int puma6_boardtype_id);
int puma6_gpioInBit(unsigned int gpio_pin);
int puma6_gpioOutBit(unsigned int gpio_pin, unsigned int value);
int puma6_gpioCtrl(unsigned int gpio_pin, PumaGpioTypes_t pin_mode, PumaGpioTypes_t pin_direction);

#endif /* _PUMA6_GPIO_CTRL_H_ */
