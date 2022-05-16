/*******************************************
 * Copyright (c) Steven Huang
 *
 * Images basic operations by using OpenCV 4.5.5
 *
 *
 ******************************************/


#ifndef H_IMAGECV_OPERATIONS_H
#define H_IMAGECV_OPERATIONS_H

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

const Mat load_img(const char* file, int flag = cv::IMREAD_COLOR);
const Mat load_grey(const char* file);
void show_img(const Mat& img, const char* windowName = "name");
void write_img(const Mat& img, const char* file);
string info_img(const Mat& img);
string type2str(int type);
int img_channel(const Mat& img);
int img_depth(const Mat& img);
int img_type(const Mat& img);
int img_w(const Mat& img);
int img_h(const Mat& img);

const Mat& img2bgr(const Mat& img);
const Mat& img2rgb(const Mat& img);
const Mat grey_img(const Mat& img);
const Mat resize_img(const Mat& img, int new_w, int new_h);
const Mat diff_imgs(const Mat& img1, const Mat& img2);
double distance_imgs(const Mat& img1, const Mat& img2);
double psnr_img(const Mat& img1, const Mat& img2);
const Mat flip_img(const Mat& img, int flipCode = 1);
const Mat rotate_img(const Mat& img, int rotateCode = ROTATE_90_CLOCKWISE);
const Mat repeat_img(const Mat& img, int ny, int nx);
const Mat histgram_img(const Mat& img);
const Mat& equalized_hist_img(const Mat& img);
const Mat threshold_img(const Mat& img, int thresh = 127, int type = THRESH_BINARY | THRESH_OTSU);
const Mat thresholdAdaptive_img(const Mat& img, int adaptiveMethod = cv::ADAPTIVE_THRESH_MEAN_C,
	int thresholdType = cv::THRESH_BINARY, int blockSize = 11, double C = 2);
const Mat covert_color_img(const Mat& img, int format = COLOR_BGR2GRAY);
const Mat filter_img(const Mat& img, const Mat& kernel);
const Mat& reverse_img(const Mat& img);
const Mat normalize_img(const Mat& img, double alpha = 1, double beta = 0,
	int norm_type = NORM_L2);

const Mat& scane_img_colortable(Mat& I, const uchar* const table);
void gen_color_table(uchar* const p, int size = 256, int divideWith = 10);
const Mat& scane_img_colortable2(Mat& I, const uchar* const table);
const Mat& scane_img_LUT(Mat& I, const Mat& table);

const Mat& lighter_img(Mat& I, float ratio = 1.0);
const Mat& exposure_img(Mat& I, uchar thresh = 50);

const Mat& gamma_img(Mat& I, float gamma = 1 / 2.2);
const Mat& contrast_bright_img(Mat& I, double alpha = 1.0, int beta = 0);

/*********************smooth images******************/
const Mat blur_img(const Mat& I, BlurType smoothType = GAUSSIAN, int ksize = 5, int sigma = 3);

/*************edge detection filters******************/
const Mat canny_img(const Mat& I, double threshold1 = 50, double threshold2 = 200);
const Mat sobel_img(const Mat& I, bool borizontal = true, int ksize = 3, double scale = 1, double delta = 0);
const Mat sobel_img_XY(const Mat& I, int ksize = 3, double scale = 1, double delta = 0);
const Mat laplacian_img(const Mat& I, int sigma = 3);
const Mat scharr_img(const Mat& I, bool borizontal = true);
const Mat scharr_img_XY(const Mat& I);
const Mat prewitt_img(const Mat& I, bool borizontal = true);
const Mat prewitt_img_XY(const Mat& I);

/*************face haar cascade detection *************/
const Mat& face_detect(Mat& img, const char* haarXML, double scale = 1);

#endif /* end of H_IMAGECV_OPERATIONS_H*/

//https://docs.opencv.org/4.x/d9/df8/tutorial_root.html