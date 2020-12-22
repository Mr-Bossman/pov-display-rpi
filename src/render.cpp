#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>
#include "common.h"
#include "render.h"
using namespace std;

static uint64_t nanos()
{
	struct timespec now;
	timespec_get(&now, TIME_UTC);
	return ((uint64_t)now.tv_sec) * 1000000000 + ((uint64_t)now.tv_nsec);
}
static cv::Mat fit(const cv::Mat &img)
{
	cv::Mat out;
	int width = img.cols;
	int height = img.rows;
	double size, width_small, height_small;
	if (width >= height)
	{
		size = width_small = (rings * 2) / sqrt(2);
		height_small = width_small * ((double)height / (double)width);
	}
	else
	{
		size = height_small = (rings * 2) / sqrt(2);
		width_small = height * (width / (double)height);
	}
	cv::resize(img, out, cv::Size((int)width_small, (int)height_small));
	int top = (size - height_small) / 2;
	int down = (size - height_small + 1) / 2;
	int left = (size - width_small) / 2;
	int right = (size - width_small + 1) / 2;
	cv::copyMakeBorder(out, out, top, down, left, right, cv::BORDER_CONSTANT,
					   cv::Scalar(0, 0, 0));
	return out;
}
static cv::Mat crop(const cv::Mat &img)
{
	cv::Mat out;
	int width = img.cols;
	int height = img.rows;
	cv::Rect crop;
	if (width >= height)
	{
		resize(img, out,
			   cv::Size(((double)rings * 2 * width) / (double)height,
						rings * 2));
		int mid = (out.cols - (rings * 2)) / 2;
		crop = cv::Rect(mid, 0, rings * 2, rings * 2);
	}
	else
	{
		resize(img, out,
			   cv::Size(rings * 2,
						((double)rings * 2 * height) / (double)width));
		int mid = (out.rows - (rings * 2)) / 2;
		crop = cv::Rect(0, mid, rings * 2, rings * 2);
	}
	return out(crop);
}
static double to_rad(double degree)
{
	double pi = 3.14159265359;
	return (degree * (pi / 180));
}

extern int render16(char *argv, bool *go, uint16_t buffer[3][degreesIn][rings], uint64_t fps, bool *swap, bool fits)
{
	cv::VideoCapture cap(argv);
	uint64_t last = 0;
	uint64_t delay = 1000000000 / fps;
	int p = 2;
	if (!cap.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	while (*go)
	{

		cv::Mat frame;
		cap >> frame;
		if (frame.empty())
			break;
		if (fits)
			frame = crop(frame);
		else
			frame = fit(frame);

		while (last + delay > nanos())
			;
		last = nanos();
		for (int d = 0; d < degreesIn; d++)
		{
			for (int radius = 0; radius < rings; radius++)
			{
				int y = -(int)round(
					sin(to_rad((double)d / (degreesIn / 360))) * radius);
				int x = -(int)round(
					cos(to_rad((double)d / (degreesIn / 360))) * radius);
				if (abs(x) >= frame.cols / 2 || abs(y) >= frame.rows / 2)
					buffer[p][d][radius] = 0;
				else
				{
					cv::Vec3b color = frame.at<cv::Vec3b>(x + frame.cols / 2, y + frame.rows / 2);
					buffer[p][d][radius] = ((((uint16_t)color[0]) + ((uint16_t)color[0]) + ((uint16_t)color[0])) / 3ul) << 8;
				}
			}
		}
		if (*swap)
		{
			if (p == 2)
				p = 0;
			else
				p++;
			*swap = false;
		}
	}

	cap.release();
	cv::destroyAllWindows();

	return 0;
}











extern int render(char *argv, bool *go, cv::Vec3b buffer[degreesIn][rings], bool fits)
{
	cv::VideoCapture cap(argv);

	if (!cap.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	while (*go)
	{

		cv::Mat frame;
		cap >> frame;
		if (frame.empty())
			break;
		if (fits)
			frame = crop(frame);
		else
			frame = fit(frame);

		for (int d = 0; d < degreesIn; d++)
		{
			for (int radius = 0; radius < rings; radius++)
			{
				int y = -(int)round(
					sin(to_rad((double)d / (degreesIn / 360))) * radius);
				int x = -(int)round(
					cos(to_rad((double)d / (degreesIn / 360))) * radius);
				if (abs(x) >= frame.cols / 2 || abs(y) >= frame.rows / 2)
					buffer[d][radius] = 0;
				else
					buffer[d][radius] = frame.at<cv::Vec3b>(x + frame.cols / 2, y + frame.rows / 2);
			}
		}
		/*
        Mat image = Mat(rings * 2, rings * 2, frame.type(), double(0));
        for (int d = 0; d < degreesIn; d++) {
			for (int radius = 0; radius < rings; radius++) {
				int y = (int) round(
						sin(to_rad((double) d / (degreesIn / 360))) * radius);
				int x = -(int) round(
						cos(to_rad((double) d / (degreesIn / 360))) * radius);
				image.at < cv::Vec3b
						> (Point(x + image.cols / 2, y + image.rows / 2)) =
						buffer[d][radius];
			}
		}*/
		/*for (int d = 0; d < degreesIn; d++) { // draw circumfince in white
			int y = (int) round(sin(to_rad((double) d / (degreesIn / 360))) * rings);
			int x = -(int) round(cos(to_rad((double) d / (degreesIn / 360))) * rings);
			image.at < cv::Vec3b > (Point(x + image.cols / 2, y + image.rows / 2)) =
					cv::Vec3b(255, 255, 255);
		}*/

		/*resize(image, image, Size(), 500 / (rings * 2), 500 / (rings * 2));
		imshow("Frame", image);

		char c = (char) waitKey(25);
		if (c == 27)
			break;*/
	}

	cap.release();
	cv::destroyAllWindows();

	return 0;
}