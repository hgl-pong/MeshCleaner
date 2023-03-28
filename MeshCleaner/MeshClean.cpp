#include "MeshClean.h"
#include "IntersectUtils.h"
#include "FTriangulator.h"
#include "HalfEdge.h"
template <typename T>
void Emplace(std::unordered_set<T>& set, std::vector<T>& array) {
	for (auto& data : array) {
		set.emplace(data);
	}
}

template <typename T>
void Erase(std::unordered_set<T>& set, std::unordered_set<T>& array) {
	for (auto& data : array) {
		set.erase(data);
	}
}

MeshClean::MeshClean(FMeshData& mesh)
{
	m_MeshData = mesh;
}

FMeshData MeshClean::Clean()
{
	FMeshData output;
	FindAndFillHoles();
	bool anyIntersect = CalculateIntersect();
	if (anyIntersect)
		Triangulate();
	//_DeleteTrianglesInsideMesh();
	//anyIntersect = CalculateIntersect();
	//if (anyIntersect)
	//	Triangulate();
	_PushInResultBuffer(output);

	return output;
}

void MeshClean::FindAndFillHoles()
{
	std::vector<std::vector<int>> holes;

	//std::vector<FVec3> points;
	//std::unordered_map<FVec3, int> pointsMap;
	//for (auto& v : m_MeshData.m_Vertices) {
	//	pointsMap.insert(std::make_pair(v.position, points.size()));
	//	points.push_back(v.position);
	//}
	/*std::unordered_map<FEdge, int>edgeNeigborsCounter;
	auto EmplaceEdge = [&](FEdge& edge) {
		FEdge iedge = edge;
		std::swap(iedge.v1, iedge.v2);
		edgeNeigborsCounter.emplace(edge, 0).first->second++;
		edgeNeigborsCounter.emplace(iedge, 0).first->second++;
	};
	for (auto& triangle : m_MeshData.m_Triangles) {
		auto& v1 = m_MeshData.m_Vertices[triangle.v1].position;
		auto& v2 = m_MeshData.m_Vertices[triangle.v2].position;
		auto& v3 = m_MeshData.m_Vertices[triangle.v3].position;
		FEdge edge1(v1, v2);
		FEdge edge2(v2, v3);
		FEdge edge3(v3, v1);
		EmplaceEdge(edge1);
		EmplaceEdge(edge2);
		EmplaceEdge(edge3);
	}
	std::unordered_set<FEdge> edgeSet;
	for (auto &edge:edgeNeigborsCounter) {
		if (edge.second == 1)
			edgeSet.emplace(edge.first);
	}

	std::unordered_map<FVec3, std::unordered_set<FVec3>>verticesNeighbors;
	for (auto& edge : edgeSet) 
	{
		auto it = verticesNeighbors.find(edge.v1);
		if (it != verticesNeighbors.end())
			it->second.emplace(edge.v2);
		else
		{
			std::unordered_set<FVec3> set;
			set.emplace(edge.v2);
			verticesNeighbors.emplace(edge.v1, set);
		}
	}
	auto EraseEdge = [&](FEdge& edge) {
		FEdge iedge(edge.v2, edge.v1);
		edgeSet.erase(edge);
		edgeSet.erase(iedge);
		auto it1 = verticesNeighbors.find(edge.v1);
		if (it1 != verticesNeighbors.end()) {
			it1->second.erase(edge.v2);
			if (it1->second.empty())
				verticesNeighbors.erase(it1);
		}

		auto it2 = verticesNeighbors.find(iedge.v1);
		if (it2 != verticesNeighbors.end()) {
			it2->second.erase(iedge.v2);
			if (it2->second.empty())
				verticesNeighbors.erase(it2);
		}
	};

	while (!edgeSet.empty()) {
		std::vector<int> hole;
		FEdge start = *edgeSet.begin();	
		hole.push_back(pointsMap[start.v1]);
		FEdge curr = start;
		hole.push_back(pointsMap[curr.v2]);
		if(verticesNeighbors.find(curr.v2) == verticesNeighbors.end()||verticesNeighbors.find(curr.v1)==verticesNeighbors.end())
			EraseEdge(curr);

		while (verticesNeighbors.find(curr.v2) != verticesNeighbors.end()) 
		{		
			FEdge newEdge(curr.v2, *verticesNeighbors.find(curr.v2)->second.begin());
			EraseEdge(curr);
			curr = newEdge;
			hole.push_back(pointsMap[curr.v2]);
			if (curr.v2 == start.v1 && hole.size() == 4) {
				FTriangle newTriangle(hole[0], hole[1], hole[2], m_MeshData.m_Vertices.data());
				m_MeshData.m_Triangles.push_back(newTriangle);
				break;
			}
			if (curr.v2 == start.v1&&hole.size()>4) {
				holes.push_back(hole);
				break;
			}
		}

	}*/

	std::vector<FVec3> points;
	std::unordered_map<FVec3, int> pointsMap;
	for (auto& v : m_MeshData.m_Vertices) {
		if(pointsMap.emplace(std::make_pair(v.position, points.size())).second)
			points.push_back(v.position);
	}
	std::vector<int>indices;
	
	for (auto& triangle : m_MeshData.m_Triangles)
	{
		indices.push_back(pointsMap[m_MeshData.m_Vertices[triangle.v1].position]);
		indices.push_back(pointsMap[m_MeshData.m_Vertices[triangle.v2].position]);
		indices.push_back(pointsMap[m_MeshData.m_Vertices[triangle.v3].position]);
	}
	HalfEdgeMesh heMesh(points,indices);


	holes = heMesh.FindHoles();
	for (auto& hole : holes) {
		std::vector<FIndex>triangles = FTriangulator::triangulate_polygon(points, hole);
		for (int i = 0; i < triangles.size() / 3; i++) {
			FTriangle newTriangle(hole[triangles[3 * i]], hole[triangles[3 * i + 1]], hole[triangles[3 * i + 2]], m_MeshData.m_Vertices.data());
			m_MeshData.m_Triangles.push_back(newTriangle);
		}
	}
}

