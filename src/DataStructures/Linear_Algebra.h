#pragma once

//
//		Linear Algebra Core
//
// This header contains the basic struct for:
//		- Vector4
//		- Matrix4
//
// All the critical components are using vector instructions for performance improvement
//


#include <math.h>
//vector instruction set include 
#include <immintrin.h>


namespace CRAB {

#define CRAB_AVX_SUPPORT 1
#define M_PI 3.14156265f
#define SMALL_NUMBER 0.000001f


#ifndef WIN32
#define _align_ __declspec(align(16))
#else
#define _align_ __attribute__((aligned(16)))
#endif


#if CRAB_AVX_SUPPORT == 1
	__declspec(align(16))//memory aling to speed up acess
	struct Vector4Df
	{
		union {//union btw array, floats to use the names and _m128 to use vetorial instructions
			struct { float x, y, z, w; };
			float _v[4];
			__m128 v128;
		};

		Vector4Df() = default;
		Vector4Df(const float _x) :v128(_mm_set1_ps(_x)) {}
		Vector4Df(const float _x, const float _y, const float _z) :x(_x), y(_y), z(_z), w(0) {}
		Vector4Df(const float _x, const float _y, const float _z, const float _w) :x(_x), y(_y), z(_z), w(_w) {}
		Vector4Df(const __m128& v) :v128(v) {}


		//_mm_dp_ps is the vetorial instruction for dot product
		__forceinline float lengthsq() const { return _mm_cvtss_f32(_mm_dp_ps(v128, v128, 0x7f)); }
		__forceinline float length() const { return sqrtf(lengthsq()); }
		//inline void normalize() { float norm = length();  x /= norm; y /= norm; z /= norm; };
		inline void normalize() { v128 = _mm_div_ps(v128, _mm_sqrt_ps(_mm_dp_ps(v128, v128, 0xff))); };
		inline Vector4Df normalized() const { float norm = 1.0f / length(); return Vector4Df{ x * norm, y * norm, z * norm, w }; }
		inline void operator+=(const Vector4Df& v) { v128 = _mm_add_ps(v128, v.v128); }
		inline void operator-=(const Vector4Df& v) { v128 = _mm_sub_ps(v128, v.v128); }
		inline void operator*=(const Vector4Df& v) { v128 = _mm_mul_ps(v128, v.v128); }
		inline void operator/=(const Vector4Df& v) { v128 = _mm_div_ps(v128, v.v128); }
		inline void operator*=(const float& a) { v128 = _mm_mul_ps(v128, _mm_set_ps1(a)); }
		inline void operator/=(const float& a) { v128 = _mm_div_ps(v128, _mm_set_ps1(a)); }
		inline Vector4Df operator*(const float a) const { return _mm_mul_ps(v128, _mm_set_ps1(a)); }
		inline Vector4Df operator/(const float a) const { return _mm_div_ps(v128, _mm_set_ps1(a)); }
		inline Vector4Df operator+(const Vector4Df& v) const { return _mm_add_ps(v128, v.v128); }
		inline Vector4Df operator-(const Vector4Df& v) const { return _mm_sub_ps(v128, v.v128); }
		inline Vector4Df operator*(const Vector4Df& v) const { return _mm_mul_ps(v128, v.v128); }
		inline Vector4Df operator/(const Vector4Df& v) const { return _mm_div_ps(v128, v.v128); }
		inline bool operator!=(const Vector4Df& v) const { return x != v.x || y != v.y || z != v.z; }
		inline bool operator==(const Vector4Df& v) const { return fabsf(x - v.x) < SMALL_NUMBER && fabsf(y - v.y) < SMALL_NUMBER && fabsf(z - v.z) < SMALL_NUMBER; }

		//getters
		inline float& operator[](const unsigned i) { return _v[i]; }//note: this one get the value from stack memory
		inline float& operator()(const unsigned i) { return _v[i]; }
		//const getters
		inline float operator[](const unsigned i) const { return _v[i]; }
		inline float operator()(const unsigned i) const { return _v[i]; }
		inline operator __m128() const { return v128; }
		//note: this one get the value from register
		//ex: v.get<1>();
		//is AVX specific
		//template<int I> inline float get() const { return _mm_cvtss_f32(_mm_shuffle_ps(v128, v128, I)); }


