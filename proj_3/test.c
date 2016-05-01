#include <fuse.h>
#include <stdio.h>

static int hello {
	printf("hello world\n");
	return 1;
}

static struct fuse_operations test = {
	.hello = hello
};

int main(int argc, char const *argv[])
{
	/* code */
	return fuse_main(argc, argv, &test);
}