#ifndef RRTMP_BYTESTREAM_H_
#define RRTMP_BYTESTREAM_H_
struct ByteStream
{
    ByteStream(uint8_t* buf, size_t size) {
        begin = buf;
        end   = begin + size;
        cur   = begin;
    }

    uint32_t get_be32() {
		uint32_t val = *cur++ << 24 | *cur++ << 16 | *cur++ << 8 | *cur++;
		return val;
	}
	uint32_t get_be24() {
		return *cur++ << 16 | *cur++ << 8 | *cur++;
	}
	uint8_t get_byte() {
		return *cur++;
	}
    void put_be32(uint32_t val) {
       *cur++ = val >> 24;
       *cur++ = val >> 16;
       *cur++ = val >> 8;
       *cur++ = val;
    }
    void put_le32(uint32_t val) {
        *cur++ = val;
        *cur++ = val >> 8;
        *cur++ = val >> 16;
        *cur++ = val >> 24;
    }
    void put_be24(uint32_t val) {
        *cur++ = val >> 16;
        *cur++ = val >> 8;
        *cur++ = val;
    }

    void put_be16(uint16_t val) {
        *cur++ = val >> 8;
        *cur++ = val;
    }

    void put_byte(uint8_t val) {
        *cur++ = val;
    }
    
    void put_buffer(uint8_t* val, size_t size)
    {
        memcpy(cur, val, size);
        cur += size;
    }

    inline size_t size() { 
        return cur - begin;
    };

    inline uint8_t* buf() {
        return begin;
    }
private:
    uint8_t* begin;
    uint8_t* end;
    uint8_t* cur;
};

#endif
