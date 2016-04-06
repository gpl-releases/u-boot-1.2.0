/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2011 Intel Corporation.
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


/*
 * This file implements board specific initilization routines.
 *
 */

#include <common.h>
#include <malloc.h>

#ifdef CONFIG_SPI_FLASH
#include <spi.h>
#endif
#ifdef CONFIG_GENERIC_MMC
#include <mmc.h>
#include <part.h>
#endif

#include <puma6.h>
#include <docsis_ip_cru_registers.h>
#include <docsis_ip_boot_params.h>
#include <puma6_hw_mutex.h>
#include <puma6_gpio_ctrl.h>
#include <active_image_designator.h>
#include <mmc_utils.h>


DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_TI_BBU
extern void bbu_init(int printEnableParam);
#endif



/* Print Docsis IP Boot Parameters */
void print_boot_params(void);

/* Update environment variable as hex unsigned long */ 
static void setenv_hex(char *name, unsigned long value);
/* Update environment variable as decimal integer */
static void setenv_dec(char * name,int val);

#ifdef CONFIG_GENERIC_MMC
/* convert mmc partiton number to offset in flash */
static int mmc_part_offset(int part);
/* Swap Endian */
static inline unsigned int mmc_swap_dword(unsigned int x); 
#endif


/* Set watchdog disable register to specified state */
int set_watchdog_disable_reg (int state)
{
    int tmo;
    volatile unsigned int* disable_lock_reg = (volatile unsigned int *)REG_WT_DISABLE_LOCK;
    volatile unsigned int* disable_reg = (volatile unsigned int *)REG_WT_DISABLE;

    /* Unlock stage 1 */
    *disable_lock_reg = WT_DISABLE_UNLOCK_WORD1;
    for (tmo = 0xff; tmo > 0; tmo--) {
        if ((*disable_lock_reg & 3) == 1)
            break;
    }

    /* Unlock stage 2 */
    *disable_lock_reg = WT_DISABLE_UNLOCK_WORD2;
    for (tmo = 0xff; tmo > 0; tmo--) {
        if ((*disable_lock_reg & 3) == 2)
            break;
    }

    /* Unlock stage 3 */
    *disable_lock_reg = WT_DISABLE_UNLOCK_WORD3;
    for (tmo = 0xff; tmo > 0; tmo--) {
        if ((*disable_lock_reg & 3) == 3)
            break;
    }

    /* Set disable reg  */
    *disable_reg = (*disable_reg & ~1) | (state & 1);

    return(0);
}


/*
 * Miscellaneous platform dependent initialisations
 */

int board_init (void)
{
   /* MACH_TYPE_PUMA6 - For Linux boot*/
    gd->bd->bi_arch_number = MACH_TYPE_PUMA6;

    /* address of Linux boot parameters */
    gd->bd->bi_boot_params = PHYS_KERNEL_PARAMS_ADDRESS;

    /* Enable UART2 output Mux*/
    *((volatile unsigned int *) BOOTCFG_DOCSIS_IP_IO_ENABLE_REG) |= DOCSIS_IP_IO_ENABLE_UART2_MASK;

    /* CRU Enable */
    CRU_MOD_STATE(CRU_NUM_UART2) = CRU_MOD_STATE_ENABLE;
    CRU_MOD_STATE(CRU_NUM_TIMER0) = CRU_MOD_STATE_ENABLE;

    /* Disable Watch-dog , To check if we need to enable the WDT module with CRU*/
    set_watchdog_disable_reg (WT_STATE_DISABLED);

    icache_enable ();

    /* Init the HW Mutex. This must be done before any SPI or eMMC work. */
    hw_mutex_init();

#ifdef CONFIG_SPI_FLASH
    /* Lock the HW Mutex */
    if (hw_mutex_lock(HW_MUTEX_NOR_SPI) == 0)
    {
        return 1;
    }
    /* Initilize SPI till this point we dont access SPI BUS :-) */
    spi_init();

    /* Release HW Mutes */
    hw_mutex_unlock(HW_MUTEX_NOR_SPI);
#endif

    /* temp test */
    /* SET_BOOT_PARAM_REG( BOOT_MODE , BOOT_MODE_SPI); */

#ifdef CONFIG_TI_BBU
    /* Initialize BBU */
    bbu_init(0);
#endif

    return 0;
}

