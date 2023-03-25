#ifndef FBOUNDING_BOX_H
#define FBOUNDING_BOX_H
#include <vector>
#include <string>
#include "vec.h"
#include <unordered_set>
#include <map>
typedef uint32_t FIndex;

struct FVertex {
    //FIndex index;
    FVec3 position;
    FVec3 normals;
    FVec2 uv;
    FVec4 tangent;
    uint32_t diffuse;

    bool operator==(const FVertex& v) const { 
        return position == v.position && uv == v.uv;
    }

	bool operator<(const FVertex& v) const {
		return position < v.position && uv < v.uv;
	}

};

struct FEdge
{
    FIndex v[2];

    FIndex& start() {
        return v[0];
    }
    const FIndex& start() const {
        return v[0];
    }

    FIndex& end() {
        return v[1];
    }
    const FIndex& end() const { return v[1]; }

    FEdge()
    {
        v[0] = v[1] = -1;
    }
};

class FBoundingBox
{
public:
    FBoundingBox();
    FBoundingBox(const std::vector<FVec3>& points);
    FBoundingBox(const std::vector<FVertex>& points);
    FBoundingBox(FVec3& v0, FVec3& v1, FVec3& v2);
    ~FBoundingBox();

    bool Intersection(FBoundingBox& box);
    void Merge(FBoundingBox& box);
    void Include(FVec3& v);
    bool isContain(const FVec3& v);

    FBoundingBox operator*= (const FFLOAT& d)const{
        m_Min * d;
        m_Max * d;
        m_Size * d;
        return *this;
    }

    FFLOAT DistanceSqr(const FVec3& v) {
        FFLOAT dx = (std::max)((std::max)(m_Min.X - v.X, (FFLOAT)0), v.X - m_Max.X);
        FFLOAT dy = (std::max)((std::max)(m_Min.Y - v.Y, (FFLOAT)0), v.Y - m_Max.Y);
        FFLOAT dz = (std::max)((std::max)(m_Min.Z - v.Z, (FFLOAT)0), v.Z - m_Max.Z);
        return dx * dx + dy * dy + dz * dz;
    }


    FFLOAT DistanceInAxis(const FVec3& v,int axis) {
        if(axis==0)
        return (std::max)((std::max)(m_Min.X - v.X, (FFLOAT)0), v.X - m_Max.X);
        if(axis==1)
        return (std::max)((std::max)(m_Min.Y - v.Y, (FFLOAT)0), v.Y - m_Max.Y);
        if(axis==2)
        return (std::max)((std::max)(m_Min.Z - v.Z, (FFLOAT)0), v.Z - m_Max.Z);
        return -1;
    }

public:
    FVec3 m_Min;
    FVec3 m_Max;
    FVec3 m_Size;
    FVec3 m_Center;
};

struct FTriangle
{
    int v1,v2,v3;
    FVertex center;
    FBoundingBox box;


    FTriangle() { v1 = 0, v2 = 0, v3 = 0; };
	FTriangle(const int& x, const int& y,const int& z,  FVertex* points)
    {
		v1 = x;
		v2 = y;
		v3 = z;
        center.position = (points[x].position+ points[y].position+ points[z].position) / 3;
		box = FBoundingBox(points[x].position, points[y].position, points[z].position);
	}

    bool operator ==(const FTriangle& triangle) const;

	bool operator <(const FTriangle& triangle) const {
        return v1 < triangle.v1 && v2 == triangle.v2 && v3 == triangle.v3;
	}

    FTriangle& operator =(const FTriangle& triangle) {
        v1 = triangle.v1;
        v2 = triangle.v2;
        v3 = triangle.v3;
        center = triangle.center;
        box = triangle.box;
        return *const_cast<FTriangle*>(this);
    }
};

namespace std {

    template<>
    struct hash<FVertex> {
        size_t operator ()(const FVertex& x) const {
            FFLOAT key = HashFVec3(x.position) * (0.1 + HashFVec2(x.uv));
            return hash<FFLOAT>()(key);
        }
    };

    template<>
    struct hash<FTriangle> {
        size_t operator ()(const FTriangle& x)const {

            FFLOAT key = HashFVec3(x.center.position) * (0.1 + HashFVec3(x.box.m_Size));
            return hash<FFLOAT>()(key);
        }
    };
}

static FFLOAT DistanceSqr(const FTriangle& triangle ,std::vector<FVertex>& vBuffer,const FVec3& p) {
    FVec3& v0 = vBuffer.at(triangle.v1).position;
    FVec3& v1 = vBuffer.at(triangle.v2).position;
    FVec3& v2 = vBuffer.at(triangle.v3).position;
    FFLOAT d01 = v0.Distance(v1);
    FFLOAT d12 = v1.Distance(v2);
    FFLOAT d20 = v2.Distance(v0);
    FFLOAT numer = d01 * d01 + d20 * d20 - d12 * d12;
    FFLOAT inv_denom = 0.5 / (d01 * d20);
    FFLOAT u = numer * inv_denom;
    FFLOAT v = (d01 * d01 - u * (numer + d20 * d20)) / (d01 * d01 + d20 * d20 - 2 * d01 * d20 * u);
    if (u >= 0-FLOAT_EPSILON && v >= -FLOAT_EPSILON && u + v <= 1+FLOAT_EPSILON) {
        FVec3 pt = v0 * u + v1 * v + v2 * (1 - u - v);
        return pt.DistanceSqr(p);
    }
    else {
        return (std::min)(v0.DistanceSqr(p), (std::min)(v1.DistanceSqr(p), v2.DistanceSqr(p)));
    }
}
struct FMeshData {
    std::vector<FTriangle> m_Triangles;
    std::vector<FVertex> m_Vertices;
};

