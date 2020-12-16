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

#include "dlx.h"
#include "globals.h"

/**
 * @param col_id The column's id.
 * @param prev[in] The previous column object.
 * @param next[in] The next column object.
 *
 * @return A new instance of a column object.
 */
static struct dlx_node *column_new(int col_id, struct dlx_node *prev, struct dlx_node *next) {
    struct dlx_node *column = malloc_s(sizeof(*column));
    column->U = column->D = column;
    column->R = next, column->L = prev;
    column->size = 0, column->col_id = col_id;
    return column;
}

/**
 * @param row[in] A pointer to the row associated with this node.
 * @param L[in] The node which is located to the left of this node or NULL if there is non.
 * @param C[in] The column which is associated with this node.
 *
 * @return A new instance of a node.
 */
static struct dlx_node *node_new(struct matrix_row *row, struct dlx_node *L, struct dlx_node *C) {
    struct dlx_node *node = malloc_s(sizeof(*node));
    if(L) node->R = L->R, node->L = L;
    else node->R = node->L = node;
    node->U = C->U, node->C = node->D = C;
    node->row = row;
    return node;
}

/**
 * Unlinks the given node horizontally.
 * @param node[in] The node which is to be unlinked horizontally.
 */
static inline void hide_h(struct dlx_node *node) {
    node->L->R = node->R, node->R->L = node->L;
}

/**
 * Restores the current node's horizontal links.
 * @param node[in] The node whose links are to be restored horizontally.
 */
static inline void show_h(struct dlx_node *node) {
    node->R->L = node->L->R = node;
}

/**
 * Unlinks the given node vertically.
 * @param node[in] The node which is to be unlinked vertically.
 */
static inline void hide_v(struct dlx_node *node) {
    node->U->D = node->D, node->D->U = node->U;
}

/**
 * Restores the current node's vertical links.
 * @param node[in] The node whose links are to be restored vertically.
 */
static inline void show_v(struct dlx_node *node) {
    node->D->U = node->U->D = node;
}

struct dlx_heuristic {
    dlx_heuristic_callback callback;
    struct dlx_heuristic *next;
};

/* Stores various information related to the current problem. */
struct dlx_solver {
    dlx_solution_callback callback;
    dlx_data_callback before, after;

    struct dlx_heuristic *heuristic;

    int column_count, row_count;
    struct dlx_node *column;
};

/* The default stub callbacks. */
static void stub_solution_callback(struct dlx_context *s) { }
static void stub_data_callback(void *data, struct dlx_node *r) { }

struct dlx_solver *dlx_new(int column_count) {
    struct dlx_solver *dlx = malloc_s(sizeof(*dlx));
    dlx->callback = stub_solution_callback;
    dlx->before = dlx->after = stub_data_callback;
    dlx->heuristic = NULL;
    dlx->row_count = 0;
    dlx->column_count = column_count;

    struct dlx_node *next = calloc_s(1, sizeof(*next));
    struct dlx_node *prev = dlx->column = next->U = next->R = next->D = next->L = next;
    for(int i = 1; i < column_count; i++)
        show_h(prev = column_new(i, prev, next));
    return dlx;
}

void dlx_set_callback(struct dlx_solver *dlx, dlx_solution_callback callback) {
    dlx->callback = callback;
}

void dlx_set_ba(struct dlx_solver *dlx, dlx_data_callback before, dlx_data_callback after) {
    dlx->before = before, dlx->after = after;
}

void dlx_add_heuristic(struct dlx_solver *dlx, dlx_heuristic_callback callback) {
    struct dlx_heuristic *heuristic = calloc_s(1, sizeof(*heuristic));
    heuristic->callback = callback;
    heuristic->next = dlx->heuristic, dlx->heuristic = heuristic;
}

/**
 * Frees the rows of the given column.
 * @param column[in] The column whose rows are to be freed.
 */
static void dlx_column_free(struct dlx_node *column) {
    struct dlx_node *i = column->D;
    while(i != column) {
        struct dlx_node *tmp = i;
        i = i->D;
        free(tmp);
    }
}