/******************************
 Routine:
 Description:
******************************/
int dram_init (void)
{
    gd->bd->bi_dram[0].start = BOOT_PARAM_DWORD_READ(ARM11_DDR_OFFSET) + DDR_BASE_ADDR;
    gd->bd->bi_dram[0].size  = BOOT_PARAM_DWORD_READ(ARM11_DDR_SIZE);
    return 0;
}

#include <command.h>
static unsigned int op_to_data (char* s)
{
    unsigned int data, *ptr;

    /* if the parameter starts with a * then assume is a pointer to the value we want */
    if (s[0] == '*') {
        ptr = (unsigned int *)simple_strtoul(&s[1], NULL, 16);
        data = *ptr;
    } else {
        data = simple_strtoul(s, NULL, 16);
    }

    return data;
}

/* command line interface to for addition and subtraction */
int do_eval (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
#define EVAL_OPS        2
#define EVAL_OP_ADD     0
#define EVAL_OP_SUB     1

    int     rc = 1, value = 0, len, i;
    unsigned int op1, op2;
    char*   op_tbl[EVAL_OPS] = {"+", "-"};
    char evalval[12];

    /* Validate arguments */
    if ((argc < 4)){
            printf("Usage:\n%s\n", cmdtp->usage);
            return rc;
    }

    len = strlen (argv[2]);

    for (i = 0; i < EVAL_OPS; i++) {
        if ((strncmp (argv[2], op_tbl[i], len) == 0) && (len == strlen (op_tbl[i])))
        {
            op1 = op_to_data (argv[1]);
            op2 = op_to_data (argv[3]);

            switch (i)
            {
                case EVAL_OP_ADD:
                    value = op1 + op2;
                    rc = 0;
                    break;
                case EVAL_OP_SUB:
                    value = (int) (op1 - op2);
                    rc = 0;
                    break;
                default:
                    printf("Unsupported operation '%s'\n", argv[2]);
                    break;
            }
            break;
        }
        else
        {
#if 0
            printf ("arg: %s, len: %d\n", argv[2], len);
            printf ("op: %s, len: %d\n", op_tbl[i],  strlen (op_tbl[i]));
#endif
        }
    }

    if (i >= EVAL_OPS)
        printf("Unknown/Unsupported operator '%s'\n", argv[2]);
    else if (rc == 0) {
        sprintf (evalval, "0x%x", value);
        setenv("evalval", evalval);
    }

    return rc;
}

U_BOOT_CMD(
    eval, CFG_MAXARGS, 0, do_eval,
    "eval\t- return addition/subraction\n",
    "[*]value1 <op> [*]value2\n"
);


static void setenv_dec(char * name,int val)
{
    char val_str[12];

    sprintf (val_str, "%d", val);
    setenv (name, val_str);

    return;
}

static void setenv_hex(char *name, unsigned long value)
{
    char hex_str[12];

    sprintf (hex_str, "0x%0.8X", value);
    setenv (name, hex_str);

    return;
}


