/*
 *   This file is part of Cube-Solver (https://github.com/nur1popcorn/Cube-Solver).
 *   Copyright (C) Keanu Poeschko
 *
 *   Cube-Solver is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, version 3.
 *
 *   Cube-Solver is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Cube-Solver.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "cube.h"

#include <stdbool.h>

#include "globals.h"

enum direction GENERATOR_PENTOMINOS[12][4][4] = {
    { { UP, UP, UP, UP } },
    { { UP, RIGHT }, { LEFT }, { DOWN } },
    { { DOWN, DOWN, DOWN, RIGHT } },
    { { UP, UP, RIGHT, DOWN } },
    { { UP, RIGHT, UP, UP } },
    { { LEFT }, { RIGHT }, { DOWN, DOWN } },
    { { DOWN, RIGHT, RIGHT, UP } },
    { { RIGHT, RIGHT, UP, UP } },
    { { RIGHT, UP, RIGHT, UP } },
    { { UP }, { RIGHT }, { DOWN }, { LEFT } },
    { { UP }, { LEFT }, { DOWN, DOWN } },
    { { RIGHT, UP, UP, RIGHT } }
};

int NEIGHBOUR_MATRIX[60][4] = {
    { 55, 15,  4, 59 }, /*  0 */ { 57,  2,  5, 56 }, /*  1 */ { 58,  3,  6,  1 }, /*  2 */
    { 59,  4,  7,  2 }, /*  3 */ {  0, 14,  8,  3 }, /*  4 */ {  1,  6,  9, 22 }, /*  5 */
    {  2,  7, 10,  5 }, /*  6 */ {  3,  8, 11,  6 }, /*  7 */ {  4, 13, 12,  7 }, /*  8 */
    {  5, 10, 16, 23 }, /*  9 */ {  6, 11, 16,  9 }, /* 10 */ {  7, 12, 17, 10 }, /* 11 */
    {  8, 13, 18, 11 }, /* 12 */ {  8, 14, 19, 12 }, /* 13 */ {  4, 15, 20, 13 }, /* 14 */
    {  0, 51, 21, 14 }, /* 15 */ { 10, 17, 24,  9 }, /* 16 */ { 11, 18, 25, 16 }, /* 17 */
    { 12, 19, 26, 17 }, /* 18 */ { 13, 20, 27, 18 }, /* 19 */ { 14, 21, 34, 19 }, /* 20 */
    { 15, 47, 38, 20 }, /* 21 */ {  5, 23, 28, 56 }, /* 22 */ {  9, 24, 29, 22 }, /* 23 */
    { 16, 25, 30, 23 }, /* 24 */ { 17, 26, 31, 24 }, /* 25 */ { 18, 27, 32, 25 }, /* 26 */
    { 19, 34, 33, 26 }, /* 27 */ { 22, 29, 43, 52 }, /* 28 */ { 23, 30, 39, 28 }, /* 29 */
    { 24, 31, 35, 29 }, /* 30 */ { 25, 32, 35, 30 }, /* 31 */ { 26, 33, 36, 31 }, /* 32 */
    { 27, 34, 37, 32 }, /* 33 */ { 27, 20, 38, 33 }, /* 34 */ { 31, 36, 39, 30 }, /* 35 */
    { 32, 37, 40, 35 }, /* 36 */ { 33, 38, 41, 36 }, /* 37 */ { 34, 21, 42, 37 }, /* 38 */
    { 35, 40, 43, 29 }, /* 39 */ { 36, 41, 44, 39 }, /* 40 */ { 37, 42, 45, 40 }, /* 41 */
    { 38, 47, 46, 41 }, /* 42 */ { 39, 44, 48, 28 }, /* 43 */ { 40, 45, 48, 43 }, /* 44 */
    { 41, 46, 49, 44 }, /* 45 */ { 42, 47, 50, 45 }, /* 46 */ { 42, 21, 51, 46 }, /* 47 */
    { 44, 49, 52, 43 }, /* 48 */ { 45, 50, 53, 48 }, /* 49 */ { 46, 51, 54, 49 }, /* 50 */
    { 47, 15, 55, 50 }, /* 51 */ { 48, 53, 56, 28 }, /* 52 */ { 49, 54, 57, 52 }, /* 53 */
    { 50, 55, 58, 53 }, /* 54 */ { 51,  0, 59, 54 }, /* 55 */ { 52, 57,  1, 22 }, /* 56 */
    { 53, 58,  1, 56 }, /* 57 */ { 54, 59,  2, 57 }, /* 58 */ { 55,  0,  3, 58 }, /* 59 */
};

