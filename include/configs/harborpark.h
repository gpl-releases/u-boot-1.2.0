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

#ifndef __HARBORPARK_H
#define __HARBORPARK_H

#include <puma.h>

#define XMK_STR(x)  #x
#define MK_STR(x)   XMK_STR(x)

#ifndef __ASSEMBLY__
extern unsigned int g_uboot_partition_size;
extern unsigned int g_boot_param_env1_base_offset;
extern unsigned int g_boot_param_env2_base_offset;
extern unsigned int g_boot_param_env_size;
#endif

/* FOR SLE UART */
//#define CONFIG_SLE_UART

/* define printf support long long prints (64bits) */
#define CFG_64BIT_VSPRINTF	1

#define CONFIG_HARBORPARK
#define CONFIG_USE_HW_MUTEX

#define CONFIG_IDENT_STRING  "Puma6 - "CONFIG_IDENT_STRING_PUMA


//CISCO ADD BEGIN
#undef CONFIG_IDENT_STRING
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define CONFIG_IDENT_STRING " Cisco-Boot "TOSTRING(CONFIG_VENDOR_UBOOT_VERSION)
//CISCO ADD END

/* Enable this to boost SPI clock to 80MHz on supporting devices */
/* #define CONFIG_ENABLE_SPI_CLOCK_BOOST */

/**
 * U-boot generic defines
 */
#undef CONFIG_USE_IRQ
#undef CONFIG_NET_MULTI
#undef CONFIG_NET_RETRY_COUNT

/* No Ethernet no Puma6 */
#define CONFIG_NOETH

#ifdef CONFIG_MACH_PUMA6_FPGA
#warning "CONFIG_MACH_PUMA6_FPGA is defined"
/* Use FPGA Clock (40MHz) for UART and timers,  */
/* else use real SoC clock (450MHz) */
/* must be removed on real SoC */
#define USE_FPGA_CLK 1

/* Enable SPI Controller Initialization for FPGA testing. */
/* Normmally done by ATOM and must be removed on real SoC*/
#define ENABLE_SPI_INIT 1
#endif

/* Enable SPI Test commands on u-boot shell */
/* Recommend to disable on real SoC product */
#define CMD_SPI_TESTS 1

/* Enable SPI Controller debug output */
#define SPI_DEBUG 1

/* Enable Debug to SRAM (offset 0x1100) in start.s, can be safely removed */
#define START_S_DEBUG 1

/* Enable FPGA test features */
//#define CONFIG_MMC_FPGA_TEST  1  

/*
 * UBFI params
 */

/* UBFI_LINUX_LOAD_ADDR_FIX - The addr (in physical memory) that the U-Boot needs to copy the kernel to   */
/*                            This information is store in the UBFI header (image file).                  */
/*                            In Puma5 SoC we put the full (vs. an offset in RAM) so the fix is zero.     */
/*                            In Puma6 SoC we put the offset in RAM (vs. the full address like in Puma-5) */
/*                            so the fix is the RAM base address                                          */
#define UBFI_LINUX_LOAD_ADDR_FIX    (CONFIG_ARM_AVALANCHE_SDRAM_ADDRESS)

/*
 * MACH_TYPE
 */
#define MACH_TYPE_PUMA6             (1505)

/***************************************************************************************************/
/***    Physical Memory Map                                                                        */
/***************************************************************************************************/

/*
 * SDRAM/DDR Definitions
 */
#define CONFIG_NR_DRAM_BANKS    (1)
#define DDR_BASE_ADDR           (0x40000000)
#define PHYS_SDRAM_1            (CONFIG_ARM_AVALANCHE_SDRAM_ADDRESS)
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ    (4*1024)        /* IRQ stack size*/
#define CONFIG_STACKSIZE_FIQ    (4*1024)        /* FIQ stack size*/
#endif

/*
 * Flash Memory definitions
 */
#define CFG_MAX_FLASH_BANKS     (2)           /* max num of memory banks */
#define CFG_FLASH_BASE          (0x08000000)  /* Flash base address */
#define CFG_FLASH_SIZE          (0x04000000)  /* Flash window size - 64MB */
#define CFG_MAX_FLASH_SECT      (4096)        /* Max sector count, must be define for u-boot */

/* MMC Memory definitions */
#define CONFIG_SYS_MMC_BASE     (0x0FF00000)  /* eMMC Host Controller Base Address */

/*
 * Size of malloc() pool
 */
