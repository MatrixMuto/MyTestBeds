#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct buf_s {
    uint8_t *start;
    uint8_t *pos;
    uint8_t *end;
} buf_t;

typedef struct context_s {
    buf_t buf;
};

typedef struct context_s context_t;

/* bits may point to end of buffer plus 1 */
uint8_t *detect_start_code_prefix(context_t *ctx, const uint8_t *buf, uint32_t *offset, uint32_t size) {
    uint8_t *bits = (uint8_t *) buf;
    for (;;) {
        int32_t idx = 0;
        while (idx < size && !(*bits)) {
            ++idx;
            ++bits;
        }

        if (idx >= size)
            break;

        ++idx;
        ++bits;

        if (idx >= 3 && (*(bits - 1) == 0x1)) {
            *offset = (int32_t) (((uintptr_t) bits) - ((uintptr_t) buf));
            return bits;
        }
        size -= idx;
    }

    return NULL;
}

char *nal_unit_name_of(uint8_t type) {
    static char *name[] = {
            "Unspecified",
            "Coded slice of a non-IDR picture",
            "Coded slice data partition A",
            "Coded slice data partition B",
            "Coded slice data partition C",
            "Coded slice of an IDR picture",
            "Supplemental enhancement information",
            "Sequence parameter set",
            "Picture parameter set",
            "Access unit delimiter",
            "End of sequence",
            "End of stream",
            "Filler data",
            "Sequence parameter set extension",
            "Prefix NAL unit",
            "Subset sequence parameter set",
            "Reserved",
            "Reserved",
            "Reserved",
            "Coded slice of an auxiliary coded picture without partitioning",
            "Coded slice extension",
            "Coded slice extension for depth view components",
            "Reserved",
            "Reserved",
            "Unspecified",
            "Unspecified",
            "Unspecified",
    };

    return name[type];
}

void feed(context_t *ctx, uint8_t *buf_pos, size_t size) {
    buf_t *buf = &ctx->buf;
    if (buf->end - buf->pos < size) {
        size_t old_size = buf->end - buf->start;
        size_t len = buf->pos - buf->start;
        buf->start = (uint8_t *) realloc(buf->start, old_size << 1);
        buf->pos = buf->start + len;
        buf->end = buf->start + (old_size << 1);
    }
    memcpy(buf->pos, buf_pos, size);
    buf->pos += size;
}

#define BUFSIZE 4096

int main(int argc, char *argv[]) {
    const char *filename;
    FILE *file;
    uint8_t buffer[BUFSIZE];
    size_t size;
    uint32_t offset;
    uint8_t *start;
    uint8_t *prev_end, *end, *buf_pos;
    context_t ctx;

    ctx.buf.start = (uint8_t *) calloc(1, 4096);
    ctx.buf.pos = ctx.buf.start;
    ctx.buf.end = ctx.buf.start + 4096;

    filename = "E:\\MatrixMuto\\xxx.h264";
    file = fopen(filename, "rb");

    //size = fread_s(buffer, BUFSIZE, BUFSIZE, 1, file);;
    //start =  detect_start_code_prefix(NULL, buffer, &offset, size);
    size = 0;
    prev_end = NULL;
    buf_pos = buffer;
    int64_t file_pos = 0;
    for (;;) {

        if (size <= 0) {
            if (!(size = fread(buffer, 1, BUFSIZE, file))) {
                fprintf(stdout, "end of file\n");
                break;
            }
            buf_pos = buffer;
            file_pos += size;
            //printf("file pos %lld\n", file_pos);
            if (file_pos > 100000) break;
        }

        start = detect_start_code_prefix(NULL, buf_pos, &offset, size);
        if (!start) {
            // TODO(mut0): construct a complete nal unit
            feed(&ctx, buf_pos, size);
            size = 0;
            prev_end = NULL;
            continue;
        }

        // TODO(mut0): start may be out of buffer
        printf("%02x%02x", *start, *(start + 1));

        uint8_t forbidden_zero_bit = (*start) & 0x80;
        if (forbidden_zero_bit) {
            printf(" invalid");
        }
        uint8_t nal_ref_idc = ((*start) >> 5) & 0x03;
        uint8_t nal_unit_type = (*start) & 0x1f;

        printf(" (%d)%50s", nal_unit_type, nal_unit_name_of(nal_unit_type));
        if (!prev_end) {
            /* first nal unit start */
            if (ctx.buf.pos != ctx.buf.start) {
                feed(&ctx, buf_pos, offset);
            }
        } else {
            /* found a nal unit, from prev_end -> start - 3 or 4*/
            feed(&ctx, buf_pos, offset);
        }
        /* consume this nal unit */
        size_t nal_unit_size = ctx.buf.pos - ctx.buf.start;
        printf("%10u", nal_unit_size);


        /* clear frame buffer */
        ctx.buf.pos = ctx.buf.start;

        printf("\n");
        size -= offset;
        buf_pos += offset;
        prev_end = start;
    }

    fclose(file);
    system("pause");
    return 0;
}

