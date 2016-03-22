#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCKSIZE 512

int main(int argc, char const *argv[])
{
	char buffer[BLOCKSIZE+1];
	int counter = -1;
	int i;

	char out_name[12];
	char test1[] = {0xff, 0xd8, 0xff, 0xe0, '\0'};
	char test2[] = {0xff, 0xd8, 0xff, 0xe1, '\0'};

	FILE *in = fopen("card.raw", "r");
	FILE *out = NULL;

	for (i = 0; fread(buffer, BLOCKSIZE, 1, in) == 1; i++)
	{
		printf("working on block %03d.jpg\n", i);

		if (strncmp(buffer, test1, 4) == 0 || strncmp(buffer, test2, 4) == 0)
		{
			printf("FOUND JPEG\n");
			counter++;
			sprintf(out_name, "img/%03d.jpg", counter);

			printf("test\n");
			if (out) fclose(out);
			out = fopen(out_name, "a");
		}
		if (out) fwrite(buffer, BLOCKSIZE, 1, out);
	}

	fclose(in);
	fclose(out);

	return 0;
}