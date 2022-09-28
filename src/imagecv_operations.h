/*******************************************
 * Copyright (c) Steven Huang
 *
 * Images basic operations by using OpenCV 4.5.5
 *
 *
 ******************************************/


#ifndef __IMAGECV_OPERATIONS_H__
#define __IMAGECV_OPERATIONS_H__

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/core/types_c.h>

#include <iostream>
#include <cmath>

using namespace std;
using namespace cv;


enum BlurType { GAUSSIAN, BLUR, MEDIAN };

Mat load_img(const char* file, int flag = cv::IMREAD_COLOR);
Mat load_grey(const char* file);
void show_img(const Mat& img, const char* windowName = "");
void write_img(const Mat& img, const char* file);
string info_img(const Mat& img);
string type2str(int type);
int img_channel(const Mat& img);
int img_depth(const Mat& img);
int img_type(const Mat& img);
int img_w(const Mat& img);
int img_h(const Mat& img);

Mat img_swap_channel(const Mat& img);
Mat img2bgr(const Mat& img);
Mat img2rgb(const Mat& img);
Mat grey_img(const Mat& img);
Mat resize_img(const Mat& img, int new_w, int new_h);
Mat diff_imgs(const Mat& img1, const Mat& img2);
double distance_imgs(const Mat& img1, const Mat& img2);
double psnr_img(const Mat& img1, const Mat& img2);
Mat flip_img(const Mat& img, int flipCode = 1);
Mat rotate_img(const Mat& img, int rotateCode = ROTATE_90_CLOCKWISE);
Mat repeat_img(const Mat& img, int ny, int nx);
Mat histgram_img(const Mat& img);

void equalized_hist_img(Mat& img);
void reverse_img(Mat& img);
void lighter_img(Mat& I, float ratio = 1.0);
void exposure_img(Mat& I, uchar thresh = 50);

void scane_img_colortable(Mat& I, const uchar* const table);
void scane_img_colortable2(Mat& I, const uchar* const table);
void scane_img_LUT(Mat& I, const Mat& table);
void gamma_img(Mat& I, float gamma = 1 / 2.2);
void contrast_bright_img(Mat& I, double alpha = 1.0, int beta = 0);

Mat threshold_img(const Mat& img, int thresh = 127, int type = THRESH_BINARY | THRESH_OTSU);
Mat thresholdAdaptive_img(const Mat& img, int adaptiveMethod = cv::ADAPTIVE_THRESH_MEAN_C,
	int thresholdType = cv::THRESH_BINARY, int blockSize = 11, double C = 2);
Mat covert_color_img(const Mat& img, int format = COLOR_BGR2GRAY);
Mat filter_img(const Mat& img, const Mat& kernel);

Mat normalize_img(const Mat& img, double alpha = 1, double beta = 0,
	int norm_type = NORM_L2);

void gen_color_table(uchar* const p, int size = 256, int divideWith = 10);



/*********************smooth images******************/
Mat blur_img(const Mat& I, BlurType smoothType = GAUSSIAN, int ksize = 5, int sigma = 3);

/*************edge detection filters******************/
Mat canny_img(const Mat& I, double threshold1 = 50, double threshold2 = 200);
Mat sobel_img(const Mat& I, bool borizontal = true, int ksize = 3, double scale = 1, double delta = 0);
Mat sobel_img_XY(const Mat& I, int ksize = 3, double scale = 1, double delta = 0);
Mat laplacian_img(const Mat& I);
Mat scharr_img(const Mat& I, bool borizontal = true);
Mat scharr_img_XY(const Mat& I);
Mat prewitt_img(const Mat& I, bool borizontal = true);
Mat prewitt_img_XY(const Mat& I);

/*************2D Features framework********************/
Mat cornerHarris(const Mat& img);

/*************face haar cascade detection *************/
void face_detect(Mat& img, const char* haarXML, double scale = 1);

#endif /* end of __IMAGECV_OPERATIONS_H__*/

//https://docs.opencv.org/4.x/d9/df8/tutorial_root.html