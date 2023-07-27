//
// Created by Administrator on 2019/3/11 0011.
//
/**
 * 管理mesh的io
 */
#ifndef HALFEDGE_DATA_LOADER_H
#define HALFEDGE_DATA_LOADER_H
#include "FBoundingBox.h"
#include <iostream>     // std::cin, std::cout
#include <fstream>      // std::ifstream
#include <vector>
#include <string>
#include <unordered_map>
#include <iomanip>
#include"vtk.h"
using namespace std;

static int readTri(ifstream& is, FMeshData& mesh, int index, FFLOAT scale = 1, FVec3 offset = { 0,0,0 }) {
    vector<FVec3> points;
    vector<FTriangle> triangles;
    vector<FEdge> edges;
    int pcount = 0, tricount = 0;
    string s;
    char c;
    while (is.get(c)) {
        switch (c) {
        case '#':
            getline(is, s);
            //if (s.find("Vertices") != string::npos) {
            //    points.resize(size_t (stoll(s.substr(11))));
            //}
            //if (s.find("Faces") != string::npos) {
            //    triangles.resize(size_t (stoll(s.substr(8))));
            //}
            break;
        case 'v':
        {
            FVec3 p;
            //p.index = pcount;
            is >> p.X >> p.Y >> p.Z;
            is.get();
            p = (p + offset) * scale;
            points.push_back(p);
            pcount++;
            //points[pcount++] = p;
            break;
        }
        case 'f':
        {

            int x, y, z;
            is >> x >> y >> z;
            is.get();
            if (!(x <= points.size() && y <= points.size() && z <= points.size())) {
                cout << "数组越界：x=" << x << "，y=" << y << "，z=" << z << endl;
                cout << "maxSize=" << points.size() << endl;
                exit(-1);
            }
            FTriangle triangle = FTriangle(x - 1, y - 1, z - 1, points.data(), index);
            triangles.push_back(triangle);
            tricount++;
            //triangles[tricount++] = triangle;
            break;
        }
        case 'm':
            getline(is, s);
            break;
        case 'o':
            getline(is, s);
            break;
        case 'u':
            getline(is, s);
            break;
        default: {
            break;
        }
        }
    }
    mesh.m_Triangles = triangles;
    mesh.m_Vertices = points;

    printf("点数量:%d\n", points.size());
    printf("三角形数量:%d\n", triangles.size());
    return 0;
}

static int writeVtk(string path, vector<FVec3> points, vector<FTriangle> triangles) {
    if (points.empty())
        return 0;
    ofstream of("result.obj");
    vector<float > nodes;
    vector<unsigned int> faces;
    for (FIndex i = 0; i < points.size(); i++) {
        of << "v " << std::setprecision(4) << points[i].X << " " << points[i].Y << " " << points[i].Z << endl;
        nodes.push_back(points[i].X);
        nodes.push_back(points[i].Y);
        nodes.push_back(points[i].Z);
    }
    of << endl;
    for (FIndex i = 0; i < triangles.size(); i++) {
        faces.push_back(triangles[i].v1);
        faces.push_back(triangles[i].v2);
        faces.push_back(triangles[i].v3);
        int v1 = triangles[i].v1 + 1;
        int v2 = triangles[i].v2 + 1;
        int v3 = triangles[i].v3 + 1;
        of << "f " << v1 << " " << v2 << " " << v3 << endl;

    }
    of.close();
    ofstream os(path.c_str());
    tri2vtk(os, &nodes[0], points.size(), &faces[0], triangles.size());
    printf("%d\n", points.size());
    printf("%d\n", triangles.size());
    return 1;
}

static int WritePoint2D(std::vector<FVec2>& points) {
    if (points.empty())
        return 0;
    ofstream of("triangle2d.csv");
    for (auto& point : points)
        of << point.X << "," << point.Y << endl;
    of.close();
    return 1;
}
#endif
