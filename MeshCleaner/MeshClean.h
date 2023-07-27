#ifndef MESH_CLEAN_H
#define MESH_CLEAN_H
#include "FBoundingBox.h"
#include <unordered_map>
#include <unordered_set>
typedef std::pair<FVec3, FVec3> IntersectEdge;

class TrianglePair {
public:
	TrianglePair(const FTriangle& triangleA, const FTriangle& triangleB)
		:m_TriangleA(triangleA),
		m_TriangleB(triangleB)
	{
	};
	~TrianglePair() = default;
	bool operator ==(const TrianglePair& pair)const {
		return (m_TriangleA == pair.m_TriangleA && m_TriangleB == pair.m_TriangleB);
	}
	FTriangle& first() const {
		return *const_cast<FTriangle*>(&m_TriangleA);
	}
	FTriangle& second()const {
		return *const_cast<FTriangle*>(&m_TriangleB);
	}
private:
	FTriangle m_TriangleA, m_TriangleB;
};

namespace std {
	template<>
	struct hash<TrianglePair>
	{
		size_t operator ()(const TrianglePair& x) const {
			string key = to_string(hash<FTriangle>()(x.first())) + to_string(hash<FTriangle>()(x.second()));
			return   hash<string>()(key);
		}
	};
}

struct Hole 
{

};

class MeshClean
{
public :
	MeshClean(FMeshData& mesh);
	FMeshData Clean();
	void FindAndFillHoles();
	bool CalculateIntersect();
	void Triangulate();
private:
	void _PushInResultBuffer(FMeshData& meshdata);
	void _DeleteTrianglesInsideMesh();
private:
	FMeshData m_MeshData;
	std::unordered_set<FTriangle> m_MeshSet;
	std::unordered_map<TrianglePair, IntersectEdge> m_IntersectMap;
	std::unordered_map<FTriangle, std::unordered_set<FTriangle>> m_IntersectNeighbors;
};

inline bool HasIntersect(FTriangle& triangleA, FTriangle& triangleB, FVec3* vBuffer)
{
	std::unordered_set<FVec3> set;
	set.emplace(vBuffer[triangleA.v1]);
	set.emplace(vBuffer[triangleA.v2]);
	set.emplace(vBuffer[triangleA.v3]);
	int time = 0;
	auto it1 = set.find(vBuffer[triangleB.v1]);
	auto it2 = set.find(vBuffer[triangleB.v2]);
	auto it3 = set.find(vBuffer[triangleB.v3]);
	//if (it1 != set.end())
	//	time++;
	//if (it2 != set.end())
	//	time++;
	//if (it3 != set.end())
	//	time++;
	//return time >=1;
	return  it1 != set.end() || it2 != set.end() || it3 != set.end();
}

static void TransformMesh(FMeshData& meshdata, const FVec3& moffset, const FFLOAT& mscale) {
	if (mscale <= 1) {
		for (auto& tri : meshdata.m_Triangles) {
			tri.box.m_Max = (tri.box.m_Max - moffset) * mscale;
			tri.box.m_Min = (tri.box.m_Min - moffset) * mscale;
			tri.box.m_Size *= mscale;
		}
		for (auto& v : meshdata.m_Vertices) {
			v = (v - moffset) * mscale;
		}
	}
	else {
		for (auto& tri : meshdata.m_Triangles) {
			tri.box.m_Max = tri.box.m_Max* mscale - moffset ;
			tri.box.m_Min = tri.box.m_Min* mscale - moffset ;
			tri.box.m_Size *= mscale;
		}
		for (auto& v : meshdata.m_Vertices) {
			v = v* mscale - moffset ;
		}
	}
}
#endif // !1



