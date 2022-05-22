/*******************************************
 * Copyright (c) Steven Huang
 *
 * Images basic operations c++ interface
 * by using OpenCV 4.5.5
 *
 ******************************************/

#ifdef _DEBUG
#pragma comment(lib, "opencv_world455d.lib")
#else
#pragma comment(lib, "opencv_world455.lib")
#endif

#include "imagecv_operations.h"


const Mat load_img(const char* file, int flag) {
	return cv::imread(file, flag);
}

const Mat load_grey(const char* file) {
	return load_img(file, IMREAD_GRAYSCALE);
}

void show_img(const Mat& img, const char* windowName) {
	cv::imshow(windowName, img);
	waitKey(0);
	destroyAllWindows();
}

void write_img(const Mat& img, const char* file) {
	cv::imwrite(file, img);
}

string info_img(const Mat& img) {
	char buff[1024] = { 0 };
	snprintf(buff, sizeof(buff) - 1, "channel:%d, width:%d, height:%d, depth:%d, type:%d, typeStr:%s",
		img_channel(img), img_w(img), img_h(img), img_depth(img), img_type(img), type2str(img_type(img)).c_str());
	std::string str = buff;
	return str;
}

string type2str(int type) {
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  r = "8U"; break;
	case CV_8S:  r = "8S"; break;
	case CV_16U: r = "16U"; break;
	case CV_16S: r = "16S"; break;
	case CV_32S: r = "32S"; break;
	case CV_32F: r = "32F"; break;
	case CV_64F: r = "64F"; break;
	default:     r = "User"; break;
	}

	r += "C";
	r += (chans + '0');
	return r;
}

int img_depth(const Mat& img) {
	return img.depth();
}

int img_type(const Mat& img) {
	return img.type();
}

int img_channel(const Mat& img) {
	return img.channels();
}

int img_w(const Mat& img) {
	return img.cols;
}

int img_h(const Mat& img) {
	return img.rows;
}

const Mat& img2bgr(const Mat& img) {
	int channel = img_channel(img);
	if (channel >= 3) {
#if 1
		assert(channel <= 4); //max 4 image channels
		Mat rgb_channels[4];
		cv::split(img, rgb_channels);

		vector<Mat> channels;
		channels.push_back(rgb_channels[2]); //swap r,g,b <==> b,g,r
		channels.push_back(rgb_channels[1]);
		channels.push_back(rgb_channels[0]);
		for (int i = 3; i < channel; i++)
			channels.push_back(rgb_channels[i]);

		cv::merge(channels, img);
#else
		Mat* rgb_channels = new Mat[channel];
		cv::split(img, rgb_channels);

		vector<Mat> channels;
		channels.push_back(rgb_channels[2]); //swap r,g,b <==> b,g,r
		channels.push_back(rgb_channels[1]);
		channels.push_back(rgb_channels[0]);
		for (int i = 3; i < channel; i++)
			channels.push_back(rgb_channels[i]);

		cv::merge(channels, img);
		delete[] rgb_channels;
#endif
	}

	return img;
}

const Mat& img2rgb(const Mat& img) {
	return img2bgr(img);
}

const Mat grey_img(const Mat& img) {
	return covert_color_img(img, COLOR_BGR2GRAY);
}

const Mat resize_img(const Mat& img, int new_w, int new_h) {
	Size sz = Size(new_w, new_h);
	Mat res;

	cv::resize(img, res, sz);
	return res;
}

const Mat diff_imgs(const Mat& img1, const Mat& img2) {
	Mat res;
	cv::absdiff(img1, img2, res);
	return res;
}

double distance_imgs(const Mat& img1, const Mat& img2) {
	Mat res;
	Mat diff = diff_imgs(img1, img2);
	pow(diff, 2, res);

	double sum1 = cv::sum(res)[0];
	double sum2 = cv::sum(res)[1];
	double sum3 = cv::sum(res)[2];
	return sqrt((sum1 + sum2 + sum3) / 3);
}

double psnr_img(const Mat& img1, const Mat& img2) {
	return cv::PSNR(img1, img2, 255);
}

