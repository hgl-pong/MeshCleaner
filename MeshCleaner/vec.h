#ifndef VEC_H
#define VEC_H
#include <vector>
#include <cmath>
#include <limits>
#include <string>

typedef float FFLOAT;
#define FLOAT_EPSILON /*std::numeric_limits<FFLOAT>::epsilon()*/ 1e-6f
#define FLOAT_MAX std::numeric_limits<FFLOAT>::max()
#define FLOAT_MIN std::numeric_limits<FFLOAT>::lowest()

#define SCALE 1000000
#define ROUND 5e-7f

#include <assert.h>

namespace FFloat
{

	inline bool isZero(FFLOAT number)
	{
		return std::abs(number) <= FLOAT_EPSILON;
	}

	inline bool isEqual(FFLOAT a, FFLOAT b)
	{
		return isZero(a - b);
	}

	inline bool isWeakZero(FFLOAT number)
	{
		return std::abs(number) <= 1e-5f;
	}

	inline bool isWeakEqual(FFLOAT a, FFLOAT b)
	{
		return isWeakZero(a - b);
	}
	
} //namespace FFLOAT
class FVec2
{
public:
	FFLOAT X, Y;
public:
	FVec2() { X = 0; Y = 0; }
	FVec2(FFLOAT x, FFLOAT y) :X(x), Y(y) {}
	FVec2& operator = (const FVec2& v) {
		X = v.X; Y = v.Y;
		return *this;
	}

	bool operator==(const FVec2& v)const {
		return FFloat::isEqual(v.X, X) && FFloat::isEqual(v.Y, Y);
	}


	bool operator<(const FVec2& v)const {
		if (X >= v.X)
			return false;
		if (Y >= v.Y)
			return false;
		return false;
	}


	FFLOAT Dot(const FVec2& v) {
		return X * v.X + Y * v.Y;
	}

	FVec2 operator - (const FVec2& v) const {
		return FVec2(X - v.X, Y - v.Y);
	}

	FVec2 operator + (const FVec2& v) const {
		return FVec2(X + v.X, Y + v.Y);
	}

	FVec2 operator * (const float& v) const {
		return FVec2(X * v, Y * v);
	}

	FVec2 operator / (const FFLOAT& d) const{
		return FVec2(X / d, Y / d);
	}

	FVec2 operator += (const FVec2& v) {
		X += v.X;
		Y += v.Y;
		return *this;
	}

	bool IsInPolygon(const std::vector<FVec2>& polygon);

	bool IsInPolygon(const std::vector<FVec2>& polygonVertices, const std::vector<uint32_t>& polygonIndices);

	bool IsInTriangle(FVec2& a, FVec2& b, FVec2& c);

	bool IsOnLine(FVec2& a, FVec2& b);

	FFLOAT Length() {
		return sqrt(LengthSqr());
	}

	FFLOAT LengthSqr() {
		return Dot(*this);
	}

	FVec2& Normalize() {
		FFLOAT l = 1 / Length();
		X *= l;
		Y *= l;
		return *this;
	}

};

struct FVec4
{
	FFLOAT X, Y, Z, W;
};
class FVec3
{
public:

	FFLOAT X, Y, Z;

public:
	FVec3() { X = 0; Y = 0; Z = 0; };
	FVec3(FFLOAT x, FFLOAT y, FFLOAT z);
	FVec3(const FVec3&);
	~FVec3() = default;

	FVec3& operator	= (const FVec3& v);
	FVec3& operator += (const FVec3& v);
	FVec3& operator -= (const FVec3& v);
	FVec3 operator *= (const FFLOAT& d);
	FVec3& operator /= (const FFLOAT d);


	FFLOAT Length() const;
	FFLOAT LengthSqr() const;
	FVec3& Normalize();
	FVec3 Cross(const FVec3& v) const;

