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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdlib.h>

#ifdef __GNUC__
    #define likely(x)   __builtin_expect((x), 1)
    #define unlikely(x) __builtin_expect((x), 0)
#else
    #define likely(x)   (x)
    #define unlikely(x) (x)
#endif

/**
 * @param size The number of bytes which are to be allocated.
 * @return A region of memory with the given number of bytes.
 */
static inline void *malloc_s(size_t size) {
    void *object = malloc(size);
    if(unlikely(!object)) exit(1);
    return object;
}

/**
 * @param nmemb The number of members which are to be allocated.
 * @param size The size of each member.
 *
 * @return A region of memory with the given number of bytes which is filled with 0s.
 */
static inline void *calloc_s(size_t nmemb, size_t size) {
    void *object = calloc(nmemb, size);
    if(unlikely(!object)) exit(1);
    return object;
}

#endif /* GLOBALS_H */
