#ifndef __MATRIX_H_
#define __MATRIX_H_

#include <stdio.h>

#include "vector.h"

class Matrix4x4 {
	public:
	float	_11, _12, _13, _14,
		_21, _22, _23, _24,
		_31, _32, _33, _34,
		_41, _42, _43, _44;

	Matrix4x4();
	Matrix4x4 operator*(const Matrix4x4 &);
};  

Vector3 operator*(const Vector3 &, const Matrix4x4 &);

#endif