int ROTATION_MATRIX[60][4] = {
    { 3, 1, 0, 0 }, /*  0 */ { 0, 0, 0, 1 }, /*  1 */ { 0, 0, 0, 0 }, /*  2 */
    { 0, 0, 0, 0 }, /*  3 */ { 0, 1, 0, 0 }, /*  4 */ { 0, 0, 0, 3 }, /*  5 */
    { 0, 0, 0, 0 }, /*  6 */ { 0, 0, 0, 0 }, /*  7 */ { 0, 1, 0, 0 }, /*  8 */
    { 0, 0, 3, 3 }, /*  9 */ { 0, 0, 0, 0 }, /* 10 */ { 0, 0, 0, 0 }, /* 11 */
    { 0, 0, 0, 0 }, /* 12 */ { 3, 0, 0, 0 }, /* 13 */ { 3, 0, 0, 0 }, /* 14 */
    { 3, 2, 0, 0 }, /* 15 */ { 0, 0, 0, 1 }, /* 16 */ { 0, 0, 0, 0 }, /* 17 */
    { 0, 0, 0, 0 }, /* 18 */ { 0, 0, 0, 0 }, /* 19 */ { 0, 0, 1, 0 }, /* 20 */
    { 0, 2, 1, 0 }, /* 21 */ { 1, 0, 0, 2 }, /* 22 */ { 1, 0, 0, 0 }, /* 23 */
    { 0, 0, 0, 0 }, /* 24 */ { 0, 0, 0, 0 }, /* 25 */ { 0, 0, 0, 0 }, /* 26 */
    { 0, 1, 0, 0 }, /* 27 */ { 0, 0, 3, 2 }, /* 28 */ { 0, 0, 3, 0 }, /* 29 */
    { 0, 0, 3, 0 }, /* 30 */ { 0, 0, 0, 0 }, /* 31 */ { 0, 0, 0, 0 }, /* 32 */
    { 0, 0, 0, 0 }, /* 33 */ { 3, 3, 0, 0 }, /* 34 */ { 0, 0, 0, 1 }, /* 35 */
    { 0, 0, 0, 0 }, /* 36 */ { 0, 0, 0, 0 }, /* 37 */ { 0, 3, 0, 0 }, /* 38 */
    { 0, 0, 0, 1 }, /* 39 */ { 0, 0, 0, 0 }, /* 40 */ { 0, 0, 0, 0 }, /* 41 */
    { 0, 1, 0, 0 }, /* 42 */ { 0, 0, 3, 1 }, /* 43 */ { 0, 0, 0, 0 }, /* 44 */
    { 0, 0, 0, 0 }, /* 45 */ { 0, 0, 0, 0 }, /* 46 */ { 3, 2, 0, 0 }, /* 47 */
    { 0, 0, 0, 1 }, /* 48 */ { 0, 0, 0, 0 }, /* 49 */ { 0, 0, 0, 0 }, /* 50 */
    { 0, 2, 0, 0 }, /* 51 */ { 0, 0, 0, 2 }, /* 52 */ { 0, 0, 0, 0 }, /* 53 */
    { 0, 0, 0, 0 }, /* 54 */ { 0, 1, 0, 0 }, /* 55 */ { 0, 0, 3, 2 }, /* 56 */
    { 0, 0, 0, 0 }, /* 57 */ { 0, 0, 0, 0 }, /* 58 */ { 0, 0, 0, 0 }, /* 59 */
};

