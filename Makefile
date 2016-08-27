!A = aaa
B = ccc

all:
	@echo $(!A)
	make -C candy
	#make -C tomato
	#make -C arbutus
	
ffmpeg:
	make -C ffmpeg