bool MeshClean::CalculateIntersect()
{
	bool anyIntersect = false;
	if (m_MeshData.m_Triangles.empty())
		return anyIntersect;
	for (auto triA : m_MeshData.m_Triangles) {
		for (auto triB : m_MeshData.m_Triangles) {

			if (!WeakBoundingBoxIntersection(triA.box, triB.box)) {
				continue;
			}
			if(HasIntersect(triA, triB,m_MeshData.m_Vertices.data()))
				continue;

			IntersectEdge* edge = nullptr;
			TrianglePair newPair1(triA, triB);
			TrianglePair newPair2(triB, triA);
			auto it = m_IntersectMap.find(newPair1);
			if (it == m_IntersectMap.end())
				IntersectUtils::TrianglesIntersect(triA, triB, m_MeshData.m_Vertices, m_MeshData.m_Vertices, edge);
			else {
				edge = new IntersectEdge[2];
				edge[0] = it->second;
				edge[1] = m_IntersectMap.find(newPair2)->second;
			}

			if (edge) {
				m_IntersectMap.emplace(newPair1, edge[0]);
				m_IntersectMap.emplace(newPair2, edge[1]);

				auto it = m_IntersectNeighbors.find(triA);
				if (it != m_IntersectNeighbors.end())
					it->second.emplace(triB);
				else {
					std::unordered_set<FTriangle> set;
					set.emplace(triB);
					m_IntersectNeighbors.emplace(triA, set);
				}

				it = m_IntersectNeighbors.find(triB);
				if (it != m_IntersectNeighbors.end())
					it->second.emplace(triA);
				else {
					std::unordered_set<FTriangle> set;
					set.emplace(triA);
					m_IntersectNeighbors.emplace(triB, set);
				}
			}
			else {
			}
		}
	}

	for (auto tri : m_MeshData.m_Triangles) {
		if (m_IntersectNeighbors.find(tri) == m_IntersectNeighbors.end())
			m_MeshSet.emplace(tri);
	}
	anyIntersect = (m_MeshSet.size() != m_MeshData.m_Triangles.size());
	return anyIntersect;
}


