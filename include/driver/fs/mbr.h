#ifndef DRI_MBR_H
#define DRI_MBR_H


// the BIOS Parameter Block (in Volume Boot Record)
typedef struct bpb {
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


// Returns the BIOS Parameter Block
bpb_t *mbr_get_bpb(unsigned char* dst);
// Returns MBR disk idendifier
unsigned long long mbr_get_id(void* mbr);

#endif