#define CFG_ENV_SIZE_MAX        (0x40000) /* Maximum environment size  - 256KB */
#define CFG_MALLOC_LEN          (CFG_ENV_SIZE_MAX + 128*1024)
#define CFG_GBL_DATA_SIZE       (128)     /* size in bytes reserved for initial data */

/* Default TFTP Load address offset from SDRAM base*/
#define CFG_TFTP_LOAD_ADDR_OFFSET (100)

/* Default eMMC Load address offset from SDRAM base*/
#define CFG_MMC_LOAD_ADDR_OFFSET (0x2000000)     /* offset 32MB in DDR*/


#define PHYS_KERNEL_PARAMS_ADDRESS  CONFIG_ARM_AVALANCHE_KERNEL_PARAMS_ADDRESS  /* SDRAM Bank #1 */


/***************************************************************************************************/
/***    Shell Commands definitions                                                                 */
/***************************************************************************************************/


#if defined CONFIG_NOETH && defined CFG_NO_FLASH
#define CONFIG_COMMANDS         (((CONFIG_CMD_DFL) & ~(CFG_CMD_NFS) & ~(CFG_CMD_NET) & ~(CFG_CMD_FLASH) & ~(CFG_CMD_IMLS)) | CFG_CMD_SPI | CFG_CMD_CACHE/*FPGA TEST*/ | CFG_CMD_MMC)
#elif defined CONFIG_NOETH
#define CONFIG_COMMANDS         (((CONFIG_CMD_DFL) & ~(CFG_CMD_NFS) & ~(CFG_CMD_NET)) | CFG_CMD_SPI | CFG_CMD_CACHE/*FPGA TEST*/| CFG_CMD_MMC )
#elif defined CFG_NO_FLASH
#define CONFIG_COMMANDS         (((CONFIG_CMD_DFL) & ~(CFG_CMD_NFS) & ~(CFG_CMD_FLASH) & ~(CFG_CMD_IMLS)) | CFG_CMD_SPI | CFG_CMD_CACHE/*FPGA TEST*/ | CFG_CMD_MMC )
#else
#define CONFIG_COMMANDS         (((CONFIG_CMD_DFL) & ~(CFG_CMD_NFS)) | CFG_CMD_SPI | CFG_CMD_CACHE/*FPGA TEST*/ | CFG_CMD_MMC )
#endif



#define CONFIG_CMDLINE_TAG         (1)         /* enable passing of ATAGs  */
#define CONFIG_SETUP_MEMORY_TAGS   (1)
#define CONFIG_REVISION_TAG        (1)


/* This must be included AFTER the definition of CONFIG_COMMANDS (if any) */
/* These are u-boot generic parameters */
#include <cmd_confdefs.h>

#define CONFIG_ENV_OVERWRITE
#define CONFIG_BOOTDELAY         2                       /* set -1 for no autoboot */
#define CONFIG_VERSION_VARIABLE

/* Only interrupt boot if space is pressed */
/* If no serial cable is connected, garbage will be read */
#define CONFIG_AUTOBOOT_KEYED 1
#define CONFIG_AUTOBOOT_PROMPT "Press SPACE to abort autoboot in %d second(s)\n"
#define CONFIG_AUTOBOOT_STOP_STR " "



#undef CONFIG_BOOTARGS


#if !defined CFG_NO_FLASH && !defined CONFIG_SPI_FLASH
#define CFG_FLASH_CFI_DRIVER
#endif

#ifdef CFG_FLASH_CFI_DRIVER
#define CFG_FLASH_CFI
#define CFG_FLASH_USE_BUFFER_WRITE  1    /* Use buffered writes (~10x faster) */
#define CFG_FLASH_PROTECTION        1    /* Use hardware sector protection */
#define CFG_FLASH_CFI_WIDTH         FLASH_CFI_32BIT    /* Flash bit width */
#endif /* CFG_FLASH_CFI_DRIVER */


/*
 * Hardware drivers
 */
#define CFG_FLASH_ERASE_TOUT    500000  /* Flash Erase Timeout (in us)  */
#define CFG_FLASH_WRITE_TOUT    500000  /* Flash Write Timeout (in us)  */
#define CFG_FLASH_LOCK_TOUT     500 /* Timeout for Flash Set Lock Bit (in ms) */
#define CFG_FLASH_UNLOCK_TOUT   10000   /* Timeout for Flash Clear Lock Bits (in ms) */
/*#define CFG_FLASH_PROTECTION*/  /* "Real" (hardware) sectors protection */


/*
 * Platform/Board specific defs
 */