static bool WeakBoundingBoxIntersection(const FBoundingBox& aBox, const FBoundingBox& bBox)
{
	if ((std::max)(aBox.m_Min.X, bBox.m_Min.X) > (std::min)(aBox.m_Max.X, bBox.m_Max.X) + FLOAT_EPSILON)
		return false;
	if ((std::max)(aBox.m_Min.Y, bBox.m_Min.Y) > (std::min)(aBox.m_Max.Y, bBox.m_Max.Y) + FLOAT_EPSILON)
		return false;
	if ((std::max)(aBox.m_Min.Z, bBox.m_Min.Z) > (std::min)(aBox.m_Max.Z, bBox.m_Max.Z) + FLOAT_EPSILON)
		return false;
	return true;
}

static FVec3 Normal(FTriangle& triangle,FVertex* vBuffer) {
    return Normal(vBuffer[triangle.v1].position, vBuffer[triangle.v2].position, vBuffer[triangle.v3].position);
}

static FFLOAT CalCulateVolume(FMeshData& meshdata) { 
    FFLOAT volume = 0;
    for (auto& tri : meshdata.m_Triangles)
    {
        const FVec3& a = meshdata.m_Vertices[tri.v1].position;
        const FVec3& b = meshdata.m_Vertices[tri.v2].position;
        const FVec3& c = meshdata.m_Vertices[tri.v3].position;

        volume += (a.X * b.Y * c.Z - a.X * b.Z * c.Y - a.Y * b.X * c.Z + a.Y * b.Z * c.X + a.Z * b.X * c.Y - a.Z * b.Y * c.X);
    }
    return (1.0f / 6.0f) * std::abs(volume);
}

static FVec3 CalCulateNormals(FMeshData& meshdata) {
    FVec3 surfaceNormal;
    for (auto& v : meshdata.m_Vertices)
        v.normals = FVec3();
    for (auto& triangle:meshdata.m_Triangles) {
        FVec3 v01 = meshdata.m_Vertices[triangle.v2].position - meshdata.m_Vertices[triangle.v1].position;
        FVec3 v02 = meshdata.m_Vertices[triangle.v3].position - meshdata.m_Vertices[triangle.v1].position;
        FVec3 normal = v01.Cross(v02).Normalize();

        meshdata.m_Vertices[triangle.v1].normals = meshdata.m_Vertices[triangle.v1].normals + normal;
        meshdata.m_Vertices[triangle.v2].normals = meshdata.m_Vertices[triangle.v2].normals + normal;
        meshdata.m_Vertices[triangle.v3].normals = meshdata.m_Vertices[triangle.v3].normals + normal;
        surfaceNormal += normal;
    }
    for (auto& v : meshdata.m_Vertices)
        v.normals.Normalize();
    surfaceNormal.Normalize();
    return surfaceNormal;
}

static FVec2 RemapTextureCoords(FVec2& texCoords)
{
    float x = texCoords.X;
    float y = texCoords.Y;
    // 进行纹理坐标的重映射
    // 这里使用线性插值进行重映射，可以根据需要选择其他方法
    texCoords.X = x * (1.0f - y) + y * x * y;
    texCoords.Y = y * (1.0f - x) + x * x * y;
    return texCoords;
}


//class UvMapper {
//public:
//    UvMapper(FBoundingBox&box) {
//        m_Box = box;
//    }
//    void AutoMapUV(FMeshData& mesh) {
//        for (auto& ver : mesh.m_Vertices) {
//            FVec3 box_pos = (ver.position - m_Box.m_Min) / m_Box.m_Size;
//
//            FVec2& uv= ver.uv;;
//            if (box_pos.X >= box_pos.Y && box_pos.X >= box_pos.Z) {
//                uv.X = box_pos.Y;
//                uv.Y = box_pos.Z;
//            }
//            else if (box_pos.Y >= box_pos.X && box_pos.Y >= box_pos.Z) {
//                uv.X = box_pos.X;
//                uv.Y = box_pos.Z;
//            }
//            else {
//                uv.X = box_pos.X;
//                uv.Y = box_pos.Y;
//            }
//        }
//    }
//private:
//    FBoundingBox m_Box;
//};