static void generate_env_vals (void)
{
    /* Setup boardtype according to boot param (set by the atom cpu) */
    setenv_hex("boardtype",BOOT_PARAM_LONG_READ(BOARD_TYPE));    
    
    printf("Setting Board-Type to %d\n",BOOT_PARAM_LONG_READ(BOARD_TYPE));
    

    setenv_hex("bootmode",                BOOT_PARAM_LONG_READ(BOOT_MODE));
    setenv_hex("ramoffset",               BOOT_PARAM_LONG_READ(ARM11_DDR_OFFSET));
    setenv_hex("ramsize",                 BOOT_PARAM_LONG_READ(ARM11_DDR_SIZE));
    setenv_dec("active_aid",              BOOT_PARAM_LONG_READ(ACTIVE_AID));
    setenv_hex("aid1offset",              BOOT_PARAM_LONG_READ(AID_1_OFFSET));
    setenv_hex("aid2offset",              BOOT_PARAM_LONG_READ(AID_2_OFFSET));
    setenv_hex("ubootoffset",             BOOT_PARAM_LONG_READ(ARM11_UBOOT_OFFSET));
    setenv_hex("ubootsize",               BOOT_PARAM_DWORD_READ(ARM11_UBOOT_SIZE));
    setenv_hex("envoffset1",              BOOT_PARAM_DWORD_READ(ARM11_ENV1_OFFSET));
    setenv_hex("envoffset2",              BOOT_PARAM_DWORD_READ(ARM11_ENV2_OFFSET));
    setenv_hex("envsize",                 BOOT_PARAM_DWORD_READ(ARM11_ENV_SIZE));
    setenv_hex("arm11ubfioffset1",        BOOT_PARAM_DWORD_READ(ARM11_UBFI1_OFFSET));
    setenv_hex("arm11ubfisize1",          BOOT_PARAM_DWORD_READ(ARM11_UBFI1_SIZE));
    setenv_hex("arm11ubfioffset2",        BOOT_PARAM_DWORD_READ(ARM11_UBFI2_OFFSET));
    setenv_hex("arm11ubfisize2",          BOOT_PARAM_DWORD_READ(ARM11_UBFI2_SIZE));
    setenv_hex("atomubfioffset1",         BOOT_PARAM_DWORD_READ(ATOM_UBFI1_OFFSET));
    setenv_hex("atomubfisize1",           BOOT_PARAM_DWORD_READ(ATOM_UBFI1_SIZE));
    setenv_hex("atomubfioffset2",         BOOT_PARAM_DWORD_READ(ATOM_UBFI2_OFFSET));
    setenv_hex("atomubfisize2",           BOOT_PARAM_DWORD_READ(ATOM_UBFI2_SIZE));
    setenv_hex("arm11nvramoffset",        BOOT_PARAM_DWORD_READ(ARM11_NVRAM_OFFSET));
    setenv_hex("arm11nvramsize",          BOOT_PARAM_DWORD_READ(ARM11_NVRAM_SIZE));
	setenv_hex("silicon_stepping",		  BOOT_PARAM_DWORD_READ(SILICON_STEPPING));
	setenv_hex("cefdk_version",			  BOOT_PARAM_DWORD_READ(CEFDK_VERSION));
	setenv_hex("signature1_offset",		  BOOT_PARAM_DWORD_READ(SIGNATURE1_OFFSET));
    setenv_hex("signature2_offset",		  BOOT_PARAM_DWORD_READ(SIGNATURE2_OFFSET));
    setenv_hex("signature_size",		  BOOT_PARAM_DWORD_READ(SIGNATURE_SIZE));
    setenv_hex("signature_number",		  BOOT_PARAM_DWORD_READ(SIGNATURE_NUMBER));
	setenv_hex("emmc_flash_size",		  BOOT_PARAM_DWORD_READ(EMMC_FLASH_SIZE));
    setenv_dec("mmc_part_arm11_kernel_0", BOOT_PARAM_BYTE_READ( ARM11_KERNEL_1_EMMC_PARTITION));
    setenv_dec("mmc_part_arm11_kernel_1", BOOT_PARAM_BYTE_READ( ARM11_KERNEL_2_EMMC_PARTITION));
    setenv_dec("mmc_part_arm11_rootfs_0", BOOT_PARAM_BYTE_READ( ARM11_ROOTFS_1_EMMC_PARTITION));
    setenv_dec("mmc_part_arm11_rootfs_1", BOOT_PARAM_BYTE_READ( ARM11_ROOTFS_2_EMMC_PARTITION));
    setenv_dec("mmc_part_arm11_gw_fs_0",  BOOT_PARAM_BYTE_READ( ARM11_GW_FS_1_EMMC_PARTITION));
    setenv_dec("mmc_part_arm11_gw_fs_1",  BOOT_PARAM_BYTE_READ( ARM11_GW_FS_2_EMMC_PARTITION));
    setenv_dec("mmc_part_arm11_nvram",    BOOT_PARAM_BYTE_READ( ARM11_NVRAM_EMMC_PARTITION));
    setenv_dec("mmc_part_arm11_nvram_2",  BOOT_PARAM_BYTE_READ( ARM11_NVRAM_2_EMMC_PARTITION ));
    setenv_dec("mmc_part_atom_kernel_0",  BOOT_PARAM_BYTE_READ( ATOM_KERNEL_1_EMMC_PARTITION ));
    setenv_dec("mmc_part_atom_kernel_1",  BOOT_PARAM_BYTE_READ( ATOM_KERNEL_2_EMMC_PARTITION ));
    setenv_dec("mmc_part_atom_rootfs_0",  BOOT_PARAM_BYTE_READ( ATOM_ROOTFS_1_EMMC_PARTITION ));
    setenv_dec("mmc_part_atom_rootfs_1",  BOOT_PARAM_BYTE_READ( ATOM_ROOTFS_2_EMMC_PARTITION ));
    
 
    return ;
}

