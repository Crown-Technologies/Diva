#include <stddef.h>
#include <driver/fs/mbr.h>
#include <driver/blk.h>


unsigned long long mbr_get_id(void* mbr) {
    return *((unsigned int*)((unsigned long) mbr + 0x1B8));
}


bpb_t* mbr_get_bpb(unsigned char* dst) {
    if (dev_readblk(0, dst, 1)) {
        // Check magic
        if(dst[510] != 0x55 || dst[511] != 0xAA)
            return NULL;
        return dst;
    }
    else
        return NULL;
} 