/* The split cube areas. */
static const double S = 1.0 / 6.0, M = 1.0 / 2.0, B = 5.0 / 6.0, N = 1;
double AREA_MATRIX[60][6] = {
   /* 0  1  2  3  4  5               0  1  2  3  4  5 */
    { S, 0, 0, B, 0, 0 }, /*  0 */ { B, S, 0, 0, 0, 0 }, /*  1 */
    { N, 0, 0, 0, 0, 0 }, /*  2 */ { N, 0, 0, 0, 0, 0 }, /*  3 */
    { M, 0, 0, M, 0, 0 }, /*  4 */ { M, M, 0, 0, 0, 0 }, /*  5 */
    { N, 0, 0, 0, 0, 0 }, /*  6 */ { N, 0, 0, 0, 0, 0 }, /*  7 */
    { B, 0, 0, S, 0, 0 }, /*  8 */ { S, B, 0, 0, 0, 0 }, /*  9 */
    { B, 0, S, 0, 0, 0 }, /* 10 */ { M, 0, M, 0, 0, 0 }, /* 11 */
    { S, 0, B, 0, 0, 0 }, /* 12 */ { 0, 0, S, B, 0, 0 }, /* 13 */
    { 0, 0, 0, N, 0, 0 }, /* 14 */ { 0, 0, 0, N, 0, 0 }, /* 15 */
    { 0, S, B, 0, 0, 0 }, /* 16 */ { 0, 0, N, 0, 0, 0 }, /* 17 */
    { 0, 0, N, 0, 0, 0 }, /* 18 */ { 0, 0, M, M, 0, 0 }, /* 19 */
    { 0, 0, 0, N, 0, 0 }, /* 20 */ { 0, 0, 0, N, 0, 0 }, /* 21 */
    { 0, N, 0, 0, 0, 0 }, /* 22 */ { 0, N, 0, 0, 0, 0 }, /* 23 */
    { 0, M, M, 0, 0, 0 }, /* 24 */ { 0, 0, N, 0, 0, 0 }, /* 25 */
    { 0, 0, N, 0, 0, 0 }, /* 26 */ { 0, 0, B, S, 0, 0 }, /* 27 */
    { 0, N, 0, 0, 0, 0 }, /* 28 */ { 0, N, 0, 0, 0, 0 }, /* 29 */
    { 0, B, S, 0, 0, 0 }, /* 30 */ { 0, 0, B, 0, S, 0 }, /* 31 */
    { 0, 0, M, 0, M, 0 }, /* 32 */ { 0, 0, S, 0, B, 0 }, /* 33 */
    { 0, 0, 0, B, S, 0 }, /* 34 */ { 0, S, 0, 0, B, 0 }, /* 35 */
    { 0, 0, 0, 0, N, 0 }, /* 36 */ { 0, 0, 0, 0, N, 0 }, /* 37 */
    { 0, 0, 0, M, M, 0 }, /* 38 */ { 0, M, 0, 0, M, 0 }, /* 39 */
    { 0, 0, 0, 0, N, 0 }, /* 40 */ { 0, 0, 0, 0, N, 0 }, /* 41 */
    { 0, 0, 0, S, B, 0 }, /* 42 */ { 0, B, 0, 0, S, 0 }, /* 43 */
    { 0, 0, 0, 0, B, S }, /* 44 */ { 0, 0, 0, 0, M, M }, /* 45 */
    { 0, 0, 0, 0, S, B }, /* 46 */ { 0, 0, 0, B, 0, S }, /* 47 */
    { 0, S, 0, 0, 0, B }, /* 48 */ { 0, 0, 0, 0, 0, N }, /* 49 */
    { 0, 0, 0, 0, 0, N }, /* 50 */ { 0, 0, 0, M, 0, M }, /* 51 */
    { 0, M, 0, 0, 0, M }, /* 52 */ { 0, 0, 0, 0, 0, N }, /* 53 */
    { 0, 0, 0, 0, 0, N }, /* 54 */ { 0, 0, 0, S, 0, B }, /* 55 */
    { 0, B, 0, 0, 0, S }, /* 56 */ { S, 0, 0, 0, 0, B }, /* 57 */
    { M, 0, 0, 0, 0, M }, /* 58 */ { B, 0, 0, 0, 0, S }, /* 59 */
};

/**
 * @param s The starting node of the walk.
 * @param rot The starting rotation of the walk.
 * @param seq The sequence of steps to perform.
 *
 * @return The nodes which were visited during the graph walk.
 */
static struct matrix_row *generate_walk(int s, int rot, enum direction seq[][4]) {
    struct matrix_row *row = calloc_s(1, sizeof(*row));
    row->row = calloc_s(72, sizeof(int));

    struct row_data *data = calloc_s(1, sizeof(*data));
    row->row_data = data;

    data->flags |= 1llu << (unsigned) s;
    row->row[s] = 1;
    for(int i = 0; i < 4; i++) {
        int curr_pos = s;
        int curr_rot = rot;
        for(int j = 0; j < 4; j++) {
            enum direction next = seq[i][j];
            if(next == NOP) break;
            next = rotate(next, curr_rot);
            curr_rot += ROTATION_MATRIX[curr_pos][next - 1];
            curr_pos = NEIGHBOUR_MATRIX[curr_pos][next - 1];
            /* Ensure that 5 unique nodes were visited. */
            if(row->row[curr_pos] == 1) {
                free(row->row_data);
                free(row->row);
                free(row);
                return NULL;
            }
            data->flags |= 1llu << (unsigned) curr_pos;
            row->row[curr_pos] = 1;
        }
    }
    return row;
}

