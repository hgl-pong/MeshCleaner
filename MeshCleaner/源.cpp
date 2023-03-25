#include <iostream>
#include <string>
#include <fstream>
#include "IO.h"
#include "IntersectUtils.h"
#include <time.h>
#include "MeshClean.h"

using namespace std;
int main() {
	int nums[3] = { 10,100,300 };
	for (int j = 0; j < 1; j++) {
		string file = "teapot";
		int num = nums[j];
		string outputDir = "output.vtk";
		string input = file + ".obj";
		ifstream is;
		FMeshData meshA;
		is.open(input);
		if (!is.is_open()) {
			cout << "fail to open the file" << endl;
			return -1;
		}
		readTri(is, meshA);
		FBoundingBox box(meshA.m_Vertices);

		FVec3 moffset = box.m_Center;
		FFLOAT mscale = std::max(box.m_Size.X, std::max(box.m_Size.Y, box.m_Size.Z));
		FFLOAT mscalefactor = 1 / mscale;
		TransformMesh(meshA, moffset, mscalefactor);

		MeshClean meshCleaner(meshA);
		clock_t start, end;

		start = clock();
		FMeshData out = meshCleaner.Clean();
		end = clock();	printf("time:%.2f ms\n", (FFLOAT)(end - start));

		TransformMesh(out, FVec3() - moffset, mscale);
		if (writeVtk(outputDir, out.m_Vertices, out.m_Triangles))
			printf("write success!-----------------\n");
	}
}