static void generate_actimage (void)
{
    int aid_offset = 0;
    int aid_active = 0;
    active_image_designator aid = {0};

/* Get Active AID offset */
    aid_active = BOOT_PARAM_DWORD_READ(ACTIVE_AID);
    switch (aid_active)
    {
    case 1:
        aid_offset = BOOT_PARAM_DWORD_READ(AID_1_OFFSET);
        break;
    case 2:
        aid_offset = BOOT_PARAM_DWORD_READ(AID_2_OFFSET);
        break;
    default:
        printf("Error: invelid 'Active AID', ACTIMAGE is not set.\n");
        return;
    }
    printf("Read AID %d \n",aid_active);

    switch (BOOT_PARAM_LONG_READ( BOOT_MODE)) 
    {
    case BOOT_MODE_eMMC:
#ifndef CONFIG_GENERIC_MMC
        printf("Error uboot does not support mmc.\n");
#else
        setenv("verify","n");      /* Disable CRC check on image loading */
        setenv("bootdevice","mmc");  /* Enable loading image from mmc */

        setenv_hex("UBFIADDR1", mmc_part_offset(BOOT_PARAM_BYTE_READ( ARM11_KERNEL_1_EMMC_PARTITION)));
        setenv_hex("UBFIADDR2", mmc_part_offset(BOOT_PARAM_BYTE_READ( ARM11_KERNEL_2_EMMC_PARTITION)));

        /* Read AID structure from flash */
        mmc_read_buff(aid_offset,(unsigned int*)&aid,sizeof(active_image_designator));
        

#endif
        break;
    
    case BOOT_MODE_SPI:
        setenv("verify","y");      /* Enable CRC check on image loading (default)*/
        setenv("bootdevice","spi");  /* Enable loading image from spi */
        setenv_hex("UBFIADDR1", BOOT_PARAM_DWORD_READ(ARM11_UBFI1_OFFSET) + CFG_FLASH_BASE);
        setenv_hex("UBFIADDR2", BOOT_PARAM_DWORD_READ(ARM11_UBFI2_OFFSET) + CFG_FLASH_BASE);
        /* Read AID structure from flash */
        
        /* memmove is used to read from SPI/NOR flash, and it use HW Mutex protection */
        memmove((void *)&aid,(const void *)(aid_offset+ CFG_FLASH_BASE),(size_t)sizeof(active_image_designator));

        break;
    default:
        setenv("bootdevice","none");  /* no flash */
        setenv("verify","y"); 
    }

    /* Set All AID structure to enviroment variables */
    setenv_dec("actimage_atom_kernel",mmc_swap_dword(aid.actimage[AID_IA_KERNEL])  +1);
    setenv_dec("actimage_atom_rootfs",mmc_swap_dword(aid.actimage[AID_IA_ROOT_FS]) +1);
    setenv_dec("actimage_atom_vgfs",  mmc_swap_dword(aid.actimage[AID_IA_VGW_FS])  +1);
    setenv_dec("actimage_arm_kernel", mmc_swap_dword(aid.actimage[AID_ARM_KERNEL]) +1);
    setenv_dec("actimage_arm_rootfs", mmc_swap_dword(aid.actimage[AID_ARM_ROOT_FS])+1);
    setenv_dec("actimage_arm_gwfs",   mmc_swap_dword(aid.actimage[AID_ARM_GW_FS])  +1);
/* 
Not In Use: 
    setenv_dec("actimage_res6",       mmc_swap_dword(aid.actimage[AID_RSVD_6])     +1);
    setenv_dec("actimage_res7",       mmc_swap_dword(aid.actimage[AID_RSVD_7])     +1);
    setenv_dec("actimage_res8",       mmc_swap_dword(aid.actimage[AID_RSVD_8])     +1);
    setenv_dec("actimage_res9",       mmc_swap_dword(aid.actimage[AID_RSVD_9])     +1);
    setenv_dec("actimage_res10",      mmc_swap_dword(aid.actimage[AID_RSVD_10])    +1);
    setenv_dec("actimage_res11",      mmc_swap_dword(aid.actimage[AID_RSVD_11])    +1);
    setenv_dec("actimage_res12",      mmc_swap_dword(aid.actimage[AID_RSVD_12])    +1);
    setenv_dec("actimage_res13",      mmc_swap_dword(aid.actimage[AID_RSVD_13])    +1);
    setenv_dec("actimage_res14",      mmc_swap_dword(aid.actimage[AID_RSVD_14])    +1);
    setenv_dec("actimage_res15",      mmc_swap_dword(aid.actimage[AID_RSVD_15])    +1);
*/

    /* Set ACTIMAGE to enviroment variables */
    setenv_dec("ACTIMAGE",        mmc_swap_dword(aid.actimage[AID_ARM_KERNEL])+1);
    printf("set ACTIMAGE to %d\n",mmc_swap_dword(aid.actimage[AID_ARM_KERNEL])+1);

}