/**
 * @param a[in] The 1st row which is to be compared to the 2nd.
 * @param b[in] The 2nd row which is to be compared to the 1st.
 *
 * @return True iff the two rows are the same false otherwise.
 */
static bool row_equals(struct matrix_row *a, struct matrix_row *b) {
    for(int i = 0; i < 72; i++)
        if(a->row[i] != b->row[i])
            return false;
    return true;
}

/**
 * Simply removes duplicates from the linked list. This might not be the fastest
 * approach but its fast enough.
 *
 * @param[in,out] matrix The matrix which is to be optimized.
 */
static void dedupe_walks(struct matrix_row *matrix) {
    struct matrix_row *d = NULL;
    for(struct matrix_row *i = matrix; i; i = i->next)
        for(struct matrix_row *j = i->next, *p = i; j; p = j, j = j->next)
            if(row_equals(i, j))
                /* Mark as to be deleted and unlink. */
                p->next = j->next, j->next = d, d = j, j = p;

    while(d) {
        struct matrix_row *tmp = d;
        d = d->next;
        free(tmp->row_data);
        free(tmp->row);
        free(tmp);
    }
}

/**
 * Assigns each walk a weight.
 * @param matrix[in,out] The matrix which is to be weighted.
 */
static void weight_walks(struct matrix_row *matrix) {
    for(struct matrix_row *i = matrix; i; i = i->next) {
        struct row_data *data = i->row_data;
        /* Construct the function. */
        double weights[6] = { };
        for(int j = 0; j < 60; j++)
            if(i->row[j])
                for(int k = 0; k < 6; k++)
                    weights[k] += AREA_MATRIX[j][k];
        /* Maximize the function. */
        double max = weights[0];
        for(int j = 1; j < 6; j++)
            if(max < weights[j])
                max = weights[j];
        data->weight = max;
    }
}

/**
 * @param a[in] The 1st parameter that should be compared to the 2nd.
 * @param b[in] The 2nd parameter that should be compared to the 1st.
 * @return A value less than 0 iff a < b, 0 iff a = b and a value greater than 0 iff a > b.
 */
static int cmp_matrix_row(const void *a, const void *b) {
    struct row_data *ad = (*(struct matrix_row **) a)->row_data;
    struct row_data *bd = (*(struct matrix_row **) b)->row_data;

    return ad->weight > bd->weight ? -1 :
           ad->weight < bd->weight ?  1 : 0;
}

/**
 * @param matrix[in] The rows which are to be sorted.
 * @return The sorted linked list.
 */
static struct matrix_row *sort_walks(struct matrix_row *matrix) {
    int size = 0;
    for(struct matrix_row *i = matrix; i; i = i->next)
        size++;

    struct matrix_row *rows[size + 1], **tmp = rows;
    for(struct matrix_row *i = matrix; i; i = i->next)
        *tmp = i, tmp++;
    rows[size] = NULL;

    qsort(rows, size, sizeof(struct matrix_row *), cmp_matrix_row);

    rows[0]->next = rows[1];
    for(int i = 1; i < size; i++)
        rows[i]->prev = rows[i - 1],
        rows[i]->next = rows[i + 1];
    return rows[0];
}

struct matrix_row *generate_walks() {
    struct matrix_row *result = NULL;
    enum direction flipped[4][4];
    for(int i = 0; i < 12; i++) {
        /* Flip the walk. */
        for(int j = 0; j < 4; j++)
            for(int k = 0; k < 4; k++)
                flipped[j][k] = flip(GENERATOR_PENTOMINOS[i][j][k]);

        for(int j = 0; j < 60; j++) {
            for(int k = 1; k < 5; k++) {
                struct matrix_row *walk = generate_walk(j, k, GENERATOR_PENTOMINOS[i]);
                if(walk)
                    walk->next = result, walk->row[i + 60] = 1, result = walk;
            }

            for(int k = 1; k < 5; k++) {
                struct matrix_row *walk = generate_walk(j, k, flipped);
                if(walk)
                    walk->next = result, walk->row[i + 60] = 1, result = walk;
            }
        }
    }
    dedupe_walks(result);
    weight_walks(result);
    result = sort_walks(result);
    return result;
}
