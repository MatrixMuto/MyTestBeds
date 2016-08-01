#include <stdio.h>
#include <string.h>

char *file_name = 0;

void get_options(int argc, char* argv[])
{
	int i;
	char *p;
	for (i = 1; i < argc; i++)
	{
		p = argv[i];

		if (*p != '-') {
			file_name = strdup(p);
		}
	}
}

void analaze_h264()
{
	FILE *file;
	char buf[4096];
	char *ptr, *end;
	int len;
	file = fopen(file_name, "r");
	if (file == NULL) {
		printf("fopen: error");
		return;
	}

	while ( (len = fread(buf,1,4096,file))>0) {
		printf("len=%d\n",len);
		ptr = buf;
		end = buf + len;

		while (ptr < end) {
			ptr++;
		}
	}

	fclose(file);
}

int main(int argc, char* argv[])
{
	get_options(argc,argv);
	if (!file_name) {
		printf("No input file.\n");
		return 0;
	}

	analaze_h264();
    return 0;
}