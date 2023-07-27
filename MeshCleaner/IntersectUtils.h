#ifndef INTERSECT_UTILS_H
#define INTERSECT_UTILS_H
#include "FBoundingBox.h"
#include <unordered_set>
#include <unordered_map>
#include "tri_tri_intersect.h"
#include <map>
#include <set>
typedef std::pair<FVec3, int> DetectPair;

namespace std {

	template<>
	struct hash<DetectPair> {
		size_t operator ()(const DetectPair& x) const {
			std::string key = std::to_string(HashFVec3(x.first)) + std::to_string(x.second);
			return hash<std::string>()(key);
		}
	};
}


enum TestDirection
{
	Positive_X,
	Positive_Y,
	Positive_Z,
	//Negative_X,
	//Negative_Y,
	//Negative_Z
};

static const std::vector<FVec3> g_testAxisList = {
{FLOAT_MAX, 0, 0},
{0, FLOAT_MAX, 0},
{0, 0, FLOAT_MAX},
//{FFLOAT_MIN, 0, 0},
//{0, FFLOAT_MIN, 0},
//{0, 0, FFLOAT_MIN}
};


class IntersectUtils {
public:

	static bool TrianglesIntersect(const FTriangle& triangleA, const FTriangle& triangleB, std::vector<FVec3>& vBufferA, std::vector<FVec3>& vBufferB, std::pair<FVec3, FVec3>*& edge) {
		int coplanar = 0;
		edge = nullptr;
		FVec3 posA, posB;
		if (!tri_tri_intersection_test_3d(
			(FFLOAT*)(&vBufferA[triangleA.v1]),
			(FFLOAT*)(&vBufferA[triangleA.v2]),
			(FFLOAT*)(&vBufferA[triangleA.v3]),
			(FFLOAT*)(&vBufferB[triangleB.v1]),
			(FFLOAT*)(&vBufferB[triangleB.v2]),
			(FFLOAT*)(&vBufferB[triangleB.v3]),
			&coplanar,
			(FFLOAT*)(&posA),
			(FFLOAT*)(&posB))) {
			return false;
		}
		if (coplanar)
			return false;
		if (posA == posB)
			return false;
		edge = new std::pair<FVec3, FVec3>[2];
		edge[0].first = posA;
		edge[0].second = posB;

		edge[1].first = posA;
		edge[1].second = posB;

		//if(!(vBufferA[triangleA.v1].uv==FVec2(0,0)&& vBufferA[triangleA.v2].uv == FVec2(0, 0)&& vBufferA[triangleA.v3].uv == FVec2(0, 0)))
		//CalcluateUV(triangleA,vBufferA, edge[0]);
		//if (!(vBufferB[triangleB.v1].uv == FVec2(0, 0) && vBufferB[triangleB.v2].uv == FVec2(0, 0) && vBufferB[triangleB.v3].uv == FVec2(0, 0)))
		//CalcluateUV(triangleB,vBufferB, edge[1]);
	}


	static bool IsInMesh(FMeshData&meshdata, FVec3 point, const FVec3& testAxis)
	{
		std::vector<FTriangle>& triangles = meshdata.m_Triangles;
		FVec3 testEnd = point + testAxis;
		bool inside = false;
		FBoundingBox box;
		box.Include(point);
		box.Include(testEnd);
		std::unordered_set<DetectPair> hits;
		int time = 0;
		for (const auto triangle : triangles) {
			if (!WeakBoundingBoxIntersection(box, triangle.box))
				continue;

			std::vector<FVec3> trianglePositions = {
				meshdata.m_Vertices[triangle.v1]  ,
				meshdata.m_Vertices[triangle.v2]  ,
				meshdata.m_Vertices[triangle.v3] 
			};
			if (IsInTriangle(point, trianglePositions.data()))
				return true;
			FVec3 intersection;
			FVec3 normal = Normal(trianglePositions.at(0), trianglePositions.at(1), trianglePositions.at(2));

			if (IntersectSegmentAndPlane(point , testEnd ,
				trianglePositions[0],
				normal,
				intersection)) {
				if (IsInTriangle(intersection, trianglePositions.data())) {
					FFLOAT dir = normal.Dot(testAxis);
					bool sameDir = dir >= 0;
					if (hits.emplace(std::make_pair(intersection,sameDir)).second&&sameDir)
						time++;
				}
			}
		}
		inside = (2*time > hits.size());
		return inside;
	}

	static bool IsInsideMesh(FMeshData& meshdata, FVec3 point, const FVec3& testAxis)
	{
		std::vector<FTriangle>& triangles = meshdata.m_Triangles;
		FVec3 testEnd = point + testAxis;
		bool inside = false;
		FBoundingBox box;
		box.Include(point);
		box.Include(testEnd);
		std::unordered_set<DetectPair> hits;
		int time = 0;
		for (const auto triangle : triangles) {
			if (!WeakBoundingBoxIntersection(box, triangle.box))
				continue;

			std::vector<FVec3> trianglePositions = {
				meshdata.m_Vertices[triangle.v1]  ,
				meshdata.m_Vertices[triangle.v2]  ,
				meshdata.m_Vertices[triangle.v3]
			};
			if (IsInTriangle(point, trianglePositions.data())) {
				continue;
			}
			FVec3 intersection;
			FVec3 normal = Normal(trianglePositions.at(0), trianglePositions.at(1), trianglePositions.at(2));

			if (IntersectSegmentAndPlane(point, testEnd,
				trianglePositions[0],
				normal,
				intersection)) {
				if (IsInTriangle(intersection, trianglePositions.data())) {
					FFLOAT dir = normal.Dot(testAxis);
					bool sameDir = dir > 0;
					if (hits.emplace(std::make_pair(intersection, sameDir)).second && sameDir)
						time++;
				}
			}
		}
		inside = (2 * time > hits.size());
		return inside;
	}
};
#endif // INTERSECT_UTILS_H