const Mat flip_img(const Mat& img, int flipCode) {
	Mat res;
	cv::flip(img, res, flipCode); //0, 1, -1
	return res;
}

const Mat rotate_img(const Mat& img, int rotateCode) {
	Mat res;  // Mat::zeros(img.size(), CV_8UC1);
	cv::rotate(img, res, rotateCode); //ROTATE_90_CLOCKWISE
	return res;
}

const Mat repeat_img(const Mat& img, int ny, int nx) {
	return cv::repeat(img, ny, nx);
}

const Mat histgram_img(const Mat& img) {
	vector<Mat> bgr_planes;
	split(img, bgr_planes);
	int histSize = 256;
	float range[] = { 0, 256 }; //the upper boundary is exclusive
	const float* histRange[] = { range };
	bool uniform = true, accumulate = false;
	Mat b_hist, g_hist, r_hist;
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, histRange, uniform, accumulate);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, histRange, uniform, accumulate);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, histRange, uniform, accumulate);
	int hist_w = 512, hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);
	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	for (int i = 1; i < histSize; i++) {
		cv::line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255, 0, 0), 2, 8, 0);
		cv::line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
			Scalar(0, 255, 0), 2, 8, 0);
		cv::line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
			Scalar(0, 0, 255), 2, 8, 0);
	}
	return histImage;
}

const Mat& equalized_hist_img(const Mat& img) {
	assert(img.channels() <= 4);
	Mat rbg_channel[4];
	cv::split(img, rbg_channel);

	vector<Mat> channels;
	for (int i = 0; i < img.channels(); i++) {
		Mat channel;
		cv::equalizeHist(rbg_channel[i], channel);
		channels.push_back(channel);
	}

	cv::merge(channels, img);
	return img;
}

const Mat threshold_img(const Mat& img, int thresh, int type) {
	Mat grey = grey_img(img);
	Mat res;
	cv::threshold(grey, res, thresh, 255, type);
	return res;
}

const Mat thresholdAdaptive_img(const Mat& img, int adaptiveMethod,
	int thresholdType, int blockSize, double C) {
	Mat grey = grey_img(img);
	Mat res;
	cv::adaptiveThreshold(grey, res, 255, adaptiveMethod, thresholdType, blockSize, C);
	return res;
}

const Mat covert_color_img(const Mat& img, int format) {
	Mat res = img.clone();
	cv::cvtColor(img, res, format);
	return res;
}

const Mat filter_img(const Mat& img, const Mat& kernel) {
	Mat res = img.clone();
	cv::filter2D(img, res, -1, kernel);
	return res;
}

const Mat& reverse_img(const Mat& img) {
	cv::bitwise_not(img, img);
	return img;
}

const Mat normalize_img(const Mat& img, double alpha, double beta, int norm_type) {
	Mat res = img.clone();
	cv::normalize(img, res, alpha, beta, norm_type);
	return res;
}

const Mat& scane_img_colortable(Mat& I, const uchar* const table) {
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);
	int channels = I.channels();
	int nRows = I.rows;
	int nCols = I.cols * channels;
	if (I.isContinuous()) {
		nCols *= nRows;
		nRows = 1;
	}

	for (int i = 0; i < nRows; ++i) {
		uchar* p = I.ptr<uchar>(i);
		for (int j = 0; j < nCols; ++j) {
			p[j] = table[p[j]];
		}
	}
	return I;
}

void gen_color_table(uchar* const p, int size, int divideWith) {
	if (p && size > 0) {
		for (int i = 0; i < size; ++i)
			*(p + i) = (uchar)(divideWith * (i / divideWith));
	}
}

const Mat& scane_img_colortable2(Mat& I, const uchar* const table) {
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);
	const int channels = I.channels();
	switch (channels)
	{
	case 1:
	{
		MatIterator_<uchar> it, end;
		for (it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it)
			*it = table[*it];
		break;
	}
	case 3:
	{
		MatIterator_<Vec3b> it, end;
		for (it = I.begin<Vec3b>(), end = I.end<Vec3b>(); it != end; ++it)
		{
			(*it)[0] = table[(*it)[0]];
			(*it)[1] = table[(*it)[1]];
			(*it)[2] = table[(*it)[2]];
		}
	}
	}
	return I;
}

