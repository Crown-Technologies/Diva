#include <driver/fs/mbr.h>
#include <ntypes.h>

u64 fat_get_partition(u8* mbr);
void fat_listdirectory(void);
// retruns 0 if FAT16 and 1 if FAT32
#define fat_get_type(mbr) (mbr_get_bpb(mbr)->spf16 > 0 ? 1 : 0)
