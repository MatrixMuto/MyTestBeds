#include <stdio.h>

#include "libavformat/avformat.h"

int main(int argc, char *argv[])
{
	AVFormatContext *c;
	AVPacket pkt1,*pkt = &pkt1;
	const char* url1 = "rtmp://localhost:1935/live/test";
	const char* url2 = "https://devimages.apple.com.edgekey.net/streaming/examples/bipbop_16x9/bipbop_16x9_variant.m3u8";
	const char* url4 = "https://devimages.apple.com.edgekey.net/streaming/examples/bipbop_4x3/bipbop_4x3_variant.m3u8";
	const char* url3 = "rtmp://localhost:1935/vod/test-1467270658.flv";
	const char* url5 = "file://home/wq1950/Videos/test.flv";
	char* urls[] = {url1, url2, url3, url4, url5};
	int target = 4;
	printf("%d %s %s\n",argc,argv[0],argv[1]);
	if (argc == 2) {
		char t = argv[1][0];
		printf("%c\n",t);
		int index = t -'0';
		if (index >=1 && index <= sizeof(urls))
			target = index;
	}

	avformat_network_init();
#if 0
	printf("%s\n",avformat_configuration());



//	AVInputFormat* iformat = av_find_input_format(url3);
//	if (iformat != NULL)
//		printf("%s\n",iformat->name);
//	else
//		printf("iforamt==NULL\n");

	c = avformat_alloc_context();
	c->format_probesize = 1 << 17;

	av_register_all();

	int res = avformat_open_input(&c, urls[target-1], NULL, NULL);
	if (c == NULL)
	{
		printf("failed open input, res %x\n",res);
		goto fail;
	}

	printf("programs %u\n",c->nb_programs);
	for (int i=0;i<100;)
	{
		memset(pkt, 0, sizeof(pkt));
		av_read_frame(c, pkt);

		if (pkt->stream_index == 1) {
			printf("[%d] pts %ld,dts %ld,size %d, si %d, flag %d\n",
				i, pkt->pts, pkt->dts, pkt->size, pkt->stream_index, pkt->flags);
			i++;
		}
	}

#endif
	AVIOContext *io;
	const char* publish_url = "rtmp://localhost:1935/";
	const char* protocol = avio_find_protocol_name(url3);
	printf("%s\n", protocol);
	avio_open2(&io, publish_url, AVIO_FLAG_WRITE, NULL, NULL);

	avio_write();

	avio_close(&io);
fail:
#if 0
	avformat_close_input(&c);

	avformat_free_context(c);

#endif
	avformat_network_deinit();
	return 0;
}