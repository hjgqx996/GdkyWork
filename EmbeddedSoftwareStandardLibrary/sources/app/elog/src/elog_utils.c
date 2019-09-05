/*
 * Function: Some utils for this library.
 * Created on: 2015-04-28
 */

#include <elog.h>
#include <string.h>

/**
 * another copy string function
 *
 * @param cur_len current copied log length, max size is ELOG_LINE_BUF_SIZE
 * @param dst destination
 * @param src source
 *
 * @return copied length
 */
size_t elog_strcpy(size_t cur_len, char *dst, const char *src) {
    const char *src_old = src;

    assert(dst);
    assert(src);

    while (*src != 0) {
        /* make sure destination has enough space */
        if (cur_len++ <= ELOG_LINE_BUF_SIZE) {
            *dst++ = *src++;
        } else {
            break;
        }
    }
    return src - src_old;
}

/**
 * Copy line log split by newline sign. It will copy all log when the newline sign isn't find.
 *
 * @param line line log buffer
 * @param log origin log buffer
 * @param len origin log buffer length
 *
 * @return copy size
 */
size_t elog_cpyln(char *line, const char *log, size_t len) {
    size_t newline_len = strlen(ELOG_NEWLINE_SIGN), copy_size = 0;

    assert(line);
    assert(log);

    while (len--) {
        *line++ = *log++;
        copy_size++;
        if (copy_size >= newline_len && !strncmp(log - newline_len, ELOG_NEWLINE_SIGN, newline_len)) {
            break;
        }
    }
    return copy_size;
}

/**
 * This function will copy memory content from source address to destination
 * address.
 *
 * @param dst the address of destination memory
 * @param src  the address of source memory
 * @param count the copied length
 *
 * @return the address of destination memory
 */
void *elog_memcpy(void *dst, const void *src, size_t count) {
    char *tmp = (char *) dst, *s = (char *) src;

    assert(dst);
    assert(src);

    while (count--)
        *tmp++ = *s++;

    return dst;
}
