#pragma once

#include "Scene_wireframe_object.h"
#include <vector>
#include "Mesh.h"

class Streamsurface : public Scene_wireframe_object
{
public:

	//Streamsurface::Streamsurface();
	void add_point_strip(std::vector<Scene_vertex_t> &points, float time);
	int length;


private:

	std::vector<float> time_stamp_;

};