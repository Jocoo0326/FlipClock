#ifndef MATRIX_H
#define MATRIX_H

void matrix_set_identity(float *sm, int smoffset);

void matrix_translate(float *tm, int tmoffset, float x, float y, float z);

void matrix_translate_m(float *tm, int tmoffset, float *sm, int smoffset, float x, float y, float z);

void matrix_scale(float *sm, int smoffset, float x, float y, float z);

void matrix_multiply_m(float *rm, int rmoffset, float *lhs, int lhsoffset, float *rhs, int rhsoffset);

void matrix_ortho_m(float *rm, int rmoffset, float left, float right, float bottom, float top, float near, float far);
#endif /* MATRIX_H */
