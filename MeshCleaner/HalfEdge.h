#ifndef HALF_EDGE_H
#define HALF_EDGE_H
#include <vector>
#include <algorithm>
#include <unordered_set>
#include "vec.h"
class HalfEdge;
class Vertex;
class Edge;
class Face;
class HalfEdge {
public:
    Vertex* vertex;
    HalfEdge* pair;
    HalfEdge* next;
    HalfEdge* prev;
    Edge* edge;
    Face* face;
};
class Vertex {
public:
    FVec3 position;
    HalfEdge* halfEdge;
    Vertex(float x = 0, float y = 0, float z = 0) : position(x, y, z), halfEdge(nullptr) {}
};
class Edge {
public:
    HalfEdge* halfEdge;
};
class Face {
public:
    HalfEdge* halfEdge;
};
class HalfEdgeMesh {
public:
    std::vector<Vertex*> vertices;
    std::vector<Edge*> edges;
    std::vector<Face*> faces;
    std::vector<HalfEdge*> halfEdges;
    HalfEdgeMesh(const std::vector<FVec3>& vertexData, const std::vector<int>& faceData) {
        // 创建顶点
        for (const auto& pos : vertexData) {
            vertices.push_back(new Vertex(pos.X, pos.Y, pos.Z));
        }
        // 创建面和半边结构
        for (size_t i = 0; i < faceData.size(); i += 3) {
            Face* face = new Face();
            faces.push_back(face);
            HalfEdge* firstHalfEdge = nullptr;
            HalfEdge* prevHalfEdge = nullptr;
            for (size_t j = 0; j < 3; ++j) {
                int vertexIndex = faceData[i + j];
                int nextVertexIndex = faceData[i + (j + 1) % 3];
                HalfEdge* halfEdge = new HalfEdge();
                halfEdges.push_back(halfEdge);
                Vertex* vertex = vertices[vertexIndex];
                vertex->halfEdge = halfEdge;
                halfEdge->vertex = vertex;
                halfEdge->face = face;
                if (firstHalfEdge == nullptr) {
                    firstHalfEdge = halfEdge;
                }
                else {
                    prevHalfEdge->next = halfEdge;
                    halfEdge->prev = prevHalfEdge;
                }
                prevHalfEdge = halfEdge;
            }
            firstHalfEdge->prev = prevHalfEdge;
            prevHalfEdge->next = firstHalfEdge;
            face->halfEdge = firstHalfEdge;
        }
        // 创建边和半边对
        for (HalfEdge* he1 : halfEdges) {
            if (he1->pair == nullptr) {
                for (HalfEdge* he2 : halfEdges) {
                    if (he2->pair == nullptr && he1->vertex == he2->next->vertex && he1->next->vertex == he2->vertex) {
                        Edge* edge = new Edge();
                        edges.push_back(edge);
                        he1->pair = he2;
                        he2->pair = he1;
                        he1->edge = edge;
                        he2->edge = edge;
                        edge->halfEdge = he1;
                        break;
                    }
                }
            }
        }
    }
    // 释放内存
    ~HalfEdgeMesh() {
        for (auto v : vertices) delete v;
        for (auto e : edges) delete e;
        for (auto f : faces) delete f;
        for (auto he : halfEdges) delete he;
    }
    // 查找孔洞
    std::vector<std::vector<int>> FindHoles() {
        std::vector<std::vector<int>> holes;
        std::unordered_set<HalfEdge*> visited;
        for (auto he : halfEdges) {
            if (he->face == nullptr && visited.find(he) == visited.end()) {
                std::vector<int> hole;
                HalfEdge* curr = he;
                do {
                    hole.push_back(std::distance(vertices.begin(), std::find(vertices.begin(), vertices.end(), curr->vertex)));
                    visited.insert(curr);
                    curr = curr->next;
                } while (curr != he);
                holes.push_back(hole);
            }
        }
        return holes;
    }
};
#endif // !HALF_EDGE_H