void MeshClean::Triangulate()
{
	for (auto triA : m_IntersectNeighbors) {
		std::unordered_map<FIndex, FVertex> pointsMap;
		std::vector<FVertex> points;
		std::unordered_map<FVertex, FIndex> indexMap;
		std::unordered_map<FIndex, std::unordered_set<FIndex>> edges;
		std::vector<FTriangle> triangles;
		for (auto triB : triA.second) {
			TrianglePair pair(*const_cast<FTriangle*>(&triA.first), triB);
			auto it = m_IntersectMap.find(pair);
			if (it == m_IntersectMap.end())
				continue;
			FVertex& keyA = it->second.first;
			FVertex& keyB = it->second.second;
			auto it1 = indexMap.find(keyA);
			auto it2 = indexMap.find(keyB);
			FIndex first, second;
			if (it1 == indexMap.end()) {
				first = indexMap.size() + 3;
				indexMap.emplace(keyA, first);
				pointsMap.emplace(first, it->second.first);

				//points.push_back(it->second.first);
			}
			else
				first = it1->second;

			if (it2 == indexMap.end()) {
				second = indexMap.size() + 3;
				indexMap.emplace(keyB, second);
				pointsMap.emplace(second, it->second.second);
				//points.push_back(it->second.second);
			}
			else
				second = it2->second;

			auto it3 = edges.find(first);
			if (it3 != edges.end())
				it3->second.emplace(second);
			else {
				std::unordered_set<FIndex> set;
				set.emplace(second);
				edges.emplace(first, set);
			}

			auto it4 = edges.find(second);
			if (it4 != edges.end())
				it4->second.emplace(first);
			else {
				std::unordered_set<FIndex> set;
				set.emplace(first);
				edges.emplace(second, set);
			}
		}

			points.resize(pointsMap.size() + 3);
			points[0] = m_MeshData.m_Vertices[triA.first.v1];
			points[1] = m_MeshData.m_Vertices[triA.first.v2];
			points[2] = m_MeshData.m_Vertices[triA.first.v3];
			for (int i = 3; i < points.size(); i++)
				points[i] = pointsMap[i];
			FTriangulator::Triangulating(*const_cast<FTriangle*>(&triA.first), m_MeshData.m_Vertices, points, edges, triangles);
			Emplace(m_MeshSet, triangles);

	}
}

void MeshClean::_PushInResultBuffer(FMeshData& output) {
	int triSize = output.m_Triangles.size();
	int triSize2 = triSize + m_MeshSet.size();
	int size = output.m_Vertices.size();


	output.m_Triangles.resize(triSize2);
	auto tri = m_MeshSet.begin();
	for (int i = triSize; i < triSize2; i++) {
		output.m_Triangles[i] = *tri;
		FTriangle& triangle = output.m_Triangles[i];
		triangle.v1 += size;
		triangle.v2 += size;
		triangle.v3 += size;
		tri++;
	}

	output.m_Vertices.resize(size + m_MeshData.m_Vertices.size());
	memcpy_s(output.m_Vertices.data() + size, sizeof(FVertex) * m_MeshData.m_Vertices.size(),
		m_MeshData.m_Vertices.data(), sizeof(FVertex) * m_MeshData.m_Vertices.size());
}

void MeshClean::_DeleteTrianglesInsideMesh()
{
	std::unordered_set<FVec3> pointsOutOfMeshA;
	std::unordered_set<FVec3> pointsInMeshA;
	std::unordered_set<FTriangle> triangleToDelete;

	for (auto& triangleB : m_MeshSet) {
		const auto& center = triangleB.center.position;

		FVec3 normal = Normal(m_MeshData.m_Vertices[triangleB.v1].position,
			m_MeshData.m_Vertices[triangleB.v2].position,
			m_MeshData.m_Vertices[triangleB.v3].position) * 100;

		bool isOut = true;
		if (pointsInMeshA.find(center) != pointsInMeshA.end())
			isOut = false;
		if (isOut) {
			if (IntersectUtils::IsInsideMesh(m_MeshData, center, normal)) {
				isOut = false;
			}
		}
		if (isOut) {
			pointsOutOfMeshA.emplace(center);
		}
		else
		{
			pointsInMeshA.emplace(center);
			triangleToDelete.emplace(triangleB);
		}
	}

	for (auto triangle : triangleToDelete)
		m_MeshSet.erase(triangle);
	triangleToDelete.clear();
}
