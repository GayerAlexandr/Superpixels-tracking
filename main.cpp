#pragma optimize( "", off )

#include "slic.h"
#include "functions.h"
#include "graph_build.h"

//#include <opencv/highgui.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <float.h>
#include <iostream>///убрать потом
using namespace std;

int main() {

	/*
	Graphs for cur. frame, prev frame and for connectivity between these 2 frames
	*/
	vec3di cur_graph, prev_graph;
	vec3di global_graph;


	const char *filename = "Resources\\test.avi";
	// creating window
	cvNamedWindow("Video Tracking", CV_WINDOW_AUTOSIZE);
	CvCapture *capture;
	capture = cvCreateFileCapture(filename);
	// number of frames of video
					int frames = (int)cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
	//mask of first frame with object
					cv::Mat mask = cv::imread("Resources\\mask_binary.png", CV_LOAD_IMAGE_GRAYSCALE);
	//cur. and prev. frames for their comparing
					IplImage *cur_image = NULL;
					IplImage *prev_image = NULL;
	// Superpixels map for cur. and prev. frames
					Slic cur_slic, prev_slic;
	//vector with centers of seperpixels, which belong to object 
					vector<CvPoint> obj_centers, prev_obj_centers;

	bool first_frame = true;

	while (1)
	{
		int count = 1;
		// getting next frame
		IplImage *frame = cvQueryFrame(capture);

		if (!frame)
			break;

		/* Load the image and convert to Lab colour space. */
		cur_image = frame;

		IplImage *out_image = cvCloneImage(cur_image);
		IplImage *lab_image = cvCloneImage(cur_image);
		cvCvtColor(cur_image, lab_image, CV_BGR2Lab);

		/* Yield the number of superpixels and weight-factors from the user. */
		int w = cur_image->width, h = cur_image->height;
		int nr_superpixels = 5200;//atoi(argv[2]);
		int nc = 500;//atoi(argv[3]);

		double step = sqrt((w * h) / (double)nr_superpixels);

		/* Perform the SLIC superpixel algorithm. */
		cur_slic.generate_superpixels(lab_image, (int)step, nc);
		cur_slic.create_connectivity(lab_image);

		/* Display the contours and show the result. */
		//cur_slic.colour_with_cluster_means(out_image);
		cur_slic.display_contours(out_image, CV_RGB(255, 0, 0));
		cur_slic.colour_with_cluster_means(cur_image);/////////////////////////!!!!!!!!!!!!!!!!!!!!!!/////////////
		//cur_slic.colour_with_cluster_means(out_image);
		//cur_slic.display_center_grid(out_image, CV_RGB(0, 0, 255));

		if (first_frame)
		{
			//save the list of centers of the clusters belonging to the object
			for (int i = 0; i < cur_slic.centers.size(); i++)
			{
				if (mask.at<uchar>(cvPoint((int)cur_slic.centers[i][3], (int)cur_slic.centers[i][4])) != 255)
				{
					obj_centers.push_back(cvPoint((int)cur_slic.centers[i][3], (int)cur_slic.centers[i][4]));
				}
			}

			/* Iniatilize graph of object */
			for (int i = 0; i < obj_centers.size(); i++)
			{
				vector<vector<double>> temp; /*in future push it in graph*/

				vector<CvPoint> vec_neighbors = cur_slic.find_neighbors(cur_image, obj_centers[i].x, obj_centers[i].y);
				for (int j = 0; j < vec_neighbors.size(); j++)
				{
					vector<double> neighbor;
					/* pushing label of first vertex */
					neighbor.push_back(cur_slic.clusters[obj_centers[i].x][obj_centers[i].y]);
					/* pushing label of sec vertex */
					neighbor.push_back(cur_slic.clusters[vec_neighbors[j].x][vec_neighbors[j].y]);
					/* pushing dist between first and sec vertex*/
					double dist = functions::graph_computeDist( 
						cur_image, 
						cur_slic,
						CvPoint(obj_centers[i].x, obj_centers[i].y),
						CvPoint(vec_neighbors[j].x, vec_neighbors[j].y)
						);

					neighbor.push_back(dist);
					temp.push_back(neighbor);
				}
				cur_graph.push_back(temp);
			}
			//rendering centers of superpixels which belong to the object
			for each (CvPoint var in obj_centers)
			{
				cvCircle(out_image, var, 0, CV_RGB(0, 0, 255), 2);
			}
			cvSaveImage("test1.png", cur_image);
			first_frame = false;
		}
		else {
			 /*obtainig graph of supposed region with object*/
			 cur_graph = graph_build::graph_of_region_with_object(
														prev_obj_centers,
														cur_image,
														out_image,
														cur_slic,
														cur_graph
														);

			 /* Obtaining of global graph (between 2 frames) */
			 global_graph = graph_build::global_graph(
										prev_obj_centers,
										cur_image,
										prev_image,
										cur_slic,
										prev_slic,
										cur_graph,
										prev_graph,
										global_graph
										);


			int min_j = 0;
			/* Remove arcs from global graph */
			for (int i = 0; i < global_graph.size(); i++)
			{
				double min = INT32_MAX;
				for (int j = 0; j < global_graph[i].size(); j++)
				{
					if (global_graph[i][j][2] < min)
					{
						min = global_graph[i][j][2];
						min_j = j;
					}
				}
				vector<double> tempP;

				if (!global_graph[i].empty())
				{

					for (int k = 0; k < global_graph[i][min_j].size(); k++)
					{
						tempP.push_back(global_graph[i][min_j][k]);
					}
					if (global_graph[i][min_j].size() != 0)
					{
						global_graph[i].clear();
						global_graph[i].push_back(tempP);
					}
				}
			}
			
			/*enter new object centers in vector*/
			for (int i = 0; i < global_graph.size(); i++)
			{
				for (int j = 0; j < global_graph[i].size(); j++)
				{
					CvPoint cp = cur_slic.label_map[(int)global_graph[i][j][0]];
					obj_centers.push_back(cp);
				}
			}


			for each (CvPoint var in obj_centers)
			{
				cvCircle(out_image, var, 0, CV_RGB(0, 0, 255), 2);
			}
		}

		// show frame
		cvShowImage("Video Tracking", out_image);

		prev_graph = cur_graph;
		prev_image = cvCloneImage(cur_image);
		prev_slic = cur_slic;
		prev_obj_centers = obj_centers;

		obj_centers.clear();
		global_graph.clear();
		cur_graph.clear();
		cur_slic.~Slic();

		char c = cvWaitKey(33); // exit by esc
		if (c == 27)
			break;
	}

	cvReleaseCapture(&capture);
	cvDestroyWindow("Video Tracking");

	return 0;
}
