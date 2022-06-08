
#include "Streamsurface.h"

//Streamsurface::Streamsurface() {}

void Streamsurface::add_point_strip(std::vector<Scene_vertex_t> &points, float time) {

	//first vertex strip
	if (get_vertices().empty()) {

		length = points.size();

		for (int i = 0; i < points.size(); i++) {
			auto current = points.at(i);
			get_vertices().push_back(current);
			if (i > 0) {
				add_edge(Scene_wireframe_edge(	get_vertices().size() - 2,
												get_vertices().size() - 1,
												Color(0, 0, 0, 255)));
			}
		}
	}
	else//later vertex strips
	{
		assert(points.size() == length);

		for (int i = 0; i < points.size(); i++) {
			auto current = points.at(i);
			get_vertices().push_back(current);

			add_edge(Scene_wireframe_edge(get_vertices().size() - length -1,
				get_vertices().size() - 1,
				Color(0, 0, 0, 255)));
			add_edge(Scene_wireframe_edge(get_vertices().size() - length,
				get_vertices().size() - 1,
				Color(0, 0, 0, 255)));

			//after first vertex in strip
			if (i > 0) {
				add_edge(Scene_wireframe_edge(	get_vertices().size() - 2,
												get_vertices().size() - 1,
												Color(0, 0, 0, 255)));


			}
		}
	}

	time_stamp_.push_back(time);
}


