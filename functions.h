#pragma once
#ifndef FUNCTIONS_H
#define FUNCTIONS_H 

#include "slic.h"

class functions {
public:
	/*Compute distance between two supepixels in local graph*/
	static double graph_computeDist(
		IplImage* image, 
		Slic slic, 
		CvPoint p1, 
		CvPoint p2);

	/*Compute distance between two supepixels in global graph*/
	static double global_graph_computeFramesDist(
		IplImage* prev_image,
		IplImage* cur_image,
		vec3di prev_graph,
		vec3di cur_graph,
		Slic prev_slic,
		Slic cur_slic,
		CvPoint p1, /*point by prev. frame*/
		CvPoint p2); /*point by cur. frame*/

	/*Finding min and max elements {x, y} in vector<CvPoint>*/
	static CvPoint min_elements(vector<CvPoint> vec);
	static CvPoint max_elements(vector<CvPoint> vec);
};


#endif