#include <driver/uart.h>
#include <driver/blk.h>
#include <syslib/kmem.h>
#include <driver/fs/mbr.h>

#include <ntypes.h>
#include <stddef.h>


// directory entry structure
typedef struct {
    char            name[8];
    char            ext[3];
    char            attr[9];
    unsigned short  ch;
    unsigned int    attr2;
    unsigned short  cl;
    unsigned int    size;
} __attribute__((packed)) fatdir_t;

/**
 * Get the starting LBA address of the first partition
 * so that we know where our FAT file system starts, and
 * read that volume's BIOS Parameter Block
 */
u64 fat_get_partition(u8* mbr)
{
    bpb_t *bpb = mbr_get_bpb(mbr);
    u64 lba = 0;
    // read the partitioning table
    if(bpb != NULL) {
        // check partition type
        if(mbr[0x1C2]!=0xE/*FAT16 LBA*/ && mbr[0x1C2]!=0xC/*FAT32 LBA*/)
            return 0;
        lba = mbr[0x1C6] + (mbr[0x1C7]<<8) + (mbr[0x1C8]<<16) + (mbr[0x1C9]<<24);
        // read the boot record
        if(!dev_readblk(lba, mbr, 1))
            return 0;
        // check file system type. We don't use cluster numbers for that, but magic bytes
        if( !(bpb->fst[0]=='F' && bpb->fst[1]=='A' && bpb->fst[2]=='T') &&
            !(bpb->fst2[0]=='F' && bpb->fst2[1]=='A' && bpb->fst2[2]=='T'))
            return 0;
    }
    return lba;
}