		__forceinline Vector4Df cross(const __m128& v2) const {
			const __m128& v1_yzx = _mm_shuffle_ps(v128, v128, _MM_SHUFFLE(3, 0, 2, 1));
			const __m128& v2_yzx = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 0, 2, 1));
			const __m128& mul1 = _mm_mul_ps(v128, v2_yzx);
			const __m128& mul2 = _mm_mul_ps(v1_yzx, v2);
			const __m128& c = _mm_sub_ps(mul1, mul2);
			return _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 0, 2, 1));
		}
		__forceinline float dot(const __m128& v2) const {
			return _mm_cvtss_f32(_mm_dp_ps(v128, v2, 0x7f));
		}
		__forceinline float dot4(const __m128& v2) const {
			return _mm_cvtss_f32(_mm_dp_ps(v128, v2, 0xff));
		}

		inline Vector4Df sqrt() const {
			return _mm_sqrt_ps(v128);
		}
	};

	/*dot product using vetorial instructions and ignoring w*/
	__forceinline float dot(const __m128& v1, const __m128& v2) { return _mm_cvtss_f32(_mm_dp_ps(v1, v2, 0x7f)); }
	/*dot product using vetorial instructions and using w*/
	__forceinline float dot4(const __m128& v1, const __m128& v2) { return _mm_cvtss_f32(_mm_dp_ps(v1, v2, 0xff)); }
	/*dot product using vetorial instructions and ignoring w returning a Vector4Df*/
	__forceinline Vector4Df dot_Vec(const __m128& v1, const __m128& v2) { return _mm_dp_ps(v1, v2, 0x7f); }
	/*dot product using vetorial instructions and using w returning a Vector4Df*/
	__forceinline Vector4Df dot4_Vec(const __m128& v1, const __m128& v2) { return _mm_dp_ps(v1, v2, 0xff); }

	/*cross product*/
	__forceinline Vector4Df cross(const __m128& v1, const __m128& v2) {
		const __m128& v1_yzx = _mm_shuffle_ps(v1, v1, _MM_SHUFFLE(3, 0, 2, 1));
		const __m128& v2_yzx = _mm_shuffle_ps(v2, v2, _MM_SHUFFLE(3, 0, 2, 1));
		const __m128& mul1 = _mm_mul_ps(v1, v2_yzx);
		const __m128& mul2 = _mm_mul_ps(v1_yzx, v2);
		const __m128& c = _mm_sub_ps(mul1, mul2);
		return _mm_shuffle_ps(c, c, _MM_SHUFFLE(3, 0, 2, 1));
	}


	//reflection vector
	inline Vector4Df reflection(const Vector4Df& v, const Vector4Df& mirror) { return (mirror * dot(mirror, v)) * (2.0f) - v; }

	inline float distancesq(const Vector4Df& v1, const Vector4Df& v2) { return (v1 - v2).lengthsq(); }
	inline float distance(const Vector4Df& v1, const Vector4Df& v2) { return sqrtf(distancesq(v1, v2)); }


	inline Vector4Df min(const Vector4Df& v1, const Vector4Df& v2) {
		return *(Vector4Df*)&_mm_min_ps(v1, v2);
	}

	inline Vector4Df max(const Vector4Df& v1, const Vector4Df& v2) {
		return *(Vector4Df*)&_mm_max_ps(v1, v2);
	}



#define MakeShuffleMask(x,y,z,w)           (x | (y<<2) | (z<<4) | (w<<6))

	// vec(0, 1, 2, 3) -> (vec[x], vec[y], vec[z], vec[w])
#define VecSwizzleMask(vec, mask)          _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(vec), mask))
#define VecSwizzle(vec, x, y, z, w)        VecSwizzleMask(vec, MakeShuffleMask(x,y,z,w))
#define VecSwizzle1(vec, x)                VecSwizzleMask(vec, MakeShuffleMask(x,x,x,x))
// special swizzle
#define VecSwizzle_0022(vec)               _mm_moveldup_ps(vec)
#define VecSwizzle_1133(vec)               _mm_movehdup_ps(vec)

