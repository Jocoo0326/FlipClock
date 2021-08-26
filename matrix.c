#include "matrix.h"
#include <assert.h>

void matrix_set_identity(float* sm, int smoffset)
{
  for (int i = 0; i < 16; ++i) {
    sm[smoffset + i] = 0.0;
  }

  for (int i = 0; i < 16; i += 5) {
    sm[smoffset + i] = 1.0;
  }
}

void matrix_translate_m(float* tm, int tmoffset, float* sm, int smoffset,
                      float x, float y, float z)
{
  for (int i = 0; i < 12; ++i) {
    tm[tmoffset + i] = sm[smoffset + i];
  }

  for (int i = 0; i < 4; ++i) {
    int tmi = tmoffset + i;
    int smi = smoffset + i;
    tm[12 + tmi] =
        sm[smi] * x + sm[4 + smi] * y + sm[8 + smi] * z + sm[12 + smi];
  }
}

void matrix_translate(float *tm, int tmoffset, float x, float y, float z)
{
	for (int i = 0; i < 4; ++i) {
    int tmi = tmoffset + i;
    tm[12 + tmi] =
        tm[tmi] * x + tm[4 + tmi] * y + tm[8 + tmi] * z + tm[12 + tmi];
  }
}

void matrix_scale(float *sm, int smoffset, float x, float y, float z)
{
	for (int i = 0; i < 4; ++i) {
		int smi = smoffset + i;
    sm[smi] *= x;
    sm[smi + 4] *= y;
    sm[smi + 8] *= z;
	}
}

void matrix_multiply_m(float *rm, int rmoffset, float *lhs, int lhsoffset, float *rhs, int rhsoffset)
{
	for (int r = 0; r < 4; ++r) {
		for (int c = 0; c < 4; ++c) {
			rm[rmoffset + r + c * 4] =
				lhs[lhsoffset + r     ] * rhs[rhsoffset + c * 4    ] +
				lhs[lhsoffset + r +  4] * rhs[rhsoffset + c * 4 + 1] +
				lhs[lhsoffset + r +  8] * rhs[rhsoffset + c * 4 + 2] +
				lhs[lhsoffset + r + 12] * rhs[rhsoffset + c * 4 + 3];
		}
	}
}

void matrix_ortho_m(float *rm, int rmoffset, float left, float right,
										float bottom, float top, float near, float far)
{
	assert(left != right);
	assert(bottom != top);
	assert(near != far);
  for (int i = 0; i < 16; ++i) {
    rm[rmoffset + i] = 0.0;
  }
	const float r_width = 1.0 / (right - left);
	const float r_height = 1.0 / (top - bottom);
	const float r_depth = 1.0 / (far - near);
	const float x = 2.0 * r_width;
	const float y = 2.0 * r_height;
	const float z = -2.0 * r_depth;
	const float tx = -(right + left) * r_width;
	const float ty = -(top + bottom) * r_height;
	const float tz = -(far + near) * r_depth;
	rm[rmoffset + 0] = x;
	rm[rmoffset + 5] = y;
	rm[rmoffset + 10] = z;
	rm[rmoffset + 12] = tx;
	rm[rmoffset + 13] = ty;
	rm[rmoffset + 14] = tz;
	rm[rmoffset + 15] = 1.0;
}
