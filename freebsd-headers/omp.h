#pragma once
/* Copyright (C) 2005 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU OpenMP Library (libgomp).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
   more details.

   You should have received a copy of the GNU Lesser General Public License 
   along with libgomp; see the file COPYING.LIB.  If not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* As a special exception, if you link this library with other files, some
   of which are compiled with GCC, to produce an executable, this library
   does not by itself cause the resulting executable to be covered by the
   GNU General Public License.  This exception does not however invalidate
   any other reasons why the executable file might be covered by the GNU
   General Public License.  */

#ifndef OMP_H
#define OMP_H 1

#ifndef _LIBGOMP_OMP_LOCK_DEFINED
#define _LIBGOMP_OMP_LOCK_DEFINED 1
/* These two structures get edited by the libgomp build process to 
   reflect the shape of the two types.  Their internals are private
   to the library.  */

typedef struct
{
  unsigned char _x[8] 
    __attribute__((__aligned__(8)));
} omp_lock_t;

typedef struct
{
  unsigned char _x[16] 
    __attribute__((__aligned__(8)));
} omp_nest_lock_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern void omp_set_num_threads (int);
extern int omp_get_num_threads (void);
extern int omp_get_max_threads (void);
extern int omp_get_thread_num (void);
extern int omp_get_num_procs (void);

extern int omp_in_parallel (void);

extern void omp_set_dynamic (int);
extern int omp_get_dynamic (void);

extern void omp_set_nested (int);
extern int omp_get_nested (void);

extern void omp_init_lock (omp_lock_t *);
extern void omp_destroy_lock (omp_lock_t *);
extern void omp_set_lock (omp_lock_t *);
extern void omp_unset_lock (omp_lock_t *);
extern int omp_test_lock (omp_lock_t *);

extern void omp_init_nest_lock (omp_nest_lock_t *);
extern void omp_destroy_nest_lock (omp_nest_lock_t *);
extern void omp_set_nest_lock (omp_nest_lock_t *);
extern void omp_unset_nest_lock (omp_nest_lock_t *);
extern int omp_test_nest_lock (omp_nest_lock_t *);

extern double omp_get_wtime (void);
extern double omp_get_wtick (void);

#ifdef __cplusplus
}
#endif

#endif /* OMP_H */
