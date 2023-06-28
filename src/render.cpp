#include "opencv2/opencv.hpp"
#include <iostream>
#include <cmath>
#include "common.h"
#include "render.h"

static uint64_t nanos() {
	struct timespec now;
	timespec_get(&now, TIME_UTC);
	return ((uint64_t)now.tv_sec) * 1000000000 + ((uint64_t)now.tv_nsec);
}

static cv::Mat fit(const cv::Mat &img) {
	cv::Mat out;
	int width = img.cols;
	int height = img.rows;
	double size, width_small, height_small;
	if (width >= height) {
		size = width_small = (RINGS * 2) / sqrt(2);
		height_small = width_small * ((double)height / (double)width);
	} else {
		size = height_small = (RINGS * 2) / sqrt(2);
		width_small = height * (width / (double)height);
	}
	cv::resize(img, out, cv::Size((int)width_small, (int)height_small));
	int top = (size - height_small) / 2;
	int down = (size - height_small + 1) / 2;
	int left = (size - width_small) / 2;
	int right = (size - width_small + 1) / 2;
	cv::copyMakeBorder(out, out, top, down, left, right, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
	return out;
}

static cv::Mat crop(const cv::Mat &img) {
	cv::Mat out;
	int width = img.cols;
	int height = img.rows;
	cv::Rect crop;
	if (width >= height) {
		resize(img, out, cv::Size(((double)RINGS * 2 * width) / (double)height, RINGS * 2));
		int mid = (out.cols - (RINGS * 2)) / 2;
		crop = cv::Rect(mid, 0, RINGS * 2, RINGS * 2);
	} else {
		resize(img, out, cv::Size(RINGS * 2, ((double)RINGS * 2 * height) / (double)width));
		int mid = (out.rows - (RINGS * 2)) / 2;
		crop = cv::Rect(0, mid, RINGS * 2, RINGS * 2);
	}
	return out(crop);
}

static inline double to_rad(double degree) {
	return (degree * (M_PI / 180));
}

static inline size_t rad_index(size_t deg, size_t rad) {
	return ((deg*RINGS) + rad);
}

static cv::Vec2i* compute_cartesian2polarLUT(void) {
	cv::Vec2i* C2P_LUT = new cv::Vec2i[DEGREESIN*RINGS];
	#pragma omp parallel for collapse(2)
	for (int d = 0; d < DEGREESIN; d++)
	for (int radius = 0; radius < RINGS; radius++) {
		int x = -(int)round(cos(to_rad((double)d / ((double)DEGREESIN / 360.0))) * radius);
		int y = -(int)round(sin(to_rad((double)d / ((double)DEGREESIN / 360.0))) * radius);
		C2P_LUT[rad_index(d, radius)] = cv::Vec2i(x, y);
	}
	return C2P_LUT;
}

static void fill_buffer(uint16_t buffer[3][DEGREESIN][RINGS],const cv::Mat &frame,int frame_buf_num,cv::Vec2i* C2P_LUT) {
	#pragma omp parallel for collapse(2)
	for (int d = 0; d < DEGREESIN; d++)
	for (int radius = 0; radius < RINGS; radius++) {
		auto tmp = C2P_LUT[rad_index(d, radius)];
		int x = tmp[0];
		int y = tmp[1];
		if (abs(x) >= frame.cols / 2 || abs(y) >= frame.rows / 2) {
			buffer[frame_buf_num][d][radius] = 0;
			continue;
		}
		cv::Vec3b color = frame.at<cv::Vec3b>(x + frame.cols / 2, y + frame.rows / 2)*radius;
		buffer[frame_buf_num][d][radius] = ((((uint32_t)color[0]) + ((uint32_t)color[1]) + ((uint32_t)color[2]))*(UINT16_MAX/UINT8_MAX)*radius)/((RINGS-1)*3);
	}
}

int render16(char *argv, bool *go, uint16_t buffer[3][DEGREESIN][RINGS], uint64_t fps, bool *swap, bool fits) {
	cv::VideoCapture cap(argv);
	uint64_t last = 0, i = 0, start;
	uint64_t delay = 1000000000 / fps;
	int frame_buf_num = 2;
	cv::Vec2i* C2P_LUT = nullptr;
	cv::Mat frame;

	start = nanos();

	if (!cap.isOpened()) {
		std::cout << "Error opening video stream or file" << std::endl;
		return -1;
	}

	while (*go) {
		i++;
		cap >> frame;
		if (frame.empty())
			break;
		if (fits)
			frame = crop(frame);
		else
			frame = fit(frame);
		if(C2P_LUT == nullptr)
			C2P_LUT = compute_cartesian2polarLUT();

		while (last + delay > nanos());
		last = nanos();
		fill_buffer(buffer, frame, frame_buf_num, C2P_LUT);
		if (*swap) {
			if (frame_buf_num == 2)
				frame_buf_num = 0;
			else
				frame_buf_num++;
			*swap = false;
		}
#ifdef DESKTOP_TEST
		std::cout << "\rfps " << ((double)i*1000000000.0)/((double)(nanos()-start))
		<< " fill time: " << nanos()-last << "ns    " << std::flush;
#else
(void)i;
(void)start;
#endif
	}
	delete[] C2P_LUT;
	cap.release();
	cv::destroyAllWindows();
	return 0;
}
