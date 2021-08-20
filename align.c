/******************************************************************************\
 align.c
 Copyright (C) Ian Korf
\******************************************************************************/

#ifndef GENOMIKON_ALIGN_C
#define GENOMIKON_ALIGN_C

#include "align.h"

// scoring matrices

static int B40[25][25] = {
	{5,0,-2,-1,-1,-3,1,-2,-1,0,-1,-2,-1,-1,0,-2,0,-2,1,0,0,0,-3,0,-2},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{-2,0,16,-2,-2,-2,-3,-4,-4,0,-3,-2,-3,-2,0,-5,-4,-3,-1,-1,0,-2,-6,-2,-4},
	{-1,0,-2,9,2,-4,-2,0,-4,0,0,-3,-3,2,0,-2,-1,-1,0,-1,0,-3,-5,-1,-3},
	{-1,0,-2,2,7,-3,-3,0,-4,0,1,-2,-2,-1,0,0,2,-1,0,-1,0,-3,-2,-1,-2},
	{-3,0,-2,-4,-3,9,-3,-2,1,0,-3,2,0,-3,0,-4,-4,-2,-2,-1,0,0,1,-1,4},
	{1,0,-3,-2,-3,-3,8,-2,-4,0,-2,-4,-2,0,0,-1,-2,-3,0,-2,0,-4,-2,-1,-3},
	{-2,0,-4,0,0,-2,-2,13,-3,0,-1,-2,1,1,0,-2,0,0,-1,-2,0,-4,-5,-1,2},
	{-1,0,-4,-4,-4,1,-4,-3,6,0,-3,2,1,-2,0,-2,-3,-3,-2,-1,0,4,-3,-1,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{-1,0,-3,0,1,-3,-2,-1,-3,0,6,-2,-1,0,0,-1,1,3,0,0,0,-2,-2,-1,-1},
	{-2,0,-2,-3,-2,2,-4,-2,2,0,-2,6,3,-3,0,-4,-2,-2,-3,-1,0,2,-1,-1,0},
	{-1,0,-3,-3,-2,0,-2,1,1,0,-1,3,7,-2,0,-2,-1,-1,-2,-1,0,1,-2,0,1},
	{-1,0,-2,2,-1,-3,0,1,-2,0,0,-3,-2,8,0,-2,1,0,1,0,0,-3,-4,-1,-2},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{-2,0,-5,-2,0,-4,-1,-2,-2,0,-1,-4,-2,-2,0,11,-2,-3,-1,0,0,-3,-4,-2,-3},
	{0,0,-4,-1,2,-4,-2,0,-3,0,1,-2,-1,1,0,-2,8,2,1,-1,0,-3,-1,-1,-1},
	{-2,0,-3,-1,-1,-2,-3,0,-3,0,3,-2,-1,0,0,-3,2,9,-1,-2,0,-2,-2,-1,-1},
	{1,0,-1,0,0,-2,0,-1,-2,0,0,-3,-2,1,0,-1,1,-1,5,2,0,-1,-5,0,-2},
	{0,0,-1,-1,-1,-1,-2,-2,-1,0,0,-1,-1,0,0,0,-1,-2,2,6,0,1,-4,0,-1},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,-2,-3,-3,0,-4,-4,4,0,-2,2,1,-3,0,-3,-3,-2,-1,1,0,5,-3,-1,-1},
	{-3,0,-6,-5,-2,1,-2,-5,-3,0,-2,-1,-2,-4,0,-4,-1,-2,-5,-4,0,-3,19,-2,3},
	{0,0,-2,-1,-1,-1,-1,-1,-1,0,-1,-1,0,-1,0,-2,-1,-1,0,0,0,-1,-2,-1,-1},
	{-2,0,-4,-3,-2,4,-3,2,0,0,-1,0,1,-2,0,-3,-1,-1,-2,-1,0,-1,3,-1,9},
};