const Mat& scane_img_LUT(Mat& I, const Mat& table) {
	cv::LUT(I, table, I);
	return I;
}

const Mat& lighter_img(Mat& I, float ratio) {
	CV_Assert(I.depth() == CV_8U);
	int channels = I.channels();
	int nRows = I.rows;
	int nCols = I.cols * channels;
	if (I.isContinuous()) {
		nCols *= nRows;
		nRows = 1;
	}

	for (int i = 0; i < nRows; ++i) {
		uchar* p = I.ptr<uchar>(i);
		for (int j = 0; j < nCols; ++j) {
			int res = int(p[j] * ratio);
			res = res > 255 ? 255 : res;
			p[j] = (uchar)res;
		}
	}
	return I;
}

const Mat& exposure_img(Mat& I, uchar thresh) {
	CV_Assert(I.depth() == CV_8U);
	int channels = I.channels();
	int nRows = I.rows;
	int nCols = I.cols * channels;
	if (I.isContinuous()) {
		nCols *= nRows;
		nRows = 1;
	}

	for (int i = 0; i < nRows; ++i) {
		uchar* p = I.ptr<uchar>(i);
		for (int j = 0; j < nCols; ++j) {
			if (p[j] < thresh)
				p[j] = 255 - p[j];
		}
	}
	return I;
}

const Mat& gamma_img(Mat& I, float gamma) {
	Mat lookUpTable = cv::Mat(1, 256, CV_8UC1, Scalar(0, 0, 0));
	for (int i = 0; i < 256; i++) {
		lookUpTable.at<uchar>(0, i) = saturate_cast<uchar>(pow(i / 255.0, 1.0 / gamma) * 255.0);
	}

	cv::LUT(I, lookUpTable, I);
	return I;
}

const Mat& contrast_bright_img(Mat& I, double alpha, int beta) {
	//Mat res = Mat::zeros(I.size(), I.type());
#if 1
	convertScaleAbs(I, I, alpha, beta);
#elif 0
	CV_Assert(I.depth() == CV_8U);
	int channels = I.channels();
	int nRows = I.rows;
	int nCols = I.cols * channels;
	if (I.isContinuous()) {
		nCols *= nRows;
		nRows = 1;
	}

	for (int i = 0; i < nRows; ++i) {
		uchar* p = I.ptr<uchar>(i);
		for (int j = 0; j < nCols; ++j) {
			p[j] = saturate_cast<uchar>(alpha * p[j] + beta);
		}
	}
#else	
	for (int y = 0; y < I.rows; y++) {
		for (int x = 0; x < I.cols; x++) {
			for (int c = 0; c < I.channels(); c++) {
				I.at<Vec3b>(y, x)[c] =
					saturate_cast<uchar>(alpha * I.at<Vec3b>(y, x)[c] + beta);
			}
		}
	}
#endif
	return I;
}

const Mat canny_img(const Mat& I, double threshold1, double threshold2) {
	Mat edges;
	Mat grey = grey_img(I);
	cv::Canny(grey, edges, threshold1, threshold2);
	return edges;
}

const Mat sobel_img(const Mat& I, bool borizontal, int ksize, double scale, double delta) {
	Mat grad;
	Mat abs_grad;
	int ddepth = CV_16S;
	Mat grey = grey_img(I);

	if (borizontal) {
		cv::Sobel(grey, grad, ddepth, 1, 0, ksize, scale, delta, BORDER_DEFAULT);
	}
	else {
		cv::Sobel(grey, grad, ddepth, 0, 1, ksize, scale, delta, BORDER_DEFAULT);
	}
	convertScaleAbs(grad, abs_grad);
	return abs_grad;
}

