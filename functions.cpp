#include "functions.h"
#include <iostream>

double functions::graph_computeDist(IplImage* image, Slic slic, CvPoint p1, CvPoint p2)
{
	double dist = 0;
	
	int p1_blue = CV_IMAGE_ELEM(image, uchar, p1.y, (p1.x * 3));
	int p1_green = CV_IMAGE_ELEM(image, uchar, p1.y, (p1.x * 3)+1);
	int p1_red = CV_IMAGE_ELEM(image, uchar, p1.y, (p1.x * 3)+2);
	
	int p2_blue = CV_IMAGE_ELEM(image, uchar, p2.y, (p2.x * 3));
	int p2_green = CV_IMAGE_ELEM(image, uchar, p2.y, (p2.x * 3) + 1);
	int p2_red = CV_IMAGE_ELEM(image, uchar, p2.y, (p2.x * 3) + 2);

	dist = abs(p1_blue - p2_blue) + abs(p1_green - p2_green) + abs(p1_red - p2_red);

	return dist;
}


double functions::global_graph_computeFramesDist(
	IplImage* prev_image, 
	IplImage* cur_image, 
	vec3di prev_graph, 
	vec3di cur_graph, 
	Slic prev_slic, 
	Slic cur_slic, 
	CvPoint p1, /*point by prev. frame*/
	CvPoint p2) /*point by cur. frame*/
{
	double dist = 0;

	int p1_blue = CV_IMAGE_ELEM(prev_image, uchar, p1.y, (p1.x * 3));
	int p1_green = CV_IMAGE_ELEM(prev_image, uchar, p1.y, (p1.x * 3) + 1);
	int p1_red = CV_IMAGE_ELEM(prev_image, uchar, p1.y, (p1.x * 3) + 2);

	int p2_blue = CV_IMAGE_ELEM(cur_image, uchar, p2.y, (p2.x * 3));
	int p2_green = CV_IMAGE_ELEM(cur_image, uchar, p2.y, (p2.x * 3) + 1);
	int p2_red = CV_IMAGE_ELEM(cur_image, uchar, p2.y, (p2.x * 3) + 2);

	int main_part = abs(p1_blue - p2_blue) + abs(p1_green - p2_green) + abs(p1_red - p2_red);
	
	vector<CvPoint> neighbors_cur_points = cur_slic.find_neighbors(cur_image, p1.x, p1.y);
	vector<CvPoint> neighbors_prev_points = cur_slic.find_neighbors(prev_image, p2.x, p2.y);

	vector<vector<int>> neighbors_cur;
	vector<vector<int>> neighbors_prev;

	for (int i = 0; i < neighbors_cur_points.size(); i++)
	{
		vector<int> temp;
		for (int j = 0; j < 3; j++)
		{
			temp.push_back(CV_IMAGE_ELEM(cur_image, uchar, neighbors_cur_points[i].y, neighbors_cur_points[i].x * 3 + j));
		}
		neighbors_cur.push_back(temp);
	}

	for (int i = 0; i < neighbors_prev_points.size(); i++)
	{
		vector<int> temp;
		for (int j = 0; j < 3; j++)
		{
			temp.push_back(CV_IMAGE_ELEM(prev_image, uchar, neighbors_prev_points[i].y, neighbors_prev_points[i].x * 3+ j));
		}
		neighbors_prev.push_back(temp);
	}
	
	int sec_part = 0;

	int num = 0;
	if (neighbors_cur.size() > neighbors_prev.size())
		num = neighbors_prev.size();
	else
		num = neighbors_cur.size();
	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			sec_part += abs(neighbors_cur[i][j] - neighbors_prev[i][j]);
		}
	}

	dist = main_part + sec_part;

	return dist;
}


CvPoint functions::min_elements(vector<CvPoint> vec)
{
	int x_min = INT32_MAX, y_min = INT32_MAX;
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i].x < x_min)
		{
			x_min = vec[i].x;
		}
		if (vec[i].y < y_min)
		{
			y_min = vec[i].y;
		}
	}
	return CvPoint(x_min, y_min);
}


CvPoint functions::max_elements(vector<CvPoint> vec)
{
	int x_max = 0, y_max = 0;
	for (int i = 0; i < vec.size(); i++)
	{
		if (vec[i].x > x_max)
		{
			x_max = vec[i].x;
		}
		if (vec[i].y > y_max)
		{
			y_max = vec[i].y;
		}
	}
	return CvPoint(x_max, y_max);
}