#ifdef CFG_EXTERNAL_CLK
#define ARM_CLKC_FREQ
#define SYS_CLKC_FREQ
#define VBUS_CLKC_FREQ
#error "CFG_EXTERNAL_CLK is not supported in puma6 now"
#else /* !CFG_EXTERNAL_CLK */

#ifdef USE_FPGA_CLK
#define FPLL_4xCLK_FREQ         (40000000)   /* [Hz] ARM, C55, P24, PP, etc.. */
#define FPLL_2xCLK_FREQ         (20000000)
#define FPLL_1xCLK_FREQ         (10000000)   /* [Hz] Timer0, UART0, WatchDog, etc..*/
#else
#define FPLL_4xCLK_FREQ         (450000000)   /* [Hz] ARM, C55, P24, PP, etc.. */
#define FPLL_2xCLK_FREQ         (225000000)
#define FPLL_1xCLK_FREQ         (112500000)   /* [Hz] Timer0, UART0, WatchDog, etc..*/
#endif /* USE_FPGA_CLK */

#endif /* !CFG_EXTERNAL_CLK */


/* UART Base address */
#define UART0_REGS_BASE         (0x00050000)
#define UART1_REGS_BASE         (0x00060000)
#define UART2_REGS_BASE         (0x00070000)

/*
 * NS16550 Configuration
 */
/* Set to use NS16550 */
#define CFG_NS16550
#define CFG_NS16550_SERIAL
#define CONFIG_CONS_INDEX       1
#define CONFIG_BAUDRATE         115200
#define CFG_BAUDRATE_TABLE      { 110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 56000, 57600, 115200 }
#define CFG_NS16550_REG_SIZE    4
#define CFG_NS16550_CLK         FPLL_1xCLK_FREQ
#define CFG_NS16550_COM1        UART2_REGS_BASE


#if defined CFG_NO_FLASH
#define CFG_ENV_IS_NOWHERE
#define CFG_ENV_SIZE            (4*1024)
#else
#define CFG_ENV_IS_IN_FLASH     1    /* Support Enviroment Variables in Flash */
#define CFG_ENV_ADDR            (CFG_FLASH_BASE + g_boot_param_env1_base_offset)
#define CFG_ENV_SIZE            (g_boot_param_env_size)   /* ENV Size is 1 Sector */
#define CFG_ENV_SECT_SIZE       (g_boot_param_env_size)   /* ENV SECT Size is 1 Sector */
#define CFG_ENV_ADDR_REDUND     (CFG_FLASH_BASE + g_boot_param_env2_base_offset)
#define CFG_ENV_SIZE_REDUND     (g_boot_param_env_size)   /* ENV Redund Size is 1 Sector */
#endif /* !CFG_NO_FLASH */

/* eMMC ENV configuration */
#define CONFIG_ENV_IS_IN_MMC            (1)  /* Support Enviroment Variables in MMC */
#define CONFIG_SYS_MMC_ENV_DEV           0   /* MMC device number for environment variables */
#define CONFIG_SYS_MMC_IMG_DEV           0   /* MMC device number for UBFI images */
#define CONFIG_MMC_BLOCK_SIZE           (512) /* MMC Block Size in bytes  -In case CONFIG_MMC_SKIP_BOOT is enabled,
                                                 then, we must specify the block size */
#define CONFIG_MMC_SDHCI_IO_ACCESSORS   (1)  /* Enable Host Controller to use private accessors functions */
#define CONFIG_MMC_SKIP_INIT            (1)  /* Set MMC Host controller to skip any registers access during boot */
                                             /* In Puma6 all Host Controller initialization is done by ATOM */
#define CONFIG_MMC_SDMA                 (1)  /* Use DMA transfer in eMMC operations */
#define CONFIG_PUMA6_ADDRFIX            (1)  /* Enable DMA address fix for Puma6. */
                                             /* In Puma6 DMA Address is written by ARM and Read by ATOM, so we need to fix the DMA Address */

#define CONFIG_PARTITIONS               (1)  /* Enable Partitions on MMC Card */
#define CONFIG_DOS_PARTITION            (1)  /* Set Partition type */
#define CONFIG_MMC
//#define CONFIG_MMC_TRACE                (1)
//#define CONFIG_MMC_DEBUG                (1)
#define CFG_ENV_OFFSET          (g_boot_param_env1_base_offset) /* must be block aligned, 512 bytes aligned */