static int B62[25][25] = {
	{4,0,0,-2,-1,-2,0,-2,-1,0,-1,-1,-1,-2,0,-1,-1,-1,1,0,0,0,-3,0,-2},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,9,-3,-4,-2,-3,-3,-1,0,-3,-1,-1,-3,0,-3,-3,-3,-1,-1,0,-1,-2,-2,-2},
	{-2,0,-3,6,2,-3,-1,-1,-3,0,-1,-4,-3,1,0,-1,0,-2,0,-1,0,-3,-4,-1,-3},
	{-1,0,-4,2,5,-3,-2,0,-3,0,1,-3,-2,0,0,-1,2,0,0,-1,0,-2,-3,-1,-2},
	{-2,0,-2,-3,-3,6,-3,-1,0,0,-3,0,0,-3,0,-4,-3,-3,-2,-2,0,-1,1,-1,3},
	{0,0,-3,-1,-2,-3,6,-2,-4,0,-2,-4,-3,0,0,-2,-2,-2,0,-2,0,-3,-2,-1,-3},
	{-2,0,-3,-1,0,-1,-2,8,-3,0,-1,-3,-2,1,0,-2,0,0,-1,-2,0,-3,-2,-1,2},
	{-1,0,-1,-3,-3,0,-4,-3,4,0,-3,2,1,-3,0,-3,-3,-3,-2,-1,0,3,-3,-1,-1},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{-1,0,-3,-1,1,-3,-2,-1,-3,0,5,-2,-1,0,0,-1,1,2,0,-1,0,-2,-3,-1,-2},
	{-1,0,-1,-4,-3,0,-4,-3,2,0,-2,4,2,-3,0,-3,-2,-2,-2,-1,0,1,-2,-1,-1},
	{-1,0,-1,-3,-2,0,-3,-2,1,0,-1,2,5,-2,0,-2,0,-1,-1,-1,0,1,-1,-1,-1},
	{-2,0,-3,1,0,-3,0,1,-3,0,0,-3,-2,6,0,-2,0,0,1,0,0,-3,-4,-1,-2},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{-1,0,-3,-1,-1,-4,-2,-2,-3,0,-1,-3,-2,-2,0,7,-1,-2,-1,-1,0,-2,-4,-2,-3},
	{-1,0,-3,0,2,-3,-2,0,-3,0,1,-2,0,0,0,-1,5,1,0,-1,0,-2,-2,-1,-1},
	{-1,0,-3,-2,0,-3,-2,0,-3,0,2,-2,-1,0,0,-2,1,5,-1,-1,0,-3,-3,-1,-2},
	{1,0,-1,0,0,-2,0,-1,-2,0,0,-2,-1,1,0,-1,0,-1,4,1,0,-2,-3,0,-2},
	{0,0,-1,-1,-1,-2,-2,-2,-1,0,-1,-1,-1,0,0,-1,-1,-1,1,5,0,0,-2,0,-2},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,0,-1,-3,-2,-1,-3,-3,3,0,-2,1,1,-3,0,-2,-2,-3,-2,0,0,4,-3,-1,-1},
	{-3,0,-2,-4,-3,1,-2,-2,-3,0,-3,-2,-1,-4,0,-4,-2,-3,-3,-2,0,-3,11,-2,2},
	{0,0,-2,-1,-1,-1,-1,-1,-1,0,-1,-1,-1,-1,0,-2,-1,-1,0,0,0,-1,-2,-1,-1},
	{-2,0,-2,-3,-2,3,-3,2,-1,0,-2,-1,-1,-2,0,-3,-1,-2,-2,-2,0,-1,2,-1,7},
};