int board_late_init (void)
{
    char *env;
    int bootparams_autoupdate = 0;
    int bootparams_autoprint = 0;

    /* Set DDR Base and Flash Base address */
    setenv_hex("flashbase",CFG_FLASH_BASE);
    setenv_hex("rambase",DDR_BASE_ADDR);

    /* Disable Boot Parameters auto print only if BOOTPARAMS_AUTOPRINT exist and it equal to 'off' */
    env = getenv ("BOOTPARAMS_AUTOPRINT");
    bootparams_autoprint = (env && (strcmp(env,"off") == 0)) ? 0 : 1;
    if (bootparams_autoprint == 1)
    {
        /* Print Boot Params */
        print_boot_params();
    }


    /* Disable Boot Parameters auto update only if BOOTPARAMS_AUTOUPDATE exist and it equal to 'off' */
    env = getenv ("BOOTPARAMS_AUTOUPDATE");
    bootparams_autoupdate = (env && (strcmp(env,"off") == 0)) ? 0 : 1;
    if (bootparams_autoupdate == 1)
    {
        /* Set environments variables from boot parameters */ 
        generate_env_vals();

        /* Set Active Image From Boot Params */
        generate_actimage();
    }
    else
    {
        printf("WARNING: BOOTPARAMS_AUTOUPDATE is equal to 'off', u-boot do not update Boot Parameters automatically.\n");
    }

    /*
     * GPIO init - Set Output for all LED and other GPIO needed. also turn On power LED!
     */
    puma6_gpioInit(BOOT_PARAM_DWORD_READ(BOARD_TYPE));

    
    /* Print warning if Boot Param RAM Base address is not the same as the address that was use for compilation */
    if ((BOOT_PARAM_LONG_READ( ARM11_DDR_OFFSET ) + DDR_BASE_ADDR) != PHYS_SDRAM_1 )
    {
        printf(" !!! Boot Param ARM11 RAM Offset (0x%0.8X), is not the same as the address that was use for compilation (0x%0.8X)\n",
               BOOT_PARAM_LONG_READ( ARM11_DDR_OFFSET ),PHYS_SDRAM_1 - DDR_BASE_ADDR);
    }

    return 0;
}

/* Get SoC revision - This info will pass to the kernel */
u32 get_board_rev(void)
{
    return MACH_TYPE_PUMA6; /* TBD Puma6 - Use the ATOM boot-param to get the SoC rev ! */
}


