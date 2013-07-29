#ifndef _INCLUDE_MATHEMATICS_H
#define _INCLUDE_MATHEMATICS_H

#include "structures.h"

/**
    Maths
    */
#define dwaPI 6.28318531
#define PI74 5.49778714
#define PI32 4.71238898
#define PI54 3.92699081
#define PI 3.14159265
#define PI34 2.35619449
#define PIpol 1.57079633
#define PI4 0.78539816

#define SQRT2 1.41421356

#define eps 0.000001
#define double_eps 0.0000000001

/**
    Physics
    */
#define MACH_SPEED 300

inline int int_abs(int);
inline long double double_abs(long double);
inline long double double_min(long double, long double);
inline long double double_max(long double, long double);
inline short int short_min(short int, short int);
inline short int short_max(short int, short int);
inline int int_max(int, int);
int rzad(int);
inline int sign(long double);
inline short short_sign(short);
long double norm(long double fi);
long double vector_angle(long double, long double);

long double square_equation(long double r0, long double fi);
long double rectangle_equation(long double r0, long double fi, long double fi0, long double fi02, long double wsp1, long double wsp2);

void get_line_from_points(long double x1, long double y1, long double x2, long double y2, line *);
void get_line_from_point_and_vector(long double x, long double y, long double vx, long double vy, line *);
inline long double point_distance_from_line(long double x0, long double y0, line *L);
void common_point(const line* L1, const line* L2, long double *x, long double *y);

inline long double vector_product(long double x1, long double y1, long double x2, long double y2);
bool vectors_on_two_sides(long double vector_pr1, long double vector_pr2);
bool do_segments_intersect(const point *A1, const point *A2,
                           const point *B1, const point *B2);
bool get_segment_intersection(const point *A1, const point *A2,
                              const point *B1, const point *B2,
                              point *I);

long double coefficient_multiplier(long double v);

#endif
