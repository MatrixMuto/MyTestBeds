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
	int res, i, err;

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

	res = avformat_open_input(&ic, test_urls[2], NULL, NULL);
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

	int flag = 0;
	for (i=0; i<100; )
	{
		err = av_read_frame(ic, pkt);
		if (err < 0) {
			printf("av_read_frame err:%x\n",err);
			continue;
		}

		if (ic->nb_streams > 1 && !flag) {
			printf("programs %u, stream %u\n",ic->nb_programs,ic->nb_streams);
			unsigned char *dummy=NULL;
			int dummy_len;
			AVCodecContext *pCodecCtx = ic->streams[1]->codec;

//			AVBitStreamFilterContext *bsfc = av_bitstream_filter_init("h264");
//			printf("crash? %d\n",__LINE__);
//			av_bitstream_filter_filter(bsfc, pCodecCtx, NULL, &dummy, &dummy_len, NULL, 0, 0);
//			printf("crash? %d\n",__LINE__);
		//	fwrite(pCodecCtx->extradata, pCodecCtx->extradata_size,1,fp);
			if (ic->streams[1]->codec->extradata_size > 0) {
				hexDump("extra", ic->streams[1]->codecpar->extradata, ic->streams[1]->codecpar->extradata_size);
				printf("crash? %d\n",__LINE__);
				char buf[4] = {0};
				buf[3] =1;
				dump("/tmp/arb.264",buf,4);
				dump("/tmp/arb.264",
				ic->streams[1]->codec->extradata+8,
				ic->streams[1]->codec->extradata_size-8
				);

				//av_bitstream_filter_close(bsfc);
				//free(dummy);
				flag = 1;
			}
		}
		if (flag) {
			if (pkt->stream_index == 1) {
				dump_avpacket(i, pkt);
				uint8_t* p = pkt->data;
				p[0] = 0; p[1] = 0; p[2] = 0;
				p[3] = 1;
				dump("/tmp/arb.264", pkt->data, pkt->size);
				i++;
			}
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
