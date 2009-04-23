/*
    Copyright (C) 2005-2008  Ricky Zheng <ricky_gz_zheng@yahoo.co.nz>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/
/** 
 * \file uffs_blockInfo.h
 * \brief data structure for operating block information
 * \author Ricky Zheng
 */

#ifndef _UFFS_BLOCKINFO_H_
#define _UFFS_BLOCKINFO_H_

#include "uffs/uffs_types.h"
#include "uffs/uffs_public.h"
#include "uffs/uffs_core.h"

#ifdef __cplusplus
extern "C"{
#endif
/** 
 * \struct uffs_pageSpareSt
 * \brief this structure is for storing uffs tag and more. 
 */
struct uffs_pageSpareSt {
	u8 expired:1;			//!< 0 not expired. 1 expired. 
	u8 checkOk:1;           //!< 0 check sum is OK. 1 check sum is error. 
	u8 blockStatus:1;       //!< for block status, 1 bad, 0 valid 
	uffs_Tags tag;			//!< page tag 
};

/** 
 * \struct uffs_blockInfoSt
 * \brief block information data. Block info is frequently accessed,
          UFFS use a cache system to speed up block info access.
 */
struct uffs_blockInfoSt {
	struct uffs_blockInfoSt *next;
	struct uffs_blockInfoSt *prev;
	u16 blockNum;					//!< block number
	struct uffs_pageSpareSt *spares;//!< page spare info array
	int expiredCount;		//!< how many pages expired in this block ? 
	int refCount;			//!< reference counter, it's safe to reuse this block memory when the counter is 0.
};


URET uffs_InitBlockInfoCache(uffs_Device *dev, int maxCachedBlocks);
URET uffs_ReleaseBlockInfoCache(uffs_Device *dev);
void uffs_CheckPageSpare(uffs_Device *dev, uffs_pageSpare *spare);
URET uffs_LoadBlockInfo(uffs_Device *dev, uffs_blockInfo *work, int page);
uffs_blockInfo * uffs_FindBlockInfoInCache(uffs_Device *dev, int block);
uffs_blockInfo * uffs_GetBlockInfo(uffs_Device *dev, int block);
void uffs_PutBlockInfo(uffs_Device *dev, uffs_blockInfo *p);
void uffs_ExpireBlockInfo(uffs_Device *dev, uffs_blockInfo *p, int page);
//uffs_blockInfo * uffs_CloneBlockInfo(uffs_Device *dev, uffs_blockInfo *p);
//void uffs_ReleaseCloneBlockInfo(uffs_blockInfo *p);
UBOOL uffs_IsAllBlockInfoFree(uffs_Device *dev);
void uffs_ExpireAllBlockInfo(uffs_Device *dev);

#ifdef __cplusplus
}
#endif


#endif
