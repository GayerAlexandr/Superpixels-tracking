#include "slic.h"
#include "graph_build.h"
#include "functions.h"


/*obtainig graph of supposed region with object*/
vec3di graph_build::graph_of_region_with_object(
	vector<CvPoint> &prev_obj_centers,
	IplImage* cur_image,
	IplImage* out_image,
	Slic &cur_slic,
	vec3di &cur_graph
	)
{
	int w = cur_image->width, h = cur_image->height;
	double step = sqrt((w * h) / (double)cur_slic.centers.size());

	const int c_increase = 20; /*constant for model of motion; how much should be moved bound of region, which presumably contain object*/
	int x_left(0), x_right(0), y_up(0), y_down(0);
	CvPoint min_element = functions::min_elements(prev_obj_centers);
	CvPoint max_element = functions::max_elements(prev_obj_centers);

	x_left = min_element.x - c_increase;
	if (x_left < 0) x_left = 0;
	x_right = max_element.x + c_increase;
	if (x_right > cur_image->width) x_right = cur_image->width;
	y_up = min_element.y - c_increase;
	if (y_up < 0) y_up = 0;
	y_down = max_element.y + c_increase;
	if (y_down > cur_image->height) y_down = cur_image->height;



	for (int y = y_up; y < y_down; y += step / 2)
	{
		for (int x = x_left; x < x_right; x += step / 2)
		{
			bool exist = false;
			//for (int i = 0; i < cur_graph.size(); i++)
			//{
			//	for (int j = 0; j < cur_graph[i].size(); j++)///////////////////////////////// неправ. проверка на вхождение
			//	{
			//		if (cur_graph[i][j][0] == cur_slic.clusters[x][y])
			//		{
			//			exist = true;
			//			break;
			//		}
			//	}
			//	if (exist)
			//		break;
			//}

			if (!exist)
			{
				vector<vector<double>> temp; /*in future push it in graph*/

				vector<CvPoint> vec_neighbors = cur_slic.find_neighbors(cur_image, x, y);
				for (int j = 0; j < vec_neighbors.size(); j++)
				{
					vector<double> neighbor;
					/* pushing label of first vertex */
					neighbor.push_back(cur_slic.clusters[x][y]);
					/* pushing label of sec vertex */
					neighbor.push_back(cur_slic.clusters[vec_neighbors[j].x][vec_neighbors[j].y]);
					/* pushing dist between first and sec vertex*/
					double dist = functions::graph_computeDist(
						cur_image, 
						cur_slic,
						CvPoint(x,y),
						CvPoint(vec_neighbors[j].x, vec_neighbors[j].y)
						);
					neighbor.push_back(dist);

					temp.push_back(neighbor);
				}
				cur_graph.push_back(temp);
			}
		}
	}
	return cur_graph;
}

/* Obtaining of global graph (between 2 frames) */
vec3di graph_build::global_graph(
	vector<CvPoint> &prev_obj_centers,
	IplImage* cur_image,
	IplImage* prev_image,
	Slic &cur_slic,
	Slic &prev_slic,
	vec3di &cur_graph,
	vec3di &prev_graph,
	vec3di global_graph
	) 
{
	int w = cur_image->width, h = cur_image->height;
	double step = sqrt((w * h) / (double)cur_slic.centers.size());

	for (int i = 0; i < prev_obj_centers.size(); i++)
	{
		/*creating a region 3*step X 3*step  */
		int x_left(prev_obj_centers[i].x),
			x_right(prev_obj_centers[i].x),
			y_up(prev_obj_centers[i].y),
			y_down(prev_obj_centers[i].y);
		if (x_left - 3 * step / 2 >= 0)
			x_left -= 3 * step / 2;
		else
			x_left = 0;

		if (x_right + 3 * step / 2 < cur_image->width)
			x_right += 3 * step / 2;
		else
			x_right = cur_image->width;

		if (y_up - 3 * step / 2 >= 0)
			y_up -= 3 * step / 2;
		else
			y_up = 0;

		if (y_down + 3 * step / 2 < cur_image->height)
			y_down += 3 * step / 2;
		else
			y_down = cur_image->height;


		vector<vector<double>> temp; /*in future push it in graph*/

		for (int y = y_up; y <= y_down; y += step / 2)
		{
			for (int x = x_left; x <= x_right; x += step / 2)
			{
				////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool exist = false;
				//for (int i = 0; i <global_graph.size(); i++)
				//{
				//	for (int j = 0; j < global_graph[i].size(); j++)///////////////////////////////// неправ. проверка на вхождение
				//	{
				//		if (global_graph[i][j][1] == cur_slic.clusters[x][y])
				//		{
				//			exist = true;
				//			break;
				//		}
				//	}
				//	if (exist)
				//		break;
				//}

				if (!exist)
				{
					vector<double> temp1;

					/* pushing label of first vertex */
					temp1.push_back(prev_slic.clusters[prev_obj_centers[i].x][prev_obj_centers[i].y]);
					/* pushing label of sec vertex */
					temp1.push_back(cur_slic.clusters[x][y]);
					/* pushing dist between first and sec vertex*/
					double dist = functions::global_graph_computeFramesDist(
						prev_image,
						cur_image,
						prev_graph,
						cur_graph,
						prev_slic,
						cur_slic,
						CvPoint(prev_obj_centers[i].x, prev_obj_centers[i].x),
						CvPoint(x,y)
						);
					temp1.push_back(dist);

					temp.push_back(temp1);
				}
			}
		}
		global_graph.push_back(temp);
	}
	return global_graph;
}