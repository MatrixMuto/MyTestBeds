FFMPEG_DIR = ../ffmpeg

DEP  = -lavformat -lavcodec -lavfilter -lavdevice -lavutil
DEP2 = -lva -lcrypto -lssl -lpthread -lm -lz -lbz2
DEP3 = -L$(FFMPEG_DIR)/libswresample -lswresample

all: main.c
	cc -std=c99 main.c -I$(FFMPEG_DIR) $(DEP) $(DEP3) $(DEP2)  -o ../objs/main