static int B80[25][25] = {
	{7,0,-1,-3,-2,-4,0,-3,-3,0,-1,-3,-2,-3,0,-1,-2,-3,2,0,0,-1,-5,-1,-4},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{-1,0,13,-7,-7,-4,-6,-7,-2,0,-6,-3,-3,-5,0,-6,-5,-6,-2,-2,0,-2,-5,-4,-5},
	{-3,0,-7,10,2,-6,-3,-2,-7,0,-2,-7,-6,2,0,-3,-1,-3,-1,-2,0,-6,-8,-3,-6},
	{-2,0,-7,2,8,-6,-4,0,-6,0,1,-6,-4,-1,0,-2,3,-1,-1,-2,0,-4,-6,-2,-5},
	{-4,0,-4,-6,-6,10,-6,-2,-1,0,-5,0,0,-6,0,-6,-5,-5,-4,-4,0,-2,0,-3,4},
	{0,0,-6,-3,-4,-6,9,-4,-7,0,-3,-7,-5,-1,0,-5,-4,-4,-1,-3,0,-6,-6,-3,-6},
	{-3,0,-7,-2,0,-2,-4,12,-6,0,-1,-5,-4,1,0,-4,1,0,-2,-3,0,-5,-4,-2,3},
	{-3,0,-2,-7,-6,-1,-7,-6,7,0,-5,2,2,-6,0,-5,-5,-5,-4,-2,0,4,-5,-2,-3},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{-1,0,-6,-2,1,-5,-3,-1,-5,0,8,-4,-3,0,0,-2,2,3,-1,-1,0,-4,-6,-2,-4},
	{-3,0,-3,-7,-6,0,-7,-5,2,0,-4,6,3,-6,0,-5,-4,-4,-4,-3,0,1,-4,-2,-2},
	{-2,0,-3,-6,-4,0,-5,-4,2,0,-3,3,9,-4,0,-4,-1,-3,-3,-1,0,1,-3,-2,-3},
	{-3,0,-5,2,-1,-6,-1,1,-6,0,0,-6,-4,9,0,-4,0,-1,1,0,0,-5,-7,-2,-4},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{-1,0,-6,-3,-2,-6,-5,-4,-5,0,-2,-5,-4,-4,0,12,-3,-3,-2,-3,0,-4,-7,-3,-6},
	{-2,0,-5,-1,3,-5,-4,1,-5,0,2,-4,-1,0,0,-3,9,1,-1,-1,0,-4,-4,-2,-3},
	{-3,0,-6,-3,-1,-5,-4,0,-5,0,3,-4,-3,-1,0,-3,1,9,-2,-2,0,-4,-5,-2,-4},
	{2,0,-2,-1,-1,-4,-1,-2,-4,0,-1,-4,-3,1,0,-2,-1,-2,7,2,0,-3,-6,-1,-3},
	{0,0,-2,-2,-2,-4,-3,-3,-2,0,-1,-3,-1,0,0,-3,-1,-2,2,8,0,0,-5,-1,-3},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
	{-1,0,-2,-6,-4,-2,-6,-5,4,0,-4,1,1,-5,0,-4,-4,-4,-3,0,0,7,-5,-2,-3},
	{-5,0,-5,-8,-6,0,-6,-4,-5,0,-6,-4,-3,-7,0,-7,-4,-5,-6,-5,0,-5,16,-5,3},
	{-1,0,-4,-3,-2,-3,-3,-2,-2,0,-2,-2,-2,-2,0,-3,-2,-2,-1,-1,0,-2,-5,-2,-3},
	{-4,0,-5,-6,-5,4,-6,3,-3,0,-4,-2,-3,-4,0,-6,-3,-4,-3,-3,0,-3,3,-3,11},
};

void gkn_smat_free(gkn_smat mat) {
	free(mat);
}

gkn_smat gkn_smat_blosum(int n) {
	gkn_smat mat = malloc(sizeof(struct gkn_SMAT));

	// set name
	switch (n) {
		case 40: strcpy(mat->name, "BLOSUM40"); break;
		case 62: strcpy(mat->name, "BLOSUM62"); break;
		case 80: strcpy(mat->name, "BLOSUM80"); break;
		default: gkn_exit("invalid matrix number");
	}

	// set gap
	switch (n) {
		case 40: mat->gap = -4;
		case 62: mat->gap = -5;
		case 80: mat->gap = -7;
	}

	// clear out
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 25; j++) {
			mat->score[i][j] = INT_MIN;
		}
	}

	// transfer scores
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 25; j++) {
			switch (n) {
				case 40: mat->score[i][j] = B40[i][j]; break;
				case 62: mat->score[i][j] = B62[i][j]; break;
				case 80: mat->score[i][j] = B80[i][j]; break;
			}
		}
	}

	return mat;
}

gkn_smat gkn_smat_mng(int m, int n, int g) {
	gkn_smat mat = malloc(sizeof(struct gkn_SMAT));

	assert(m > 0);
	assert(n < 0);
	assert(g < 0);

	sprintf(mat->name, "+%d/%d/%d", m, n, g);
	mat->gap = g;
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 25; j++) {
			if (i == j) mat->score[i][j] = m;
			else        mat->score[i][j] = n;
		}
	}

	return mat;
}

// HSP

void gkn_hsp_free (gkn_hsp hsp) {
	free(hsp->s1);
	free(hsp->s2);
	free(hsp->as);
	free(hsp);
}