void dlx_free(struct dlx_solver *dlx) {
    /* Free the matrix. */
    struct dlx_node *i = dlx->column;
    do {
        struct dlx_node *tmp = i;
        i = i->R;
        dlx_column_free(tmp);
        free(tmp);
    } while(i != dlx->column);

    /* Free the list of heuristics. */
    struct dlx_heuristic *j = dlx->heuristic;
    while(j) {
        struct dlx_heuristic *tmp = j;
        j = j->next;
        free(tmp);
    }
    free(dlx);
}

void dlx_row(struct dlx_solver *dlx, struct matrix_row *row) {
    struct dlx_node *column = dlx->column, *prev = NULL;
    for(int i = 0; i < dlx->column_count; i++) {
        if(row->row[i]) {
            prev = node_new(row, prev, column);
            show_v(prev);
            show_h(prev);
            column->size++;
        }
        column = column->R;
    }
    dlx->row_count++;
}

/**
 * @param start[in] The starting column.
 * @return The column with the least number of vertical nodes.
 */
static struct dlx_node *choose_min(struct dlx_node *start) {
    struct dlx_node *min = start;
    for(struct dlx_node *i = min->R; i != start; i = i->R)
        if(i->size < min->size)
            min = i;
    return min;
}

/**
 * Covers the column unlinking the row objects from the matrix.
 * @param dlx[in] For updating the column pointer if needed.
 * @param column[in] The column which is to be covered.
 */
static void cover_column(struct dlx_solver *dlx, struct dlx_node *column) {
    if(column == dlx->column)
        dlx->column = column->R == column ? NULL : column->R;
    hide_h(column);
    for(struct dlx_node *i = column->D; i != column; i = i->D)
        for(struct dlx_node *j = i->R; j != i; j = j->R)
            hide_v(j), j->C->size--;
}

/**
 * Uncovers the column object relinking the row objects.
 * @param dlx[in] For restoring the column pointer if needed.
 * @param column[in] The column which is to be uncovered.
 */
static void uncover_column(struct dlx_solver *dlx, struct dlx_node *column) {
    for(struct dlx_node *i = column->U; i != column; i = i->U)
        for(struct dlx_node *j = i->L; j != i; j = j->L)
            show_v(j), j->C->size++;
    show_h(column);
    dlx->column = column;
}

/**
 * Check if one of the heuristics thinks this branch should be terminated.
 *
 * @param dlx[in] The dlx solver instance which is to be used.
 * @param dlx_data[in] The data which is used by the heuristic.
 *
 * @return True iff a heuristic has decided to terminate this branch.
 */
static bool call_heuristics(struct dlx_solver *dlx, void *dlx_data) {
    for(struct dlx_heuristic *h = dlx->heuristic; h; h = h->next)
        if(h->callback(dlx_data))
            return true;
    return false;
}

/**
 * Searches the current "sub-tree" for solutions.
 *
 * @param dlx[in] The instance of the dlx algorithm.
 * @param context[in] The current context storing some important exposed information.
 */
static void search(struct dlx_solver *dlx, struct dlx_context *context) {
    if(!dlx->column) {
        dlx->callback(context);
        return;
    }

    struct dlx_solution *s = malloc_s(sizeof(*s)), *p = context->solution;
    struct dlx_node *column = choose_min(dlx->column);

    cover_column(dlx, column);
    for(struct dlx_node *r = column->D; r != column; r = r->D) {
        dlx->before(context->dlx_data, r);

        /* Construct and update the current solution. */
        s->row = r->row, s->next = p, context->solution = s;
        for(struct dlx_node *j = r->R; j != r; j = j->R)
            cover_column(dlx, j->C);

        if(!call_heuristics(dlx, context->dlx_data))
            search(dlx, context);

        for(struct dlx_node *j = r->L; j != r; j = j->L)
            uncover_column(dlx, j->C);

        dlx->after(context->dlx_data, r);
    }

    uncover_column(dlx, column);
    free(s);
}

void dlx_solve(struct dlx_solver *dlx, void *dlx_data) {
    struct dlx_context *context = calloc_s(1, sizeof(*context));
    context->dlx_data = dlx_data;

    search(dlx, context);
    free(context);
}