/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP                            /* undef to save memory         */
#define CFG_PROMPT              "=> "           /* Monitor Command Prompt       */
#define CFG_CBSIZE              (256)             /* Console I/O Buffer Size      */
#define CFG_PBSIZE              (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS             (16)              /* max number of command args   */
#define CFG_BARGSIZE            (CFG_CBSIZE)      /* Boot Argument Buffer Size    */

#define CFG_HUSH_PARSER
#define CFG_PROMPT_HUSH_PS2     ">"
#define CFG_HUSH_STATIC_MAP     /* Use static map and avoid slow getenv calls per each command */

#define CFG_MEMTEST_START       (PHYS_SDRAM_1 + 0x00100000)  /*  memtest works on     */
#define CFG_MEMTEST_END         (PHYS_SDRAM_1 + 0x00800000)  /*  1 ... 8 MB in DRAM   */

#undef  CFG_CLKS_IN_HZ          /* everything, incl board info, in Hz */

#define CFG_LOAD_ADDR           (PHYS_SDRAM_1 + CFG_TFTP_LOAD_ADDR_OFFSET) /* default load address */
#define CFG_EMMC_LOAD_ADDR      (PHYS_SDRAM_1 + CFG_MMC_LOAD_ADDR_OFFSET)  /* default emmc load address */

#define CFG_HZ                  (1000)        /* 1ms clock */
#define CFG_ETH_TX_DELAY        (1000)        /* usecs to wait after tx */

/* define memory region for UBFI image on RAM (UBFI3) */
#define CFG_RAM_IMAGE_OFFSET    0x03C00000
#define CFG_RAM_IMAGE_SIZE      0x00400000




#define CONFIG_CMDLINE_EDITING  1   /* add command line history */



/* Implement late init to set UBFI partitions */
#define BOARD_LATE_INIT
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/***      Default Environment Variable settings,                              ***/
/***      For first run only, or Flash failure                                ***/
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

/*#define CONFIG_ETHADDR        08:00:3e:26:0a:5b*/

#define CONFIG_NETMASK          255.255.255.0

#define CONFIG_IPADDR           192.168.100.1

#define CONFIG_SERVERIP         192.168.100.2

#define CONFIG_GATEWAYIP        192.168.100.2

#define CONFIG_BOOTCOMMAND                                                  \
    "while itest.b 1 == 1;"  \
    "do;"   \
        "if itest.b ${ACTIMAGE} == 1 || itest.b ${ACTIMAGE} == 3;"  \
        "then "  \
            "aimgname=UBFI1; aubfiaddr=${UBFIADDR1};"   \
            "bimgname=UBFI2; bubfiaddr=${UBFIADDR2}; bimgnum=2;"    \
        "else "  \
            "if itest.b ${ACTIMAGE} == 2;"  \
            "then "  \
                "aimgname=UBFI2; aubfiaddr=${UBFIADDR2};"   \
                "bimgname=UBFI1; bubfiaddr=${UBFIADDR1}; bimgnum=1;"    \
            "else "  \
                "echo *** ACTIMAGE invalid; exit;"  \
            "fi;"   \
        "fi;"   \
        "if itest.b ${ACTIMAGE} == 3;"  \
        "then "  \
            "eval ${rambase} + ${ramoffset};" \
            "eval ${RAM_IMAGE_OFFSET} + ${evalval};" \
            "set UBFIADDR3 ${evalval};" \
            "if autoscr ${evalval};"    \
            "then "  \
                "bootm ${LOADADDR};"   \
            "else " \
                "echo Reloading RAM image;" \
                "tftpboot ${ramimgaddr} ${UBFINAME3};" \
                "if autoscr ${ramimgaddr};"    \
                "then "  \
                    "bootm ${LOADADDR};"   \
                "else " \
                    "setenv ACTIMAGE 1;" \
                "fi;"   \
            "fi;"   \
        "fi; "  \
        "echo *** ACTIMAGE = ${ACTIMAGE}, will try to boot $aimgname stored @${aubfiaddr};"   \
        "if autoscr $aubfiaddr;"    \
           "then "  \
            "echo *** $aimgname bootscript executed successfully.;"  \
            "echo Start booting...;"  \
            "bootm ${LOADADDR};"   \
        "fi;"   \
        "echo *** $aimgname is corrupted, try $bimgname...;"    \
        "setenv ACTIMAGE $bimgnum;" \
        "if autoscr $bubfiaddr;"    \
            "then "  \
            "echo *** $bimgname bootscript executed successfully.;" \
            "echo Check image...;"  \
            "if imi ${LOADADDR};"  \
                "then " \
                "echo Save updated ACTIMAGE...;"  \
                "saveenv;"  \
                "echo Image OK, start booting...;"  \
                "bootm ${LOADADDR};"   \
            "fi;"   \
        "fi;"   \
        "echo Backup image also corrupted...exit.;" \
    "exit;" \
    "done;"




