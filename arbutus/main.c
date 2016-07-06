#include <stdio.h>

#include "libavformat/avformat.h"

int main(int argc, char *argv[])
{
	AVFormatContext *c;
	AVPacket pkt1,*pkt = &pkt1;
	const char* url = "rtmp://localhost:1935/live/test";
	const char* url2 = "https://devimages.apple.com.edgekey.net/streaming/examples/bipbop_16x9/bipbop_16x9_variant.m3u8";
	const char* url3 = "rtmp://localhost:1935/vod/test-1467270658.flv";
	printf("%s\n",avformat_configuration());

	avformat_network_init();

	AVInputFormat* iformat = av_find_input_format(url2);
	if (iformat != NULL)
		printf("%s\n",iformat->name);
	else
		printf("iforamt==NULL\n");

	c = avformat_alloc_context();
	c->format_probesize = 1 << 15;
	av_register_all();

	avformat_open_input(&c, url2, NULL, NULL);

	for (int i=0;i<100;i++)
	{
		av_read_frame(c, pkt);
		printf("[%d] pts %ld,dts %ld,size %d\n",i, pkt->pts, pkt->dts, pkt->size);
	}

	avformat_close_input(&c);

	avformat_free_context(c);

	avformat_network_deinit();

	return 0;
}