#include "FBoundingBox.h"
#include <algorithm>
#include <unordered_set>
bool FTriangle::operator ==(const FTriangle& triangle) const {

	return this->box.m_Min == triangle.box.m_Min && this->box.m_Max == triangle.box.m_Max && this->center == triangle.center;
}

FBoundingBox::FBoundingBox(const std::vector<FVec3>& points) {
	m_Min = { FLOAT_MAX,FLOAT_MAX,FLOAT_MAX };
	m_Max = { FLOAT_MIN,FLOAT_MIN,FLOAT_MIN};
	
	for (auto& point : points) {
		m_Max = m_Max.Ceiling(point);
		m_Min = m_Min.Floor(point);
	}
	m_Size = m_Max - m_Min;
	m_Center = (m_Max + m_Min / 2);
}

FBoundingBox::FBoundingBox(const std::vector<FVertex>& points) {
	m_Min = { FLOAT_MAX,FLOAT_MAX,FLOAT_MAX };
	m_Max = { FLOAT_MIN,FLOAT_MIN,FLOAT_MIN };
	for (auto& point : points) {
		m_Max = m_Max.Ceiling(point.position);
		m_Min = m_Min.Floor(point.position);
	}
	m_Size = m_Max - m_Min;
	m_Center = (m_Max + m_Min / 2);
}

FBoundingBox::FBoundingBox(FVec3& v0, FVec3& v1, FVec3& v2) {
	m_Min = { FLOAT_MAX,FLOAT_MAX,FLOAT_MAX };
	m_Max = { FLOAT_MIN,FLOAT_MIN,FLOAT_MIN };

	m_Max = m_Max.Ceiling(v0);
	m_Min = m_Min.Floor(v0);
	m_Max = m_Max.Ceiling(v1);
	m_Min = m_Min.Floor(v1);
	m_Max = m_Max.Ceiling(v2);
	m_Min = m_Min.Floor(v2);

	m_Size = m_Max - m_Min;
	m_Center = (m_Max + m_Min / 2);
}

FBoundingBox::FBoundingBox()
{
	m_Min = { FLOAT_MAX,FLOAT_MAX,FLOAT_MAX };
	m_Max = { FLOAT_MIN,FLOAT_MIN,FLOAT_MIN };

}

FBoundingBox::~FBoundingBox() {
	
}

bool FBoundingBox::Intersection(FBoundingBox& box) {
	return WeakBoundingBoxIntersection(*this,box);
}

void FBoundingBox::Merge(FBoundingBox& box) {
	m_Max = m_Max.Ceiling(box.m_Max);
	m_Min = m_Min.Floor(box.m_Min);

	m_Size = m_Max - m_Min;
	m_Center = (m_Max + m_Min / 2);
}

void FBoundingBox::Include(FVec3& position)
{
	m_Max = m_Max.Ceiling(position);
	m_Min = m_Min.Floor(position);

	m_Size = m_Max - m_Min;
	m_Center = (m_Max + m_Min / 2);
}

bool FBoundingBox::isContain(const FVec3& v)
{
	if (std::max(v.X, m_Min.X) > std::min(v.X, m_Max.X) + FLOAT_EPSILON)
		return false;
	if (std::max(v.Y, m_Min.Y) > std::min(v.Y, m_Max.Y) + FLOAT_EPSILON)
		return false;
	if (std::max(v.Z, m_Min.Z) > std::min(v.Z, m_Max.Z) + FLOAT_EPSILON)
		return false;
	return true;
}