// return (vec1[x], vec1[y], vec2[z], vec2[w])
#define VecShuffle(vec1, vec2, x,y,z,w)    _mm_shuffle_ps(vec1, vec2, MakeShuffleMask(x,y,z,w))
// special shuffle
#define VecShuffle_0101(vec1, vec2)        _mm_movelh_ps(vec1, vec2)
#define VecShuffle_2323(vec1, vec2)        _mm_movehl_ps(vec2, vec1)

	__declspec(align(32))
		struct Matrix4 {
		//using row-major order 
		union
		{
			float m[16];
			float _m[4][4];
			__m128 row[4];
		};

		__forceinline Vector4Df operator *(const Vector4Df& v) const {
			return Vector4Df{
				dot4(v, row[0]),
				dot4(v, row[1]),
				dot4(v, row[2]),
				dot4(v, row[3]) };
		}

		inline Matrix4 Inverse() const {
			Matrix4 r;

			// transpose 3x3, we know m03 = m13 = m23 = 0
			__m128 t0 = VecShuffle_0101(row[0], row[1]); // 00, 01, 10, 11
			__m128 t1 = VecShuffle_2323(row[0], row[1]); // 02, 03, 12, 13
			r.row[0] = VecShuffle(t0, row[2], 0, 2, 0, 3); // 00, 10, 20, 23(=0)
			r.row[1] = VecShuffle(t0, row[2], 1, 3, 1, 3); // 01, 11, 21, 23(=0)
			r.row[2] = VecShuffle(t1, row[2], 0, 2, 2, 3); // 02, 12, 22, 23(=0)

			// last line
			r.row[3] = _mm_mul_ps(r.row[0], VecSwizzle1(row[3], 0));
			r.row[3] = _mm_add_ps(r.row[3], _mm_mul_ps(r.row[1], VecSwizzle1(row[3], 1)));
			r.row[3] = _mm_add_ps(r.row[3], _mm_mul_ps(r.row[2], VecSwizzle1(row[3], 2)));
			r.row[3] = _mm_sub_ps(_mm_setr_ps(0.f, 0.f, 0.f, 1.f), r.row[3]);

			return r;
		}

		inline Matrix4 Transpose() const {
			Matrix4 mt = *this;
			_MM_TRANSPOSE4_PS(mt.row[0], mt.row[1], mt.row[2], mt.row[3]);
			return mt;
		}

		//matrix multiplication using vetorial instructions
		inline Matrix4 operator *(const Matrix4& m1) const {
			Matrix4 mr;//answer matrix 
			//join the rows in two __m256 registers to make the calculations faster
			const register __m256 row_1_2 = _mm256_set_m128(m1.row[0], m1.row[1]), row_3_4 = _mm256_set_m128(m1.row[2], m1.row[3]);
			//_mm_broadcast_ss load one single float to the 4 floats of a __m128
			//_mm256_set_m128 joins 2 __m128 into a __m256
			//_mm256_add_ps adds float by float the floats in both __m256 
			//_mm256_mul_ps multiply float by float the floats in both __m256 
			const __m256 a{ _mm256_add_ps(
				_mm256_mul_ps(_mm256_set_m128(_mm_broadcast_ss(&m[0]), _mm_broadcast_ss(&m[1])), row_1_2),
				_mm256_mul_ps(_mm256_set_m128(_mm_broadcast_ss(&m[2]), _mm_broadcast_ss(&m[3])), row_3_4)
			) };
			const __m256 b{ _mm256_add_ps(
				_mm256_mul_ps(_mm256_set_m128(_mm_broadcast_ss(&m[4]), _mm_broadcast_ss(&m[5])), row_1_2),
				_mm256_mul_ps(_mm256_set_m128(_mm_broadcast_ss(&m[6]), _mm_broadcast_ss(&m[7])), row_3_4)
			) };


			_mm256_store_ps((float*)&mr.m,
				_mm256_add_ps(//permutes a and b, after this we add then to discover the two firt rows of mr
					_mm256_permute2f128_ps(b, a, 0x02),
					_mm256_permute2f128_ps(b, a, 0x13)
				));

			//same of before to rows 3 and 4
			const __m256 a2{ _mm256_add_ps(
				_mm256_mul_ps(_mm256_set_m128(_mm_broadcast_ss(&m[8]), _mm_broadcast_ss(&m[9])), row_1_2),
				_mm256_mul_ps(_mm256_set_m128(_mm_broadcast_ss(&m[10]), _mm_broadcast_ss(&m[11])), row_3_4)
			) };
			const __m256 b2{ _mm256_add_ps(
				_mm256_mul_ps(_mm256_set_m128(_mm_broadcast_ss(&m[12]), _mm_broadcast_ss(&m[13])), row_1_2),
				_mm256_mul_ps(_mm256_set_m128(_mm_broadcast_ss(&m[14]), _mm_broadcast_ss(&m[15])), row_3_4)
			) };

			_mm256_store_ps((float*)&mr.row[2],
				_mm256_add_ps(
					_mm256_permute2f128_ps(b2, a2, 0x02),
					_mm256_permute2f128_ps(b2, a2, 0x13)
				));

			return mr;
		}
	
		inline bool operator==(const Matrix4& m1) const {
			for (int i = 0; i < 12; i++) {
				if (m[i] != m1.m[i]) { return false; }
			}
			return true;
		}

	
	};
