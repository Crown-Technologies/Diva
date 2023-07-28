typedef struct FramebufferInfo {
    unsigned int width, height, depth;
} FBInfo;

void fb_init(FBInfo framebuffer_info);
void fb_print(int x, int y, char *s);
void fb_proprint(int x, int y, char *s);