#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "uffs_config.h"
#include "uffs/uffs_public.h"
#include "uffs/uffs_fs.h"
#include "uffs/uffs_utils.h"
#include "uffs/uffs_core.h"
#include "uffs/uffs_mtb.h"
#include "uffs/uffs_fd.h" // posix interface for uffs
#include "uffs_fileem.h"  // emulator interface

/* default basic parameters of the NAND device */
#define PAGES_PER_BLOCK_DEFAULT         32
#define PAGE_DATA_SIZE_DEFAULT          512
#define PAGE_SPARE_SIZE_DEFAULT         16
#define STATUS_BYTE_OFFSET_DEFAULT      5
#define TOTAL_BLOCKS_DEFAULT            128
#define ECC_OPTION_DEFAULT              UFFS_ECC_SOFT
//#define ECC_OPTION_DEFAULT            UFFS_ECC_HW
//#define ECC_OPTION_DEFAULT            UFFS_ECC_HW_AUTO

#define MAX_MOUNT_TABLES        10
#define MAX_MOUNT_POINT_NAME    32

#ifndef UFFS_FILEEM_DEFAULT_MOUNT
#   define UFFS_FILEEM_DEFAULT_MOUNT "/flash/"
#endif

static int conf_pages_per_block = PAGES_PER_BLOCK_DEFAULT;
static int conf_page_data_size = PAGE_DATA_SIZE_DEFAULT;
static int conf_page_spare_size = PAGE_SPARE_SIZE_DEFAULT;
static int conf_status_byte_offset = STATUS_BYTE_OFFSET_DEFAULT;
static int conf_total_blocks = TOTAL_BLOCKS_DEFAULT;
static int conf_ecc_option = ECC_OPTION_DEFAULT;
static int conf_ecc_size = 0; // 0 - Let UFFS choose the size

static uffs_Device test_device;

static struct uffs_MountTableEntrySt uffs_mount = {
    &test_device,
    0,    /* start from block 0 */
    -1,   /* use whole chip */
    UFFS_FILEEM_DEFAULT_MOUNT,  /* mount point */
    NULL,
    NULL,
};

static void setup_uffs_storage(struct uffs_StorageAttrSt *attr)
{
    attr->total_blocks = conf_total_blocks;             /* total blocks */
    attr->page_data_size = conf_page_data_size;         /* page data size */
    attr->spare_size = conf_page_spare_size;            /* page spare size */
    attr->pages_per_block = conf_pages_per_block;       /* pages per block */

    attr->block_status_offs = conf_status_byte_offset;  /* block status offset is 5th byte in spare */
    attr->ecc_opt = conf_ecc_option;                    /* ECC option */
    attr->ecc_size = conf_ecc_size;                     /* ECC size */
    attr->layout_opt = UFFS_LAYOUT_UFFS;                /* let UFFS handle layout */
}

static void setup_uffs_device(uffs_Device *dev)
{
    // using file emulator device
    dev->Init = femu_InitDevice;
    dev->Release = femu_ReleaseDevice;
    dev->attr = femu_GetStorage();
}

static void release_uffs_fs(void)
{
    uffs_UnMount(UFFS_FILEEM_DEFAULT_MOUNT);
    uffs_ReleaseFileSystemObjects();
}

///////////////////////////////// END UFFS BOILERPLATE

int uffs_emu_Defaults()
{
    struct uffs_MountTableEntrySt *mtbl = &uffs_mount;
    memset(&test_device, 0, sizeof(test_device));

    setup_uffs_storage(femu_GetStorage());
    uffs_MemSetupSystemAllocator(&mtbl->dev->mem);
    setup_uffs_device(mtbl->dev);
    uffs_RegisterMountTable(mtbl);

    /* mount it */
    uffs_Mount(UFFS_FILEEM_DEFAULT_MOUNT);
    /* setup atexit handler */
    atexit(release_uffs_fs);

    return uffs_InitFileSystemObjects() == U_SUCC ? 0 : -1;
}

