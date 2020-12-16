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

#ifndef DLX_H
#define DLX_H

#include <stdint.h>
#include <stdbool.h>

struct dlx_solver;
struct dlx_node {
    union {
        struct { int size, col_id; };
        struct { struct dlx_node *C; struct matrix_row *row; };
    };
    struct dlx_node *U, *R, *D, *L;
};

/* A row in the cover matrix. */
struct matrix_row {
    int *row;
    void *row_data;
    struct matrix_row *prev, *next;
};

/* The solution returned by the solver. */
struct dlx_solution {
    struct matrix_row *row;
    struct dlx_solution *next;
};

/* The context used by the solver. */
struct dlx_context {
    struct dlx_solution *solution;
    void *dlx_data;
};

typedef void (*dlx_solution_callback)(struct dlx_context *context);
typedef void (*dlx_data_callback)(void *dlx_data, struct dlx_node *row);
typedef bool (*dlx_heuristic_callback)(void *dlx_data);

/**
 * @param column_count The number of columns in the matrix.
 * @return A new instance of the dlx solver.
 */
struct dlx_solver *dlx_new(int column_count);

/**
 * @param dlx[in] The instance of the solver which is to be freed.
 */
void dlx_free(struct dlx_solver *dlx);

/**
 * Inserts the given row into the matrix.
 * @param row[in] The row vector.
 * @param count The size of the vector.
 */
void dlx_row(struct dlx_solver *dlx, struct matrix_row *row);

/**
 * Adds a callback to the solver. <i>NOTE:</i> that solution instances will eventually be
 * overridden and must therefore be copied if they are to remain in memory.
 *
 * @param dlx[in] The solver instance to which the callback should be added.
 * @param callback[in] The callback which is supposed to be added to the solver.
 */
void dlx_set_callback(struct dlx_solver *dlx, dlx_solution_callback callback);

/**
 * Adds a before and after callback to the solver instance.
 *
 * @param dlx[in] The solver instance to which the callback should be added.
 * @param before[in] The before callback which is supposed to be added to the solver.
 * @param after[in] The after callback which is supposed to be added to the solver.
 */
void dlx_set_ba(struct dlx_solver *dlx, dlx_data_callback before, dlx_data_callback after);

/**
 * Adds a heuristic callback to the solver which is invoked after a row choice.
 *
 * @param dlx[in] The solver instance to which the callback should be added.
 * @param callback[in] The callback which is supposed to be added to the solver.
 */
void dlx_add_heuristic(struct dlx_solver *dlx, dlx_heuristic_callback callback);

/**
 * Lists all solutions to the exact cover problem calling the callback whenever a solution is found.
 * @param dlx[in] The instance of the solver which is to be used.
 * @param dlx_data[in] Some additional data which may be utilized by the heuristics or NULL if non is needed.
 */
void dlx_solve(struct dlx_solver *dlx, void *dlx_data);

#endif /* DLX_H */
