/*
*Class graph_build contains only static functions and they are used
*for initializing graphs on one frame in some region, and second one
*for graph between 2 frames (global graph)
*/

#pragma once
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
/*
*Graph realised as list of arcs (3d vector): graph[i][j][k]
* i - index for list of arcs for one vertex
* j - index in list of arcs for j-destination vertex
* k = {0, 1, 2} : 0 - vertex FROM
*				  1 - vertex TO
*				  2 - "distance" between these two vertexes
*/
#define vec3di vector<vector<vector<double>>>

class graph_build {
public:
	static vec3di graph_of_region_with_object(
		vector<CvPoint> &prev_obj_centers,
		IplImage* cur_image,
		IplImage* out_image,
		Slic &cur_slic,
		vec3di &cur_graph
		);
	static vec3di global_graph(
		vector<CvPoint> &prev_obj_centers,
		IplImage* cur_image,
		IplImage* prev_image,
		Slic &cur_slic,
		Slic &prev_slic,
		vec3di &cur_graph,
		vec3di &prev_graph,
		vec3di global_graph
		);
};




#endif