/* Print Docsis IP Boot Parameters */
void print_boot_params(void)
{
    /* Print Boot Params */
    printf("\n Docsis IP Boot Params \n");
    printf(" =====================\n");
    printf(" Boot Params Version ....... 0x%0.8X \n", BOOT_PARAM_LONG_READ( BOOT_PARAM_VER                ));
    printf(" ARM11 Boot Status ......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_BOOT_STATUS             ));
    printf(" Boot Mode ................. 0x%0.8X \n", BOOT_PARAM_LONG_READ( BOOT_MODE                     ));
    printf(" Board Type ................ 0x%0.8X \n", BOOT_PARAM_LONG_READ( BOARD_TYPE                    ));
    printf(" Numebr of flashes ......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( NUMBER_OF_FLASHES             ));
    printf(" ARM11 RAM Offset .......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_DDR_OFFSET              ));
    printf(" ARM11 RAM Size ............ 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_DDR_SIZE                ));
    printf(" Active AID     ............ 0x%0.8X \n", BOOT_PARAM_LONG_READ( ACTIVE_AID                    ));
    printf(" AID 1 Offset............... 0x%0.8X \n", BOOT_PARAM_LONG_READ( AID_1_OFFSET                  ));
    printf(" AID 2 Offset .............. 0x%0.8X \n", BOOT_PARAM_LONG_READ( AID_2_OFFSET                  ));
    printf(" ARM11 Uboot Offset ........ 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_UBOOT_OFFSET            ));
    printf(" ARM11 Uboot Size .......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_UBOOT_SIZE              ));
    printf(" ARM11 Env1 Offset ......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_ENV1_OFFSET             ));
    printf(" ARM11 Env2 Offset ......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_ENV2_OFFSET             ));
    printf(" ARM11 Env Size ............ 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_ENV_SIZE                ));
    printf(" ARM11 NVRAM Offset ........ 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_NVRAM_OFFSET            ));
    printf(" ARM11 NVRAM Size .......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_NVRAM_SIZE              ));
    printf(" ARM11 UBFI1 Offset ........ 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_UBFI1_OFFSET            ));
    printf(" ARM11 UBFI1 Size .......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_UBFI1_SIZE              ));
    printf(" ARM11 UBFI2 Offset ........ 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_UBFI2_OFFSET            ));
    printf(" ARM11 UBFI2 Size .......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ARM11_UBFI2_SIZE              ));
    printf(" ATOM UBFI1 Offset ......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ATOM_UBFI1_OFFSET             ));
    printf(" ATOM UBFI1 Size ........... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ATOM_UBFI1_SIZE               ));
    printf(" ATOM UBFI2 Offset ......... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ATOM_UBFI2_OFFSET             ));
    printf(" ATOM UBFI2 Size ........... 0x%0.8X \n", BOOT_PARAM_LONG_READ( ATOM_UBFI2_SIZE               ));
    printf(" ARM11 Kernel 0 partition .. 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ARM11_KERNEL_1_EMMC_PARTITION ));
    printf(" ARM11 Kernel 1 partition... 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ARM11_KERNEL_2_EMMC_PARTITION ));
    printf(" ARM11 Root FS 0 partition . 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ARM11_ROOTFS_1_EMMC_PARTITION ));
    printf(" ARM11 Root FS 1 partition . 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ARM11_ROOTFS_2_EMMC_PARTITION ));
    printf(" ARM11 GW FS 0 partition ... 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ARM11_GW_FS_1_EMMC_PARTITION  ));
    printf(" ARM11 GW FS 1 partition ... 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ARM11_GW_FS_2_EMMC_PARTITION  ));
    printf(" ARM11 NVRAM partition ..... 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ARM11_NVRAM_EMMC_PARTITION    ));
    printf(" ARM11 NVRAM 2 partition ... 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ARM11_NVRAM_2_EMMC_PARTITION  ));
    printf(" ATOM Kernel 0 partition ... 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ATOM_KERNEL_1_EMMC_PARTITION  ));
    printf(" ATOM Kernel 1 partition ... 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ATOM_KERNEL_2_EMMC_PARTITION  ));
    printf(" ATOM Root FS 0 partition .. 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ATOM_ROOTFS_1_EMMC_PARTITION  ));
    printf(" ATOM Root FS 1 partition .. 0x%0.2X \n", BOOT_PARAM_BYTE_READ( ATOM_ROOTFS_2_EMMC_PARTITION  ));
	printf(" Silicon Stepping........... 0x%0.8x \n", BOOT_PARAM_LONG_READ( SILICON_STEPPING			  ));
	printf(" CEFDK Version.............. 0x%0.8x \n", BOOT_PARAM_LONG_READ( CEFDK_VERSION				  ));
	printf(" Signature 0 Offset..........0x%0.8x \n", BOOT_PARAM_LONG_READ( SIGNATURE1_OFFSET			  ));
    printf(" Signature 1 Offset..........0x%0.8x \n", BOOT_PARAM_LONG_READ( SIGNATURE2_OFFSET			  ));
    printf(" Signature Size..............0x%0.8x \n", BOOT_PARAM_LONG_READ( SIGNATURE_SIZE 	    		  ));
    printf(" Signature number ...........0x%0.8x \n", BOOT_PARAM_LONG_READ( SIGNATURE_NUMBER			  ));
	printf(" EMMC Flash Size............ 0x%0.8x \n", BOOT_PARAM_LONG_READ( EMMC_FLASH_SIZE				  ));

    printf("\n");
}

int do_bpinfo (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    print_boot_params();
    return 0;
}

U_BOOT_CMD(
    bpinfo, CFG_MAXARGS, 0, do_bpinfo,
    "bpinfo\t - Print Docsis IP Boot Parameters\n",
    " - No help available.\n"
);

int do_cli_hw_mutex (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    hw_mutex_device_type resource_id;

    if (argc < 3) 
    {
		printf ("Usage:\n"
                "hwmutex [t/r] [mmc/spi/mail]\n"
                "Option:\n"
                "   t    - Take the mutex (hung forever until receive the mutex).\n"
                "   r    - Release the mutex.\n"
                "   mmc  - emmc hw mutex.\n"
                "   spi  - spi hw mutex.\n"
                "   mail - Arm11 mailbox hw mutex.\n"
                "   gpio - gpio hw mutex.\n");
		return 1;
	}

    if (strcmp(argv[2], "mmc") == 0)
    {
        resource_id = HW_MUTEX_EMMC;
        printf("eMMC HW Mutex resource id %d\n",resource_id);
    }
    if (strcmp(argv[2], "spi") == 0)
    {
        resource_id = HW_MUTEX_NOR_SPI;
        printf("SPI HW Mutex resource id %d\n",resource_id);
    }
    if (strcmp(argv[2], "mail") == 0)
    {
        resource_id = HW_MUTEX_ARM_MBX;
        printf("MailBox HW Mutex resource id %d\n",resource_id);
    }

    if (strcmp(argv[2], "gpio") == 0)
    {
        resource_id = HW_MUTEX_GPIO;
        printf("GPIO HW Mutex resource id %d\n",resource_id);
    }


	if (strcmp(argv[1], "t") == 0)
    {
        /* Lock the HW Mutex */
        if (hw_mutex_lock(resource_id) == 0)
        {
            printf("Fail to lock HW Mutex!\n");
            return 0;
        }
        printf("HW Mutex ID %d locked\n",resource_id);
        return 0;
    }

    if (strcmp(argv[1], "r") == 0)
    {
        /* Release HW Mutes */
        hw_mutex_unlock(resource_id);
        printf("HW Mutex ID %d unlocked\n",resource_id);
    }
    return 0;
}

U_BOOT_CMD(
    hwmutex, 3, 0, do_cli_hw_mutex,
    "hwmutex\t - Use the HW Mutex [t/r] [mmc/spi/mail]\n",
	"hwmutex [t/r] [mmc/spi/mail/gpio] - control the HW mutex t-Take the mutex r-Release the mutex\n"
);



#ifdef CONFIG_GENERIC_MMC
int board_mmc_init(bd_t *bd)
{
    sdhci_puma6_init();
    return 0;
}

static int mmc_part_offset(int part)
{
    int curr_device = 0;
	struct mmc *mmc;
    block_dev_desc_t *mmc_dev;
    disk_partition_t info = {0};

    if (get_mmc_num() > 0)
        curr_device = CONFIG_SYS_MMC_IMG_DEV;
    else {
        puts("No MMC device available\n");
        return 0;
    }
    
    if (part > PART_ACCESS_MASK) {
        printf("#part_num shouldn't be larger than %d\n", PART_ACCESS_MASK);
        return 0;
    }

	mmc = find_mmc_device(curr_device);

	if (mmc) {
		mmc_init(mmc);
        mmc_dev = mmc_get_dev(curr_device);
        if (mmc_dev != NULL)
        {
            init_part(mmc_dev);
    		if (mmc_dev->type != DEV_TYPE_UNKNOWN) {
                if (get_partition_info (mmc_dev, part, &info) == 0){
                    return info.start*info.blksz;
                }
                
                printf("partition %d not found\n",part);
                return 0;
    		}
            printf("unknown partition type\n");
            return 0;
        }
		puts("get mmc type error!\n");
		return 0;
	} else {
		printf("no mmc device at slot %x\n", curr_device);
		return 0;
	}
}
#endif

/* Endian swap for 32bits double word */
inline unsigned int mmc_swap_dword(unsigned int x) 
{ 
    int swp = x;
    return ( ((swp&0x000000FF)<<24) + ((swp&0x0000FF00)<<8 ) +
             ((swp&0x00FF0000)>>8 ) + ((swp&0xFF000000)>>24) );
}

