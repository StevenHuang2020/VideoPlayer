#ifndef QIMAGE_CONVERT_MAT_H
#define QIMAGE_CONVERT_MAT_H
#include <QImage>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


void qimage_to_mat(const QImage& image, cv::OutputArray& out);
void mat_to_qimage(cv::InputArray& image, QImage& out);
QString print_mat(const char* name, const cv::Mat& M);
#endif