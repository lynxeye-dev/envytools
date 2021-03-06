/*
 * Copyright (C) 2011 Marcin Kościelnicki <koriakin@0x04.net>
 * Copyright (C) 2011 Emil Velikov <emil.l.velikov@gmail.com>
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "nva.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

int main(int argc, char **argv) {
	if (nva_init()) {
		fprintf (stderr, "PCI init failure!\n");
		return 1;
	}
	int c;
	int cnum =0;
	int alias = 0;
	int slow = 0;
	while ((c = getopt (argc, argv, "asc:")) != -1)
		switch (c) {
			case 'c':
				sscanf(optarg, "%d", &cnum);
				break;
			case 'a':
				alias = 1;
				break;
			case 's':
				slow = 1;
				break;
		}
	if (cnum >= nva_cardsnum) {
		if (nva_cardsnum)
			fprintf (stderr, "No such card.\n");
		else
			fprintf (stderr, "No cards found.\n");
		return 1;
	}
	int32_t a, b = 4, i;
	if (optind >= argc) {
		fprintf (stderr, "No address specified.\n");
		return 1;
	}
	sscanf (argv[optind], "%x", &a);
	if (optind + 1 < argc)
		sscanf (argv[optind + 1], "%x", &b);
	int ls = 1;
	for (i = 0; i < b; i+=4) {
		uint32_t x, y, z;
		x = nva_rd32(cnum, a+i);
		nva_wr32(cnum, a+i, -1);
		y = nva_rd32(cnum, a+i);
		nva_wr32(cnum, a+i, 0);
		z = nva_rd32(cnum, a+i);
		nva_wr32(cnum, a+i, x);
		if (x || y || z) {
			int cool = (x != y) || (y != z);
			int isalias = 0, areg;
			if (cool && alias) {
				int j;
				nva_wr32(cnum, a+i, -1);
				for (j = 0; j < i; j+=4) {
					uint32_t sv = nva_rd32(cnum, a+j);
					nva_wr32(cnum, a+j, 0);
					uint32_t ch = nva_rd32(cnum, a+i);
					nva_wr32(cnum, a+j, sv);
					if (ch == z) {
						areg = a + j;
						isalias = 1;
						break;
					}
				}
				nva_wr32(cnum, a+i, x);
			}
			printf ("%06x: %08x %08x %08x%s", a+i, x, y, z, cool?" *":"");
			if (isalias) {
				printf(" ALIASES %06x", areg);
			}
			printf("\n");
			ls = 1;
		} else {
			if (ls)
				printf("...\n");
			ls = 0;
		}
		if (slow) {
			int j;
			for (j = 0; j < 100; j++)
				nva_rd32(cnum, 0);
			usleep(10000);
			for (j = 0; j < 100; j++)
				nva_rd32(cnum, 0);
		}
	}
	return 0;
}
