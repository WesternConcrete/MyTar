#include "conformer.h"

/* For interoperability with GNU tar. GNU seems to
 * set the high–order bit of the first byte, then
 * treat the rest of the field as a binary integer
 * in network byte order.
 * I don’t know for sure if it’s a 32 or 64–bit int, but for
 * this version, we’ll only support 32. (well, 31)
 * returns the integer on success, –1 on failure.
 * In spite of the name of htonl(), it converts int32 t
 */
uint32_t extra_special_int(char *where, int len) {
    int32_t val = -1;
    if (((size_t) len >= sizeof(val)) && (where[0] & 0x80)) {
        /* the top bit is set and we have space
         * extract the last four bytes */
        val = *(int32_t *)(where + len - sizeof(val));
        val = ntohl(val); /* covert to host byte order */
    }

    return val;
}

/* For interoperability with GNU tar. GNU seems to
 * set the high–order bit of the first byte, then
 * treat the rest of the field as a binary integer
 * in network byte order.
 * Insert the given integer into the given field
 * using this technique. Returns 0 on success, nonzero
 * otherwise
 */
int insert_special_character(char *where, size_t size, int32_t val) {
    int err = 0;

    if (val < 0 || (size < sizeof(val))) {
        /* if it's negative, bit 31 is set and we can't use the flag
         * if len is too smal, we can't write it. Either way, we're
         * done
         */
        err++;
    } else {
        memset(where, 0, size); /* clear out the buffer */

        /* places the int */ 
        *(int32_t *)(where + size - sizeof(val)) = htonl(val);

        *where |= 0x80; /*set that high-order bit */
    }

    return err;
}
