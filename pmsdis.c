/*
 *
 * Copyright (C) 2009 Marcin Kościelnicki <koriakin@0x04.net>
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

#include "dis.h"

/*
 * Immediate fields
 */

static int stdoff[] = { 0xdeaddead };
static int staoff[] = { 0xdeaddead };
#define ADDR16 atomst16, staoff
#define ADDR32 atomst32, staoff
#define DATA16 atomst16, stdoff
#define DATA32 atomst32, stdoff
static void atomst16 APROTO {
	if (!ctx->out)
		return;
	int *n = (int*)v;
	ull num = BF(8, 16);
	n[0] &= 0xffff0000;
	n[0] |= num;
	fprintf (ctx->out, " %s%#x", cyel, n[0]);
}
static void atomst32 APROTO {
	if (!ctx->out)
		return;
	int *n = (int*)v;
	ull num = BF(8, 32);
	n[0] = num;
	fprintf (ctx->out, " %s%#x", cyel, n[0]);
}

static struct insn tabm[] = {
	{ AP, 0x40, 0xff, OP24, N("addr"), ADDR16 },
	{ AP, 0x42, 0xff, OP24, N("data"), DATA16 },
	{ AP, 0x5f, 0xff, OP24, U("5f") },
	{ AP, 0xe0, 0xff, OP40, N("addr"), ADDR32 },
	{ AP, 0xe2, 0xff, OP40, N("data"), DATA32 },
	{ AP, 0x7f, 0xff, OP8, N("exit") },
	{ AP, 0, 0, OP8, OOPS },
};

static struct disisa pms_isa_s = {
	tabm,
	5,
	1,
	1,
};

struct disisa *pms_isa = &pms_isa_s;