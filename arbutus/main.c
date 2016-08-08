#include <stdio.h>

#include "libavformat/avformat.h"

const char* test_urls[] = {
		"https://devimages.apple.com.edgekey.net/streaming/examples/bipbop_16x9/bipbop_16x9_variant.m3u8",
		"https://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/bipbop_4x3_variant.m3u8",
		"rtmp://localhost:1935/live/test",
		"rtmp://localhost:1935/vod/test-1467270658.flv",
		"file://home/wq1950/Videos/test.flv",
		"rtmp://live.hkstv.hk.lxdns.com/live/hks",
};

void hexDump (char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}

static inline void dump(char* path, char* data, int size)
{
	if (path == NULL) {
		return;
	}

	FILE *file  = fopen(path, "a+");
	if (file == NULL) {
		return;
	}

	fwrite(data, size, 1, file);

	fclose(file);
}

static inline void dump_avpacket(int i, AVPacket* pkt)
{
	printf("[%04d] pts %6ld, dts %6ld, size %6d, si %4d, flag %2d\n",
				i, pkt->pts, pkt->dts, pkt->size, pkt->stream_index, pkt->flags);
}

void test_read_frame()
{
	AVFormatContext *ic;
	AVPacket pkt1,*pkt = &pkt1;
	int res, i, err, vi, ai;

	printf("%s\n",avformat_configuration());

	avformat_network_init();

//	AVInputFormat* iformat = av_find_input_format(url3);
//	if (iformat != NULL)
//		printf("%s\n",iformat->name);
//	else
//		printf("iforamt==NULL\n");


	ic = avformat_alloc_context();
	ic->format_probesize = 1 << 17;

	av_register_all();

	res = avformat_open_input(&ic, test_urls[5], NULL, NULL);
	if (ic == NULL)
	{
		printf("failed open input, res %x\n",res);
		goto fail;
	}
	printf("avformat_find_stream_info\n", err);

	err = avformat_find_stream_info(ic, 0);
	if (err < 0) {
		printf("avformat_find_stream_info err %d\n", err);
	}

	printf("programs %u, stream %u\n",ic->nb_programs,ic->nb_streams);

	for (i=0; i < ic->nb_streams; ++i) {
		AVStream* st = ic->streams[i];
		if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			vi = st->index;
		}
		else if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
			ai = st->index;
		}
	}

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;

	for (i=0; i<100; )
	{
		err = av_read_frame(ic, pkt);
		if (err < 0) {
			printf("av_read_frame err:%x\n",err);
			continue;
		}
		if (pkt->stream_index == vi) {
			dump_avpacket(i, pkt);
			if (i==0) hexDump("pkt data",pkt->data,pkt->size);
			for (uint8_t *p = pkt->data; p < pkt->data+pkt->size;) {
				int len = (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3];
				printf("len = %d\n",len);
				p[0] = p[1] = p[2] = 0; p[3] = 1;
				dump("/tmp/arb.264",p, len + 4);
				p += len + 4;
			}
			i++;
		}
	}

fail:
	avformat_close_input(&ic);

	avformat_free_context(ic);

	avformat_network_deinit();
}

void test_rtmp_publish()
{
	AVIOContext *io;
	const char* publish_url = "rtmp://localhost:1935/";
	const char* protocol = avio_find_protocol_name(publish_url);

	printf("%s\n", protocol);

	avio_open2(&io, publish_url, AVIO_FLAG_WRITE, NULL, NULL);

	//avio_write();

	avio_close(&io);
}

int main(int argc, char *argv[])
{
	test_read_frame();
//	int target = 4;
//	printf("%d %s %s\n",argc,argv[0],argv[1]);
//	if (argc == 2) {
//		char t = argv[1][0];
//		printf("%c\n",t);
//		int index = t -'0';
//		if (index >=1 && index <= sizeof(test_urls))
//			target = index;
//	}
	return 0;
}