	FVec3 operator - (const FVec3& v)const;
	FVec3 operator + (const FVec3& v)const;
	FVec3 operator * (const FFLOAT d);
	FVec3 operator * (const FFLOAT d)const;
	FVec3 operator * (const FVec3& v);
	FVec3 operator / (const FFLOAT d)const;
	FVec3 operator / (const FVec3& d);
	FVec3 operator ^ (const FVec3& v);
	int operator == (const FVec3& v)const;
	FVec3 Prod(const FVec3& v);
	FFLOAT Dot(const FVec3& v)const;
	FFLOAT Distance(const FVec3& v)const;
	FFLOAT DistanceSqr(const FVec3& v)const;

	FVec3 Ceiling(const FVec3& v)const;
	FVec3 Floor(const FVec3& v)const;

	bool IsOnSegment(FVec3& a, FVec3& b);
	bool IsOnLine(const FVec3& a, const FVec3& b)const;

	bool IsZero() {
		return X == 0 && Y == 0 && Z == 0;
	}

	bool operator<(const FVec3& v)const {
		if (X >= v.X)
			return false;
		if (Y >= v.Y)
			return false;
		if (Z >= v.Z)
			return false;
		return false;
	}

	FFLOAT operator[](const int i)const {
		if (i == 0)
			return X;
		else if (i == 1)
			return Y;
		else if (i == 2)
			return Z;
		else
			throw("FVec3 out of range!");
	}
};

static FVec3 Normal(const FVec3& a, const FVec3& b, const FVec3& c)
{
	FVec3 ba = b - a;
	FVec3 ca = c - a;
	FVec3 normal = ba.Cross(ca);

	FFLOAT length = normal.Length();
	if (FFloat::isZero(length))
		return FVec3();

	return FVec3(normal.X / length, normal.Y / length, normal.Z / length);
}

static FFLOAT Area(const FVec3& a, const FVec3& b, const FVec3& c)
{
	FVec3 ba = b - a;
	FVec3 ca = c - a;
	FVec3 normal = ba.Cross(ca);

	FFLOAT length = normal.Length();
	//if (FFloat::isZero(length))
	//	return 0;
	return length;
}

static bool IntersectSegmentAndPlane(const FVec3& segmentPoint0, const  FVec3& segmentPoint1,
	const FVec3& pointOnPlane, FVec3& planeNormal,
	FVec3& intersection)
{
	FVec3 u = segmentPoint1 - segmentPoint0;
	FVec3 w = pointOnPlane - segmentPoint0;
	FFLOAT d = planeNormal.Dot(u);
	FFLOAT n = planeNormal.Dot(w);
	//

	if (FFloat::isZero(d))
		return false;
	auto s = n / d;
	if (s < 0 || s> 1 || std::isnan(s) || std::isinf(s))
		return false;
	intersection = segmentPoint0 + u * s;
	return true;

}

static bool IsInTriangle(const FVec3& intersection, const FVec3* trianglePositions) {
	//点在三角形内
	std::vector<FFLOAT> areas(3);
	FFLOAT s = 1/Area(trianglePositions[0], trianglePositions[1], trianglePositions[2]);
	for (size_t i = 0; i < 3; ++i) {
		size_t j = (i + 1) % 3;
		FFLOAT area = Area(intersection, trianglePositions[i], trianglePositions[j])*s;
		areas[i] = area;
	}
	//在三角形内或三角形边上时
	FFLOAT d = (areas[0] + areas[1] + areas[2] - 1);
	return d< FLOAT_EPSILON/*FFloat::isZero(d)*/;

}

static FFLOAT DistanceSqrToSegement(const FVec3& v1, const FVec3& v2, const FVec3& p) {
	FVec3 test = p - v2;
	FVec3 line = (v1 - v2);
	FFLOAT d1 = line.Length();
	line.Normalize();
	FFLOAT d2 = line.Dot(test);
	if (d2 > 0 && d1 > d2)
		return line.Cross(test).LengthSqr();
	else
		return (std::min)(v1.DistanceSqr(p), v2.DistanceSqr(p));
}

