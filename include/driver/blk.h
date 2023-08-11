#define DEV_OK                0
#define DEV_TIMEOUT          -1
#define DEV_ERROR            -2

int dev_init();
int dev_readblk(unsigned int lba, unsigned char *buffer, unsigned int num);