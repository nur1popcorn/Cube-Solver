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

#ifndef CUBE_H
#define CUBE_H

#include <stdint.h>

#include "dlx.h"

/* This enum maps each of the graph neighbours to a direction. */
enum direction { NOP = 0, UP, RIGHT, DOWN, LEFT };

/**
 * @param dir The direction which is to be rotated.
 * @param rot The number of right rotations to perform.
 * @return The rot times rotated direction.
 */
static inline enum direction rotate(enum direction dir, int rot) {
    return dir == NOP ? NOP : ((dir - 1 + rot) % 4) + 1;
}

/**
 * @param dir The direction which is to be flipped.
 * @return The flipped direction.
 */
static inline enum direction flip(enum direction dir) {
    return rotate(dir, (dir == RIGHT || dir == LEFT) ? 2 : 0);
}

/* Encodes each pentomino as a set of graph walks. */
enum direction GENERATOR_PENTOMINOS[12][4][4];
/* Encodes the cube as a graph where each node has 4 neighbours. */
int NEIGHBOUR_MATRIX[60][4];
/* Applies the required rotation at each step to perform the walk. */
int ROTATION_MATRIX[60][4];
/* Maps each tile to a cube-face and it's area for that cube face. */
double AREA_MATRIX[60][6];

struct row_data {
    double weight;
    uint64_t flags;
};

/**
 * Generates the rows needed for the exact cover formulation.
 * @return The matrix rows of the exact cover formulation.
 */
struct matrix_row *generate_walks();

#endif /* CUBE_H */
