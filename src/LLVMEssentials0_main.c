/* vim: set filetype=c : */
/* vim: set noet tw=100 ts=8 sw=8 cinoptions=+4,(0,t0: */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int bar(int32_t, int64_t, int32_t, int64_t);

int main(int argc, char **argv)
{
	if (argc < 4)
		return EXIT_FAILURE;

	int32_t a = (int32_t)strtoul(argv[1], NULL, 0);
	int64_t b = (int64_t)strtoul(argv[2], NULL, 0);
	int32_t c = (int32_t)strtoul(argv[3], NULL, 0);
	printf("bar(%" PRIi32 ", %" PRIi64 ", %" PRIi32 ", %" PRIi32 ") = %d\n",
	       a, b, c, 0, bar(a, b, c, 0));
}
