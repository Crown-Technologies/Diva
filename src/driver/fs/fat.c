#include <driver/uart.h>
#include <driver/blk.h>

#include "../log.h"

extern volatile unsigned char _end;

static unsigned int partitionlba = 0;

// the BIOS Parameter Block (in Volume Boot Record)
typedef struct {
    char            jmp[3];
    char            oem[8];
    unsigned char   bps0;
    unsigned char   bps1;
    unsigned char   spc;
    unsigned short  rsc;
    unsigned char   nf;
    unsigned char   nr0;
    unsigned char   nr1;
    unsigned short  ts16;
    unsigned char   media;
    unsigned short  spf16;
    unsigned short  spt;
    unsigned short  nh;
    unsigned int    hs;
    unsigned int    ts32;
    unsigned int    spf32;
    unsigned int    flg;
    unsigned int    rc;
    char            vol[6];
    char            fst[8];
    char            dmy[20];
    char            fst2[8];
} __attribute__((packed)) bpb_t;

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
int fat_getpartition(void)
{
    unsigned char *mbr=&_end;
    bpb_t *bpb=(bpb_t*)&_end;
    // read the partitioning table
    if(dev_readblk(0,&_end,1)) {
        // check magic
        if(mbr[510]!=0x55 || mbr[511]!=0xAA) {
            blklog("ERROR: Bad magic in MBR\n");
            return 0;
        }
        // check partition type
        if(mbr[0x1C2]!=0xE/*FAT16 LBA*/ && mbr[0x1C2]!=0xC/*FAT32 LBA*/) {
            blklog("ERROR: Wrong partition type\n");
            return 0;
        }
        blklog("MBR disk identifier: ");
        blklogh(*((unsigned int*)((unsigned long)&_end+0x1B8)));
        blklog("\nFAT partition starts at: ");
        // should be this, but compiler generates bad code...
        //partitionlba=*((unsigned int*)((unsigned long)&_end+0x1C6));
        partitionlba=mbr[0x1C6] + (mbr[0x1C7]<<8) + (mbr[0x1C8]<<16) + (mbr[0x1C9]<<24);
        blklogh(partitionlba);
        blklog("\n");
        // read the boot record
        if(!dev_readblk(partitionlba,&_end,1)) {
            blklog("ERROR: Unable to read boot record\n");
            return 0;
        }
        // check file system type. We don't use cluster numbers for that, but magic bytes
        if( !(bpb->fst[0]=='F' && bpb->fst[1]=='A' && bpb->fst[2]=='T') &&
            !(bpb->fst2[0]=='F' && bpb->fst2[1]=='A' && bpb->fst2[2]=='T')) {
            blklog("ERROR: Unknown file system type\n");
            return 0;
        }
        blklog("FAT type: ");
        // if 16 bit sector per fat is zero, then it's a FAT32
        blklog(bpb->spf16>0?"FAT16":"FAT32");
        blklog("\n");
        return 1;
    }
    return 0;
}
