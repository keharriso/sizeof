/* Copyright (c) 2013  Kevin Harrison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <ftw.h>
#include <unistd.h>


static const char help[] =
"%s [-bfisBFIS] FILE ...\n"
"\n"
"Options (capitalize to negate):\n"
"b - print bytes (no K, M, G, or T)\n"
"f - print file name (default)\n"
"i - print with base 2 units (default)\n"
"s - sum all sizes together\n";

static int b = 0, f = 1, i = 1, s = 0;

static double size = 0;
static char unit = 0;

static int size_up(const char *fpath, const struct stat *sb, int typeflag) {
	if (typeflag == FTW_F) {
		size += sb->st_size;
	}

	return 0;
}

static void handle_args(int argc, char *argv[]) {
	int k;

	opterr = 0;

	while ((k = getopt(argc, argv, "bfisBFIS")) != -1) {
		switch (k) {
			case 'b': b = 1; break;
			case 'f': f = 1; break;
			case 'i': i = 1; break;
			case 's': s = 1; break;
			case 'B': b = 0; break;
			case 'F': f = 0; break;
			case 'I': i = 0; break;
			case 'S': s = 0; break;
			case '?':
				if (isprint(optopt)) {
					fprintf(stderr, "Unknown option "
					        "`-%c'.\n",
					        optopt);
				} else {
					fprintf(stderr, "Unknown option "
					        "character `\\x%x'.\n",
					        optopt);
				}

				exit(1);
			default:
				abort();
		}
	}

	if (argc == optind) {
		printf(help, argv[0]);
		exit(0);
	}
}

static void format_size(void) {
	if (!b) {
		if (i) {
			if (size >= 1099511627776.0) {
				size /= 1099511627776.0;
				unit = 'T';
			} else if (size >= 1073741824.0) {
				size /= 1073741824.0;
				unit = 'G';
			} else if (size >= 1048576.0) {
				size /= 1048576.0;
				unit = 'M';
			} else if (size >= 1024.0) {
				size /= 1024.0;
				unit = 'K';
			} else {
				unit = 0;
			}
		} else {
			if (size >= 1000000000000.0) {
				size /= 1000000000000.0;
				unit = 'T';
			} else if (size >= 1000000000.0) {
				size /= 1000000000.0;
				unit = 'G';
			} else if (size >= 1000000.0) {
				size /= 1000000.0;
				unit = 'M';
			} else if (size >= 1000.0) {
				size /= 1000.0;
				unit = 'k';
			} else {
				unit = 0;
			}
		}
	}
}

void print_size(void) {
	if (f && !s) {
		if (b) {
			printf("%14llu", (long long unsigned) size);
		} else if (unit) {
			printf("%8.02f", size);
		} else {
			printf("%8llu", (long long unsigned) size);
		}
	} else {
		if (b || !unit) {
			printf("%llu", (long long unsigned) size);
		} else {
			printf("%.02f", size);
		}
	}

	if (unit) {
		printf(" %c", unit);

		if (i) {
			printf("i");
		}
	} else {
		printf("  ");

		if (i) {
			printf(" ");
		}
	}

	if (!b) {
		printf("B");
	}

	printf("\n");
}

int main(int argc, char *argv[]) {
	static int len, max = 0, k;

	handle_args(argc, argv);

	if (f && !s) {
		for (k = optind; k < argc; ++k) {
			len = strlen(argv[k]);

			if (len > max) {
				max = len;
			}
		}
	}

	for (k = optind; k < argc; ++k) {
		if (!s) {
			size = 0;
		}

		ftw(argv[k], size_up, 255);

		if (!s) {
			if (f) {
				printf("%s", argv[k]);
				len = strlen(argv[k]);

				while (len++ < max + 2) {
					putchar(' ');
				}
			}

			format_size();
			print_size();
		}
	}

	if (s) {
		format_size();
		print_size();
	}

	return 0;
}
