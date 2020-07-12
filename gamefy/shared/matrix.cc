#include "matrix.h"

Matrix4x4 Matrix4x4::operator*(const Matrix4x4 &rhs) {
	Matrix4x4 r;
	r._11 = _11*rhs._11 + _12*rhs._21 + _13*rhs._31 + _14*rhs._41;
	r._12 = _11*rhs._12 + _12*rhs._22 + _13*rhs._32 + _14*rhs._42;
	r._13 = _11*rhs._13 + _12*rhs._23 + _13*rhs._33 + _14*rhs._43;
	r._14 = _11*rhs._14 + _12*rhs._24 + _13*rhs._34 + _14*rhs._44;

	r._21 = _21*rhs._11 + _22*rhs._21 + _23*rhs._31 + _24*rhs._41;
	r._22 = _21*rhs._12 + _22*rhs._22 + _23*rhs._32 + _24*rhs._42;
	r._23 = _21*rhs._13 + _22*rhs._23 + _23*rhs._33 + _24*rhs._43;
	r._24 = _21*rhs._14 + _22*rhs._24 + _23*rhs._34 + _24*rhs._44;

	r._31 = _31*rhs._11 + _32*rhs._21 + _33*rhs._31 + _34*rhs._41;
	r._32 = _31*rhs._12 + _32*rhs._22 + _33*rhs._32 + _34*rhs._42;
	r._33 = _31*rhs._13 + _32*rhs._23 + _33*rhs._33 + _34*rhs._43;
	r._34 = _31*rhs._14 + _32*rhs._24 + _33*rhs._34 + _34*rhs._44;

	r._41 = _41*rhs._11 + _42*rhs._21 + _43*rhs._31 + _44*rhs._41;
	r._42 = _41*rhs._12 + _42*rhs._22 + _43*rhs._32 + _44*rhs._42;
	r._43 = _41*rhs._13 + _42*rhs._23 + _43*rhs._33 + _44*rhs._43;
	r._44 = _41*rhs._14 + _42*rhs._24 + _43*rhs._34 + _44*rhs._44;
	
	return r;
} // end function operator*

// Default constructor loads identity matrix
Matrix4x4::Matrix4x4(): _11(1), _12(0), _13(0), _14(0),
			_21(0), _22(1), _23(0), _24(0),
			_31(0), _32(0), _33(1), _34(0),
			_41(0), _42(0), _43(0), _44(1) {} // end constructor Matrix4x4()

Vector3 operator*(const Vector3 &v, const Matrix4x4 &m) {
	Vector3 r;
	r.x = v.x*m._11 + v.y*m._21 + v.z*m._31 + 1.0*m._41;
	r.y = v.x*m._12 + v.y*m._22 + v.z*m._32 + 1.0*m._42;
	r.z = v.x*m._13 + v.y*m._23 + v.z*m._33 + 1.0*m._43;
	return r;
} // end function operator*
