#include "qimage_convert_mat.h"

void qimage_to_mat(const QImage& image, cv::OutputArray& out) {

	switch (image.format()) {
	case QImage::Format_Invalid:
	{
		cv::Mat empty;
		empty.copyTo(out);
		break;
	}
	case QImage::Format_RGB32:
	{
		cv::Mat view(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		view.copyTo(out);
		break;
	}
	case QImage::Format_RGB888:
	{
		cv::Mat view(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(view, out, cv::COLOR_RGB2BGR);
		break;
	}
	default:
	{
		QImage conv = image.convertToFormat(QImage::Format_ARGB32);
		cv::Mat view(conv.height(), conv.width(), CV_8UC4, (void*)conv.constBits(), conv.bytesPerLine());
		view.copyTo(out);
		break;
	}
	}
}

void mat_to_qimage(cv::InputArray& image, QImage& out)
{
	int type = image.type();
	switch (type) {
	case CV_8UC4:
	{
		qDebug("image format:%d, CV_8UC4", type);

		cv::Mat view(image.getMat());
		QImage view2(view.data, view.cols, view.rows, (int)view.step[0], QImage::Format_ARGB32);
		out = view2.copy();
		break;
	}
	case CV_8UC3:
	{
		qDebug("image format:%d, CV_8UC3", type);

		cv::Mat mat;
		cvtColor(image, mat, cv::COLOR_BGR2BGRA); //COLOR_BGR2RGB doesn't behave so use RGBA
		QImage view(mat.data, mat.cols, mat.rows, (int)mat.step[0], QImage::Format_ARGB32);
		out = view.copy();
		break;
	}
	case CV_8UC1:
	{
		qDebug("image format:%d, CV_8UC1", type);

		cv::Mat mat;
		cvtColor(image, mat, cv::COLOR_GRAY2BGRA);
		QImage view(mat.data, mat.cols, mat.rows, (int)mat.step[0], QImage::Format_ARGB32);
		out = view.copy();
		break;
	}
	default:
	{
		qDebug("Unspoorted image format:%d", image.type());
		throw std::invalid_argument("Image format not supported");
		break;
	}
	}
}

QString print_mat(const char* name, const cv::Mat& M)
{
	std::ostringstream oss;
	oss << name << "\n" << " " << M << "\n";
	return QString(oss.str().c_str());
}