/********* ENV for Image updates and dual image supprt **************/

/* UBFI Partitions:
 *  Set these macros to absolute base address values if you don't want U-Boot to
 *  automatically determine (on first run only) the UBFI partition base addresses
 */
#define UBFI1_SECT_BASE         0
#define UBFI2_SECT_BASE         0

#define UBFIADDR1               UBFI1_SECT_BASE
#define UBFIADDR2               UBFI2_SECT_BASE

#define ENV_UBFIADDR1             "UBFIADDR1=" MK_STR(UBFIADDR1) "\0"
#define ENV_LOADADDR              "LOADADDR=0\0"
#define ENV_UBFIADDR2             "UBFIADDR2=" MK_STR(UBFIADDR2) "\0"
#define ENV_RAM_IMAGE_OFFSET      "RAM_IMAGE_OFFSET=" MK_STR(CFG_RAM_IMAGE_OFFSET) "\0"
#define ENV_RAM_IMAGE_SIZE        "RAM_IMAGE_SIZE=" MK_STR(CFG_RAM_IMAGE_SIZE) "\0"
#define ENV_BOOTPARAMS_AUTOUPDATE "BOOTPARAMS_AUTOUPDATE=on\0"
#define ENV_BOOTPARAMS_AUTOPRINT  "BOOTPARAMS_AUTOPRINT=off\0"
#ifndef CFG_NO_FLASH
#define ENV_ACTIMAGE              "ACTIMAGE=1\0"
#else
#define ENV_ACTIMAGE              "ACTIMAGE=3\0"
#endif

#define ENV_ERASE \
    "erase_env="\
        "if itest.s ${bootdevice} == mmc; "           \
        "then run erase_mmc_env;"                     \
        "else run erase_spi_env;"                     \
        "fi;"                                         \
        "echo Please reset the board to get default env.\0"
    
#define ENV_SPI_ERASE \
    "erase_spi_env="\
        "eval ${flashbase} + ${envoffset1} && "      \
        "protect off ${evalval} +${envsize} && "     \
        "erase ${evalval} +${envsize} && "           \
        "protect on ${evalval} +${envsize} && "      \
        "eval ${flashbase} + ${envoffset2} && "      \
        "protect off ${evalval} +${envsize} && "     \
        "erase ${evalval} +${envsize} && "           \
        "protect on ${evalval} +${envsize}\0"

#define ENV_MMC_ERASE \
    "erase_mmc_env="\
        "eval ${rambase} + ${ramoffset} && "        \
        "bufferbase=${evalval} &&"                  \
        "mmcaddr2blk $envoffset1 && "               \
        "envblkaddr=$blocksize && "                 \
        "mmcaddr2blk $envsize && "                  \
        "envblksize=$blocksize && "                 \
        "mw ${bufferbase} 0xFF $envsize &&"            \
        "mmc write ${bufferbase} $envblkaddr $envblksize\0"


#define CONFIG_EXTRA_ENV_SETTINGS                                   \
    ENV_UBFIADDR1                                                   \
    ENV_LOADADDR                                                    \
    ENV_UBFIADDR2                                                   \
    ENV_ACTIMAGE                                                    \
    ENV_RAM_IMAGE_OFFSET                                            \
    ENV_RAM_IMAGE_SIZE                                              \
    ENV_BOOTPARAMS_AUTOUPDATE                                       \
    ENV_BOOTPARAMS_AUTOPRINT                                        \
    ENV_SPI_ERASE                                                   \
    ENV_MMC_ERASE                                                   \
    ENV_ERASE                                                       \
    ""

/********************************************************************/

#ifndef __ASSEMBLY__
/* 8 byte read buffer - Optimized for SFI burst reads.
   The ARM11 in Puma6, can worked with maximum chunks of 8 bytes.
   Any chunks bugger than 8 might cause to the Puma6 buses to hung.
   Especially on flash reading. */

typedef struct {
  unsigned char buffer[8];

} sfi_read_buf_t;

#define SFI_BUF_SIZE    ( sizeof( sfi_read_buf_t ) )
#endif

#endif    /* ! __HARBORPARK_H */
