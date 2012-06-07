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

/**
    Physics
    */
#define MACH_SPEED 300

inline int int_abs(int);
inline double double_abs(double);
inline double double_min(double, double);
inline double double_max(double, double);
inline short int short_min(short int, short int);
inline short int short_max(short int, short int);
int rzad(int);
inline int sign(double);
double norm(double fi);
double vector_angle(double, double);

double square_equation(double r0, double fi);
double rectangle_equation(double r0, double fi, double fi0, double fi02, double wsp1, double wsp2);

void get_line_from_points(double x1, double y1, double x2, double y2, line *);
void get_line_from_point_and_vector(double x, double y, double vx, double vy, line *);
inline double point_distance_from_line(double x0, double y0, line *L);
void common_point(const line* L1, const line* L2, double *x, double *y);

inline double vector_product(double x1, double y1, double x2, double y2);
bool vectors_on_two_sides(double vector_pr1, double vector_pr2);
bool do_segments_intersect(const point *A1, const point *A2,
                           const point *B1, const point *B2);
bool get_segment_intersection(const point *A1, const point *A2,
                              const point *B1, const point *B2,
                              point *I);

double coefficient_multiplier(double v);

#endif