inline FFLOAT HashFVec3(const FVec3& v) {
	FFLOAT vx, vy, vz;
	FVec3 temp = v / v.LengthSqr();
	if (v.X >= 0)
		vx = ((long)((temp.X + ROUND) * SCALE)) / SCALE;
	else
		vx = ((long)((temp.X - ROUND) * SCALE)) / SCALE;

	if (v.Y >= 0)
		vy = ((long)((temp.Y + ROUND) * SCALE)) / SCALE;
	else
		vy = ((long)((temp.Y - ROUND) * SCALE)) / SCALE;

	if (v.Z >= 0)
		vz = ((long)((temp.Z + ROUND) * SCALE)) / SCALE;
	else
		vz = ((long)((temp.Z - ROUND) * SCALE)) / SCALE;
	FFLOAT key = vx * (0.1 + vy) * (0.2 + vz);
	return key;
}

inline FFLOAT HashFVec2(const FVec2& v) {
	FFLOAT vx, vy;
	FFLOAT l = std::sqrt(v.X * v.X + v.Y * v.Y);
	FVec2 temp = { v.X / l,v.Y / l };
	if (v.X >= 0)
		vx = ((long)((temp.X + ROUND) * SCALE)) / SCALE;
	else
		vx = ((long)((temp.X - ROUND) * SCALE)) / SCALE;

	if (v.Y >= 0)
		vy = ((long)((temp.Y + ROUND) * SCALE)) / SCALE;
	else
		vy = ((long)((temp.Y - ROUND) * SCALE)) / SCALE;
	FFLOAT key = vx * (0.1 + vy);
	return key;
}

#define HSCALE 100000000
#define HROUND 5e-9f

inline size_t HashFFLOAT(const FFLOAT& v) {
	long x;
	if (v >= 0)
		x = ((long)((v + HROUND) * HSCALE)) / HSCALE;
	else
		x = ((long)((v - HROUND) * HSCALE)) / HSCALE;
	return std::hash<long>()(x);
}

#define WEAK_SCALE 10000
#define WEAK_ROUND 1e-5f
inline FFLOAT WeakHashFVec3(const FVec3& v) {
	FFLOAT vx, vy, vz;
	FVec3 temp = v / v.LengthSqr();
	if (v.X >= 0)
		vx = ((long)((temp.X + WEAK_ROUND) * WEAK_SCALE)) / WEAK_SCALE;
	else
		vx = ((long)((temp.X - WEAK_ROUND) * WEAK_SCALE)) / WEAK_SCALE;

	if (v.Y >= 0)
		vy = ((long)((temp.Y + WEAK_ROUND) * WEAK_SCALE)) / WEAK_SCALE;
	else
		vy = ((long)((temp.Y - WEAK_ROUND) * WEAK_SCALE)) / WEAK_SCALE;

	if (v.Z >= 0)
		vz = ((long)((temp.Z + WEAK_ROUND) * WEAK_SCALE)) / WEAK_SCALE;
	else
		vz = ((long)((temp.Z - WEAK_ROUND) * WEAK_SCALE)) / WEAK_SCALE;
	FFLOAT key = vx * (0.1 + vy) * (0.2 + vz);
	return key;
}

inline size_t WeakHashFFLOAT(const FFLOAT& v) {
	long x;
	if (v >= 0)
		x = ((long)((v + WEAK_ROUND) * WEAK_SCALE)) / WEAK_SCALE;
	else
		x = ((long)((v - WEAK_ROUND) * WEAK_SCALE)) / WEAK_SCALE;
	return std::hash<long>()(x);
}
namespace std {

	template<>
	struct hash<FVec3> {
		size_t operator ()(const FVec3& v) const {

			return hash<FFLOAT>()(HashFVec3(v));
		}
	};

	template<>
	struct hash<FVec2> {
		size_t operator ()(const FVec2& v) const {
			return hash<FFLOAT>()(HashFVec2(v));
		}
	};




}

#endif // VEC_H
