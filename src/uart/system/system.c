#include "../mbox.h"


unsigned long get_serial () {
    // get the board's unique serial number with a mailbox call
    mbox[0] = 8*4;                  // length of the message
    mbox[1] = MBOX_REQUEST;         // this is a request message
    
    mbox[2] = MBOX_TAG_GETSERIAL;   // get serial number command
    mbox[3] = 8;                    // buffer size
    mbox[4] = 8;
    mbox[5] = 0;                    // clear output buffer
    mbox[6] = 0;

    mbox[7] = MBOX_TAG_LAST;

    unsigned long serial = 0;

    if (mbox_call(MBOX_CH_PROP))
        serial = (unsigned long) mbox[5] + (((unsigned long) mbox[6]) << 36);

    return serial;
}