gkn_hsp gkn_hsp_new (void) {
	gkn_hsp hsp = gkn_malloc(sizeof(struct gkn_HSP));
	hsp->beg1 = 0;
	hsp->end1 = 0;
	hsp->beg2 = 0;
	hsp->end2 = 0;
	hsp->length = 0;
	hsp->score = 0;
	hsp->s1 = NULL;
	hsp->s2 = NULL;
	hsp->as = NULL;
	return hsp;
}

// alignment

gkn_hsp gkn_sw(const char *s1, const char *s2, gkn_smat m) {
	int l1 = strlen(s1);
	int l2 = strlen(s2);

	// allocate matrices
	int  ** sm = malloc(sizeof(double*) * (l1+1));
	char ** tm = malloc(sizeof(double*) * (l1+1));
	for (int i = 0; i <= l1; i++)  sm[i] = malloc(sizeof(double) * (l2+1));
	for (int i = 0; i <= l1; i++)  tm[i] = malloc(sizeof(double) * (l2+1));

	// init first column and row
	for (int i = 0; i <= l1; i++) sm[i][0] = 0, tm[i][0] = '.';
	for (int j = 1; j <= l2; j++) sm[0][j] = 0, tm[0][j] = '.';

	// fill matrix
	int max_s = 0;
	int max_i = 0;
	int max_j = 0;
	for (int i = 1; i <= l1; i++) {
		for (int j = 1; j <= l2; j++) {
			int a = s1[i-1] - 'A';
			int b = s2[j-1] - 'A';
			int d = sm[i-1][j-1] + m->score[a][b];
			int h = sm[i-1][j  ] + m->gap;
			int v = sm[i  ][j-1] + m->gap;

			if (d > h && d > v && d > 0) sm[i][j] = d, tm[i][j] = 'd';
			else if (h > v && h > 0)     sm[i][j] = h, tm[i][j] = 'h';
			else if (v > 0)              sm[i][j] = v, tm[i][j] = 'v';
			else                         sm[i][j] = 0, tm[i][j] = '.';

			if (d > max_s) {
				max_s = d;
				max_i = i;
				max_j = j;
			}
		}
	}

	// allocate alignment strings
	char *a1 = malloc((l1 + l2 + 1) * sizeof(char));
	char *a2 = malloc((l1 + l2 + 1) * sizeof(char));
	char *a3 = malloc((l1 + l2 + 1) * sizeof(char));

	// traceback
	int i = max_i;
	int j = max_j;
	int pos = 0;
	int min_i = -1;
	int min_j = -1;
	while (sm[i][j] > 0) {
		if (tm[i][j] == 'd') {
			a1[pos] = s1[i-1];
			a2[pos] = s2[j-1];
			i--;
			j--;
		} else if (tm[i][j] == 'v') {
			a1[pos] = '-';
			a2[pos] = s2[j-1];
			j--;
		} else if (tm[i][j] == 'h') {
			a1[pos] = s1[i-1];
			a2[pos] = '-';
			i--;
		}
		min_i = i;
		min_j = j;
		pos++;
	}
	a1[pos] = '\0';
	a2[pos] = '\0';
	a3[pos] = '\0';

	// return values
	gkn_hsp hsp = gkn_hsp_new();
	hsp->score = max_s;
	hsp->length = strlen(a1);
	hsp->beg1 = min_i;
	hsp->end1 = max_i -1;
	hsp->beg2 = min_j;
	hsp->end2 = max_j -1;
	hsp->s1 = malloc(hsp->length +1);
	hsp->s2 = malloc(hsp->length +1);
	hsp->as = malloc(hsp->length +1);

	for (int i = 0; i < hsp->length; i++) {
		int pos = hsp->length -i -1;
		hsp->s1[pos] = a1[i];
		hsp->s2[pos] = a2[i];
	}
	for (int i = 0; i< hsp->length; i++) {
		if (hsp->s1[i] == hsp->s2[i]) hsp->as[i] = '|';
		else                          hsp->as[i] = ' ';
	}
	hsp->s1[hsp->length] = '\0';
	hsp->s2[hsp->length] = '\0';
	hsp->as[hsp->length] = '\0';

	// clean up
	free(a1);
	free(a2);
	free(a3);
	for (i = 0; i <= l1; i++) {
		free(sm[i]);
		free(tm[i]);
	}
	free(sm);
	free(tm);

	return hsp;
}

#endif
