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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "cube.h"
#include "dlx.h"

struct dlx_data {
    struct matrix_row *matrix;

    double best_score, current_score;
    uint64_t graph;
    int k;
};

static void print_solution(struct dlx_solution *solution) {
    for(struct dlx_solution *i = solution; i; i = i->next) {
        for(int j = 0; j < 60; j++)
            if(i->row->row[j])
                printf("%d ", j);
        for(int j = 0; j < 12; j++)
            if(i->row->row[j + 60])
                printf("[%d] ", j);
        printf("\n");
    }
    printf("\n");
}

static void solution_callback(struct dlx_context *context) {
    struct dlx_data *data = context->dlx_data;
    if(data->best_score < data->current_score)
        data->best_score = data->current_score;

    if(fabs(data->current_score - data->best_score) < 0.001) {
        printf("Score: %f\n", data->current_score);
        print_solution(context->solution);
        fflush(stdout);
    }
}

static inline void hide(struct matrix_row *r) {
    if(r->prev) r->prev->next = r->next;
    if(r->next) r->next->prev = r->prev;
}

static inline void show(struct matrix_row *r) {
    if(r->prev) r->prev->next = r;
    if(r->next) r->next->prev = r;
}

void before(struct dlx_data *data, struct dlx_node *r) {
    struct row_data *row_data = r->row->row_data;
    data->current_score += row_data->weight;
    data->graph |= row_data->flags;
    data->k++;

    for(struct dlx_node *i = r->R; i != r; i = i->R)
        for(struct dlx_node *j = i->C->D; j != i->C; j = j->D)
            hide(j->row);
}

void after(struct dlx_data *data, struct dlx_node *r) {
    struct row_data *row_data = r->row->row_data;
    data->current_score -= row_data->weight;
    data->graph &= ~row_data->flags;
    data->k--;

    for(struct dlx_node *i = r->L; i != r; i = i->L)
        for(struct dlx_node *j = i->C->U; j != i->C; j = j->U)
            show(j->row);
}

static uint64_t flood(uint64_t graph, unsigned node) {
    uint64_t v = 1ull << node;
    if(graph & v) return graph;
    graph |= v;
    return (graph = flood(graph, NEIGHBOUR_MATRIX[node][0])) |
           (graph = flood(graph, NEIGHBOUR_MATRIX[node][1])) |
           (graph = flood(graph, NEIGHBOUR_MATRIX[node][2])) |
           (graph = flood(graph, NEIGHBOUR_MATRIX[node][3]));
}

static bool flood_fill(struct dlx_data *d) {
    if(d->k < 2 || d->k > 4) return false;
    /* Find an empty tile in the graph. */
    unsigned index = __builtin_ctzll(~d->graph);
    return __builtin_popcountll(flood(d->graph, index)) % 5 != 0;
}

static bool sum_max(struct dlx_data *d) {
    struct matrix_row *r = d->matrix;
    double sum = d->current_score;
    for(int i = 0; i < 12 - d->k; i++) {
        struct row_data *rd = r->row_data;
        sum += rd->weight;
        r = r->next;
    }
    return sum < d->best_score;
}

static bool check_max(struct dlx_data *d) {
    return (d->current_score + ((29.0 / 6.0) * (12 - d->k))) < d->best_score;
}

int main() {
    struct matrix_row *matrix = generate_walks();
    struct dlx_solver *solver = dlx_new(72);
    for(struct matrix_row *i = matrix; i; i = i->next)
        dlx_row(solver, i);

    dlx_set_callback(solver, solution_callback);
    dlx_set_ba(solver, (dlx_data_callback) before,
                       (dlx_data_callback) after);

    dlx_add_heuristic(solver, (dlx_heuristic_callback) flood_fill);
    dlx_add_heuristic(solver, (dlx_heuristic_callback) sum_max);
    dlx_add_heuristic(solver, (dlx_heuristic_callback) check_max);

    static struct dlx_data data;
    data.matrix = matrix;

    dlx_solve(solver, &data);
    dlx_free(solver);

    /* Free the matrix. */
    while(matrix) {
        struct matrix_row *tmp = matrix;
        matrix = matrix->next;
        free(tmp->row_data);
        free(tmp->row);
        free(tmp);
    }
}