#endif

#if CRAB_AVX_SUPPORT == 0
	struct Vector4Df
	{
		union {//union btw array, floats to use the names=
			struct { float x, y, z, w; };
			float _v[4];
		};

		Vector4Df() = default;
		Vector4Df(const float _x) :x(_x), y(_x), z(_x), w(_x) {}
		Vector4Df(const float _x, const float _y, const float _z) :x(_x), y(_y), z(_z), w(0) {}
		Vector4Df(const float _x, const float _y, const float _z, const float _w) :x(_x), y(_y), z(_z), w(_w) {}


		__forceinline Vector4Df cross(const Vector4Df& v2) const {
			return Vector4Df{ y * v2.z - v2.y * z,
				z * v2.x - v2.z * x,
				x * v2.y - v2.x * y };
		}
		__forceinline float dot(const Vector4Df& v) const {
			return x * v.x + y * v.y + z * v.z;
		}
		__forceinline float dot4(const Vector4Df& v) const {
			return x * v.x + y * v.y + z * v.z + w * v.w;
		}

		__forceinline float lengthsq() const { return x * x + y * y + z * z; }
		__forceinline float length() const { return sqrtf(lengthsq()); }
		inline void normalize() { float norm = 1.0f / length(); x *= norm; y *= norm; z *= norm; };
		inline Vector4Df normalized() const { float norm = 1.0f / length(); return Vector4Df{ x * norm, y * norm, z * norm, w }; }
		inline void operator+=(const Vector4Df& v) { x += v.x; y += v.y; z += v.z; }
		inline void operator-=(const Vector4Df& v) { x -= v.x; y -= v.y; z -= v.z; }
		inline void operator*=(const Vector4Df& v) { x *= v.x; y *= v.y; z *= v.z; }
		inline void operator*=(const float& a) { x *= a; y *= a; z *= a; }
		inline void operator/=(const float& a) { x /= a; y /= a; z /= a; }
		inline Vector4Df operator*(float a) const { return Vector4Df{ x * a, y * a, z * a, w }; }
		inline Vector4Df operator/(const float a) const { return Vector4Df{ x / a, y / a, z / a, w }; }
		inline Vector4Df operator*(const Vector4Df& v) const { return Vector4Df{ x * v.x, y * v.y, z * v.z, w * v.w }; }
		inline Vector4Df operator/(const Vector4Df& v) const { return Vector4Df{ x / v.x, y / v.y, z / v.z, w / v.w }; }
		inline Vector4Df operator+(const Vector4Df& v) const { return Vector4Df{ x + v.x, y + v.y, z + v.z, w + v.w }; }
		inline Vector4Df operator-(const Vector4Df& v) const { return Vector4Df{ x - v.x, y - v.y, z - v.z, w - v.w }; }
		inline bool operator!=(const Vector4Df& v) const { return x != v.x || y != v.y || z != v.z; }
		inline bool operator==(const Vector4Df& v) const { return fabsf(x - v.x) < SMALL_NUMBER && fabsf(y - v.y) < SMALL_NUMBER && fabsf(z - v.z) < SMALL_NUMBER; }

		//getters
		inline float& operator[](const int i) { return _v[i]; }//note: this one get the value from stack memory
		inline float& operator()(const int i) { return _v[i]; }
		//const getters
		inline float operator[](const int i) const { return _v[i]; }//note: this one get the value from register
		inline float operator()(const int i) const { return _v[i]; }

		inline Vector4Df sqrt() const {
			return Vector4Df{ sqrtf(x),sqrtf(y) ,sqrtf(z) ,sqrtf(w) };
		}
	};


	//reflection vector
	inline Vector4Df reflection(const Vector4Df& v, const Vector4Df& mirror) { return (mirror * mirror.dot(v)) * (2.0f) - v; }

	inline float distancesq(const Vector4Df& v1, const Vector4Df& v2) { return (v1 - v2).lengthsq(); }
	inline float distance(const Vector4Df& v1, const Vector4Df& v2) { return sqrtf(distancesq(v1, v2)); }


	inline Vector4Df min(const Vector4Df& v1, const Vector4Df& v2) {
		return Vector4Df{ v1.x < v2.x ? v1.x : v2.x, v1.y < v2.y ? v1.y : v2.y, v1.z < v2.z ? v1.z : v2.z };
	}

	inline Vector4Df max(const Vector4Df& v1, const Vector4Df& v2) {
		return Vector4Df{ v1.x > v2.x ? v1.x : v2.x, v1.y > v2.y ? v1.y : v2.y, v1.z > v2.z ? v1.z : v2.z };
	}

	__declspec(align(32))
		struct Matrix4 {
		//using row-major order 
		union
		{
			float m[16];
			float _m[4][4];
			Vector4Df row[4];
		};

		__forceinline Vector4Df operator *(const Vector4Df& v) const {
			return Vector4Df{
				v.dot(row[0]),
				v.dot(row[1]),
				v.dot(row[2]),
				v.dot(row[3]) };
		}

		inline Matrix4 Transpose() const {
			Matrix4 mt;
			for (unsigned i = 0; i < 4; i++) {
				for (unsigned j = 0; j < 4; j++) {
					mt._m[j][i] = _m[i][j];
				}
			}
			return mt;
		}

		//matrix multiplication using vetorial instructions
		inline Matrix4 operator *(const Matrix4& m1) const {
			Matrix4 mr;//answer matrix
			for (unsigned i = 0; i < 4; i++) {
				for (unsigned j = 0; j < 4; j++) {
					mr._m[i][j] = 0;
					for (unsigned k = 0; k < 4; k++) {
						mr._m[i][j] += _m[i][k] * m1._m[k][j];
					}
				}
			}
			return mr;
		}
	};
