#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "colorlut.h"

uint8_t rgb_to_term(int32_t r, int32_t g, int32_t b, uint32_t n)
{
	/* Incredibly dumb brute-force search, and does a very poor job
	 * of actually finding the closest color. Improve please.
	 * */
	
	uint32_t best = 0;
	uint32_t best_error = UINT32_MAX;

	for (uint32_t i = 0; i < n; i++) {
		int32_t error = 
			abs(r - (int32_t)color_lut[i][0]) +
			abs(g - (int32_t)color_lut[i][1]) +
			abs(b - (int32_t)color_lut[i][2]);
		if (error <= best_error) {
			best_error = error;
			best = i;
		}
	}
	return best;
}

char *read_stdin()
{
	size_t buffer_size = 1024*4; // 4kb
	size_t buffer_index = 0;
	char *buffer = malloc(buffer_size);
	int c = 0;
	while((c = getchar()) != EOF) {
		buffer[buffer_index++] = c;
		if (buffer_index == buffer_size) {
			buffer_size*=2;
			buffer = realloc(buffer, buffer_size);
		}
	}
	return buffer;
}

char *read_file(char *path)
{
	FILE *f = fopen(path, "r");

	if (f==NULL) return NULL;

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET); 

	char *string = calloc(fsize, 1 );
	if(string==NULL) return NULL;

	size_t ret = fread(string, 1, fsize, f);
	fclose(f);
	if (ret != fsize) {
		return NULL;
	}
	return string;
}

char *ppm_find_data(char*c)
{
	if (*c++ == 'P' && *c++ == '6'); else return 0;
	c++;
	if (*c == '#') while (*c++ != '\n');
	return c;
}

char *ppm_get_num(char*c, int *n)
{
	int num = 0;
	while (!isspace(*c)) num = num * 10 + (*c++ - '0');
	*n = num;
	return ++c;
}

enum {
	VT,
	TrueColor,
};

int main(int argc, char **argv)
{
	char* file = NULL;

	int mode = 256;
	bool debug = false;

	for (int i = 1; i < argc; i++) {

		if (argv[i][0] != '-') {
			file=read_file(argv[i]);
			if (file == NULL) {
				printf("Failed to read file '%s'\n", argv[i]);
				printf("Try '%s --help'\n", argv[0]);
				return 1;
			}
			continue;
		}

		if(strcmp( argv[i], "--tc") == 0 
	    || strcmp( argv[i], "--truecolor") == 0 ) {
			mode = TrueColor;
			continue;
		}
		if(strcmp( argv[i], "--vt") == 0 ){
			mode = VT;
			continue;
		}

		if(strcmp( argv[i], "--debug") == 0 ){
			debug = true;
			continue;
		}
		
		if(strcmp( argv[i], "--256") == 0 ){
			mode = 256;
			continue;
		}

		if(strcmp( argv[i], "--16") == 0 
		||  strcmp( argv[i], "--ansi") == 0 ) {
		
			mode = 16;
			continue;
		}

		if(strcmp( argv[i], "-v") == 0 
	    || strcmp( argv[i], "--version") == 0 ) {
			printf("%s 1.0\n", argv[0]);
			return 0;
		}
			
		if(strcmp( argv[i], "-") == 0 
	    || strcmp( argv[i], "-i") == 0 ) 
			continue;
		
		if(strcmp( argv[i], "-h") == 0 
	    || strcmp( argv[i], "--help") == 0 ) {
			printf("Usage: %s [OPTION]... [FILE]\n", argv[0]);
			printf("\n");
			printf("  With no FILE, or when FILE is -, read standard input.\n");
			printf("\n");
			printf("  --vt               Linux Virtual Terminal or TTY mode.\n");
			printf("  --tc               TrueColor mode.\n");
			printf("  --256              256 color mode.\n");
			printf("  --16, --ansi       Ansi/16 color mode.\n");
			printf("  -i, -              Read file from stdin.\n");
			printf("  -v, --version      Print version.\n");
			printf("  -h, --help         Print help.\n");

			printf("\n");
			printf("\
Synopsis\n\
  Print PNM image to the terminal with half-blocks (▀).\n\
\n\
File support\n\
  Only PNM files of type P6 (binary color) are supported. This format is default\n\
  output of tools such as 'pngtopnm'.\n\
\n\
Print modes\n\
  Linux Virtual Terminal (--vt)\n\
    Prints full RGB in the Linux TTY, by hacky abuse of color change escape \n\
	sequences and framebuffer persistence. The printed image breaks on any \n\
	scroll or redraw, so clear the screen first. \n\
\n\
  TrueColor (--tc)\n\
    Prints with TrueColor codes. The best method, but requires a \"modern\" \n\
    terminal.\n\
\n\
  256 Color (--256)\n\
    Prints with the standard 256 colors.\n\
\n\
  16 Color (--ansi, --16)\n\
    Prints with the standard 16 colors. \n\
"
);
			return 0;
		}

		printf("Unrecognized option '%s'\n", argv[i]);
		printf("Try '%s --help'\n", argv[0]);
		return 1;
	}

	if (!file) {
		file=read_stdin(); 
	}

	if (!file) return 1;

	char* data = ppm_find_data(file);
	if (!data) {
		printf("Unsupported filetype. Only PNM format P6 is supported.\n");
		printf("Try '%s --help'\n", argv[0]);
		return 1;
	}
	int w = 0, h = 0, z = 0;
	data = ppm_get_num(data, &w);
	data = ppm_get_num(data, &h);
	data = ppm_get_num(data, &z);
	if (debug) printf("%i,%i,%i\n", w,h,z);

	for (int y = 0; y < h/2; ++y) {
		for (int x = 0; x <   w; ++x){
			int32_t i0 = ( x + w * (y*2  ) ) * 3; 
			int32_t i1 = ( x + w * (y*2+1) ) * 3;
			uint8_t r0 = (data[ i0 + 0 ]);
			uint8_t g0 = (data[ i0 + 1 ]);
			uint8_t b0 = (data[ i0 + 2 ]);
			uint8_t r1 = (data[ i1 + 0 ]);
			uint8_t g1 = (data[ i1 + 1 ]);
			uint8_t b1 = (data[ i1 + 2 ]);
		
			if(debug){
				fprintf( stderr, "%i, %i, %i\n", r0, g0, b0 );
				fprintf( stderr, "%i, %i, %i\n", r1, g1, b1 );
			}

			static uint8_t color0;
			static uint8_t color1;

			switch (mode) {
				case VT:
					printf("\e]P0%02x%02x%02x", r1, g1, b1);
					printf("\e]P1%02x%02x%02x", r0, g0, b0);
					printf("\e[0;31m\e[40m▀");
					break;
				case TrueColor:
					/* 
					 * \x1b[38;5;R;G;Bm
					 * Where 38 is fg, 48 is bg
					 */

					printf("\x1b[38;2;%i;%i;%im", r0, g0, b0);
					printf("\x1b[48;2;%i;%i;%im", r1, g1, b1);
					printf("▀");
					break;
				default:
					color0 = rgb_to_term(r0, g0, b0, mode);
					color1 = rgb_to_term(r1, g1, b1, mode);
					printf("\e[38;5;%im", color0);
					printf("\e[48;5;%im", color1);
					printf("▀");
					break;
			}
		}
		printf("\e[0m");
		printf("\n");
	}
	printf("\e[0m");
};
