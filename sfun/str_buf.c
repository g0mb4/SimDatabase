#include "str_buf.h"

str_buf_t * str_buf_create(void) {
    str_buf_t * buf = (str_buf_t *)malloc(sizeof(str_buf_t));
    if (buf == NULL) {
        fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__,
                __func__);
        return NULL;
    }

    buf->str = (char *)malloc(sizeof(char) * 10);
    if (buf->str == NULL) {
        fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__, __LINE__,
                __func__);
        return NULL;
    }

    memset(buf->str, 0, 10);
    buf->len = 0;
    buf->cap = 10;

    return buf;
}

void str_buf_destroy(str_buf_t * sb) {
    if (sb) {
        if (sb->str) {
            free(sb->str);
        }

        free(sb);
    }
}

int str_buf_add(str_buf_t * sb, const char * s) {
    uint32_t len = strlen(s);

    if (sb->len + len >= sb->cap) {
        sb->str = realloc(sb->str, sizeof(char) * (sb->len + len + 1));

        if (sb->str == NULL) {
            return -1;
        }

        sb->cap = sb->len + len + 1;
    }

    strcpy((sb->str + sb->len), s);
    sb->len += len;

    return 0;
}

int str_buf_addn(str_buf_t * sb, const char * s, uint32_t len) {
    if (sb->len + len >= sb->cap) {
        sb->str = realloc(sb->str, sizeof(char) * (sb->len + len + 1));

        if (sb->str == NULL) {
            return -1;
        }

        sb->cap = sb->len + len + 1;
    }

    strncpy((sb->str + sb->len), s, len);
    sb->len += len;

    return 0;
}

void str_buf_clear(str_buf_t * sb) {
    if (sb) {
        if (sb->str) {
            free(sb->str);
            sb->str = (char *)malloc(sizeof(char) * 10);
            if (sb->str == NULL) {
                fprintf(stderr, "%s:%d:%s() : malloc() failed.\n", __FILE__,
                        __LINE__, __func__);
                return;
            }

            memset(sb->str, 0, 10);
            sb->len = 0;
            sb->cap = 10;
        }
    }
}

int str_buf_copy(str_buf_t * sb_dest, const str_buf_t * sb_src) {
    if (sb_dest && sb_src) {
        if (sb_dest->str) {
            free(sb_dest->str);
        }
        sb_dest->str == (char *)malloc(sb_src->cap);
        if (sb_dest->str) {
            strcpy(sb_dest->str, sb_src->str);
            sb_dest->len = sb_src->len;
            sb_dest->cap = sb_src->cap;
        } else {
            return -2;
        }
    } else {
        return -1;
    }

    return 0;
}