static void CalculateUVs(FMeshData& meshdata,FBoundingBox&box,FVec3& surfaceNormal) {
    //UvMapper mapper(box);
    //mapper.AutoMapUV(meshdata);
    //for (auto& v:meshdata.m_Vertices) {  
    //    FVec3 uv = (v.position - box.m_Min) / box.m_Size;
    //    v.uv = FVec2(uv.X/(1+uv.Z), uv.Y/(1+uv.Z));
    //    RemapTextureCoords(v.uv);
    //}

    //std::vector<FVec2> texBuffer(meshdata.m_Triangles.size() * 3);
    //FVec3 P0 = meshdata.m_Vertices[0].position;
    //FVec3 P1 = P0 + surfaceNormal;  // 选择一个点和法向量作为投影平面
    //std::vector<FVertex>vBuffer = meshdata.m_Vertices;
    //for (auto& Vertex : vBuffer)
    //{
    //    FVec3 ProjPoint = Vertex.position - surfaceNormal * (Vertex.position - P0).Dot(surfaceNormal);
    //    Vertex.position = ProjPoint;
    //}

    //const float MinU = 0.0f;
    //const float MaxU = 1.0f;

    //const float MinV = 0.0f;
    //const float MaxV = 1.0f;
    //int i = 0;
    //for (auto&tri:meshdata.m_Triangles)
    //{
    //    const FVec3& V0 = vBuffer[tri.v1].position;
    //    const FVec3& V1 = vBuffer[tri.v2].position;
    //    const FVec3& V2 = vBuffer[tri.v3].position;

    //    FVec3 uv1 = (V0 - box.m_Min) / box.m_Size;
    //    FVec3 uv2 = (V1 - box.m_Min) / box.m_Size;
    //    FVec3 uv3 = (V2 - box.m_Min) / box.m_Size;

    //    texBuffer[3 * i] = FVec2(uv1.X * uv1.Z, uv1.Y * uv1.Z);
    //    texBuffer[3*i+1] = FVec2(uv2.X * uv2.Z, uv2.Y * uv2.Z);
    //    texBuffer[3*i+2] = FVec2(uv3.X * uv3.Z, uv3.Y * uv3.Z);
    //    i++;
    //}

    //FVec2 umin(1000, 1000);
    //FVec2 umax(-1000, 1000);
    //for (auto& Vertex:meshdata.m_Vertices)
    //{
    //    std::map<int,FTriangle*> AdjTriangles;
    //    int i = 0;
    //    for (auto& tri:meshdata.m_Triangles)
    //    {
    //        if (tri.v1 == &Vertex - &meshdata.m_Vertices[0] || tri.v2 == &Vertex - &meshdata.m_Vertices[0] || tri.v3 == &Vertex - &meshdata.m_Vertices[0])
    //        {
    //            AdjTriangles.emplace(i,&tri);
    //        }
    //        i++;
    //    }

    //    FVec2 UV;
    //    float TotalArea = 0.0f;

    //    for (const auto& it : AdjTriangles)
    //    {
    //        const FTriangle* tri = it.second;
    //        const FVec2& UV0 = texBuffer[3*it.first];
    //        const FVec2& UV1 = texBuffer[3 * it.first+1];
    //        const FVec2& UV2 = texBuffer[3 * it.first+2];

    //        FVec3 Edge1 = meshdata.m_Vertices[tri->v2].position - meshdata.m_Vertices[tri->v1].position;
    //        FVec3 Edge2 = meshdata.m_Vertices[tri->v3].position - meshdata.m_Vertices[tri->v1].position;
    //        float TriangleArea = Edge1.Cross(Edge2).LengthSqr() * 0.5;

    //        UV += (UV0 + UV1 + UV2)  *TriangleArea/ 3.0f;
    //        TotalArea += TriangleArea;
    //    }

    //    if (TotalArea > 0.0f)
    //    {
    //        UV = UV / TotalArea;
    //    }

    //    Vertex.uv = UV;

    //FVec2 umin(1000, 1000);
    //FVec2 umax(-1000, 1000);
    //for (auto& Vertex : meshdata.m_Vertices){
    //    FVec2 UV = Vertex.uv;

    //    umin.X = (std::min)(UV.X, umin.X);
    //    umin.Y = (std::min)(UV.Y, umin.Y);

    //    umax.X = (std::max)(UV.X, umax.X);
    //    umax.Y = (std::max)(UV.Y, umax.Y);
    //}

    //for (auto& Vertex : meshdata.m_Vertices) {
    //    Vertex.uv.X = (Vertex.uv.X - umin.X) / (umax.X - umin.X);
    //    Vertex.uv.Y = (Vertex.uv.Y - umin.Y) / (umax.Y - umin.Y);
    //}
}

static void MergeMeshdata(FMeshData& main, FMeshData& targ) {
    int size = main.m_Vertices.size();
    for (auto tri : targ.m_Triangles) {
        tri.v1 += size;
        tri.v2 += size;
        tri.v3 += size;;
        main.m_Triangles.push_back(tri);
    }
    for (auto& v : targ.m_Vertices)
        main.m_Vertices.push_back(v);
}

#endif // FBOUNDING_BOX_H