const Mat sobel_img_XY(const Mat& I, int ksize, double scale, double delta) {
	Mat grad;
	Mat abs_grad_x = sobel_img(I, true, ksize, scale, delta);
	Mat abs_grad_y = sobel_img(I, false, ksize, scale, delta);

	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
	return grad;
}

const Mat scharr_img(const Mat& I, bool borizontal) {
	Mat grad, abs_grad;

	Mat grey = grey_img(I);
	if (borizontal) {
		Scharr(grey, grad, CV_16S, 1, 0);
	}
	else {
		Scharr(grey, grad, CV_16S, 0, 1);
	}

	convertScaleAbs(grad, abs_grad);
	return abs_grad;
}

const Mat scharr_img_XY(const Mat& I) {
	Mat grad;
	Mat abs_grad_x = scharr_img(I, true);
	Mat abs_grad_y = scharr_img(I, false);
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
	return grad;
}

const Mat prewitt_img(const Mat& I, bool borizontal) {
	Mat grey = grey_img(I);

	if (borizontal) {
		cv::Mat kernel({ 3, 3 }, { 1, 0, -1, 1, 0, -1, 1, 0, -1 });
		return filter_img(grey, kernel);
	}
	else {
		cv::Mat kernel({ 3, 3 }, { 1, 1, 1, 0, 0, 0, -1, -1, -1 });
		return filter_img(grey, kernel);
	}
}

const Mat prewitt_img_XY(const Mat& I) {
	Mat grad;
	Mat abs_grad_x = prewitt_img(I, true);
	Mat abs_grad_y = prewitt_img(I, false);
	cv::addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
	return grad;
}

const Mat laplacian_img(const Mat& I, int sigma) {
	Mat laplace, result;
	Mat grey = grey_img(I);
	cv::Laplacian(grey, laplace, CV_16S, 5);
	//cv::convertScaleAbs(laplace, result, (sigma + 1) * 0.25);
	cv::convertScaleAbs(laplace, result);
	return result;
}

const Mat blur_img(const Mat& I, BlurType smoothType, int ksize, int sigma) {
	ksize = ksize | 1; // ksize must be odd number
	Mat smoothed = I.clone();
	if (smoothType == GAUSSIAN) {
		cv::GaussianBlur(I, smoothed, Size(ksize, ksize), sigma, sigma);
	}
	else if (smoothType == BLUR) {
		cv::blur(I, smoothed, Size(ksize, ksize));
	}
	else {
		cv::medianBlur(I, smoothed, ksize);
	}

	return smoothed;
}

const Mat& face_detect(Mat& img, const char* haarXML, double scale) {
	vector<Rect> faces, faces2;
	Mat gray, smallImg;
	CascadeClassifier cascade;

	cascade.load(haarXML);
	if (cascade.empty())
		return img;

	cvtColor(img, gray, COLOR_BGR2GRAY); // Convert to Gray Scale
	double fx = 1 / scale;

	// Resize the Grayscale Image 
	resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR);
	equalizeHist(smallImg, smallImg);

	// Detect faces of different sizes using cascade classifier 
	cascade.detectMultiScale(smallImg, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	// Draw circles around the faces
	for (size_t i = 0; i < faces.size(); i++) {
		Rect r = faces[i];
		Mat smallImgROI;
		vector<Rect> nestedObjects;
		Point center;
		Scalar color = Scalar(255, 0, 0); // Color for Drawing tool
		int radius;

		double aspect_ratio = (double)r.width / r.height;
		if (0.75 < aspect_ratio && aspect_ratio < 1.3) {
			center.x = cvRound((r.x + r.width * 0.5) * scale);
			center.y = cvRound((r.y + r.height * 0.5) * scale);
			radius = cvRound((r.width + r.height) * 0.25 * scale);
			circle(img, center, radius, color, 3, 8, 0);
		}
		else {
			rectangle(img, cvPoint(cvRound(r.x * scale), cvRound(r.y * scale)),
				cvPoint(cvRound((r.x + r.width - 1) * scale),
					cvRound((r.y + r.height - 1) * scale)), color, 3, 8, 0);
		}
	}

	return img;
}