#endif

	/*
	----------------- Create the transformation matrices----------------------
	*/
	inline
		Matrix4 Identity()
	{
		return Matrix4{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1 };
	}

	inline
		Matrix4 translate(const Vector4Df& v)
	{
		return Matrix4{
			1, 0, 0, v.x,
			0, 1, 0, v.y,
			0, 0, 1, v.z,
			0, 0, 0, 1 };
	}

	inline
		Matrix4 resizeMatrix(float x, float y, float z)
	{
		return Matrix4{
		x,	0,	0,	0,
		0,	y,	0,	0,
		0,	0,	z,	0,
		0,	0,	0,	1 };

	}

	inline
		Matrix4 rotateX(float ang)
	{
		ang = (ang * M_PI) / 180.0f;

		return Matrix4{
		1,	0,	0,	0,
		0,	cosf(ang),	-sinf(ang),	0,
		0,	sinf(ang),	cosf(ang),	0,
		0,	0,	0,	1 };
	}

	inline
		Matrix4 rotateY(float ang)
	{
		ang = (ang * M_PI) / 180.0f;

		return Matrix4{
		cosf(ang),	0,	sinf(ang),	0,
		0,	1,	0,	0,
		-sinf(ang),	0,	cosf(ang),	0,
		0,	0,	0,	1 };
	}

	inline
		Matrix4 rotateZ(float ang)
	{
		ang = (ang * M_PI) / 180.0f;

		return Matrix4{
		cosf(ang),	-sinf(ang),	0,	0,
		sinf(ang),	cosf(ang),	0,	0,
		0,	0,	1,	0,
		0,	0,	0,	1 };

	}

	inline
		Matrix4 rotateArbitrary(float ang, const Vector4Df& _u)
	{
		const Vector4Df u = _u.normalized();

		ang = (ang * M_PI) / 180.0f;
		return Matrix4{
			cosf(ang) + u.x * u.x * (1 - cosf(ang)), u.y * u.x * (1 - cosf(ang)) - u.z * sinf(ang),
				   u.z * u.x * (1 - cosf(ang)) + u.y * sinf(ang),	0,

			u.y * u.x * (1 - cosf(ang)) + u.z * sinf(ang), cosf(ang) + u.y * u.y * (1 - cosf(ang)),
				   u.z * u.y * (1 - cosf(ang)) - u.x * sinf(ang),	0,

			u.z * u.x * (1 - cosf(ang)) - u.y * sinf(ang), u.z * u.y * (1 - cosf(ang)) + u.x * sinf(ang),
				   cosf(ang) + u.z * u.z * (1 - cosf(ang)),	0,

			0,0,0,1
		};
	}

}