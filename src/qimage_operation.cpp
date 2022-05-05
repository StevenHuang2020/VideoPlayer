#include "qimage_operation.h"


void image_info(const QImage& img)
{
	qDebug("QImage: w:%d, h:%d, (format:%d,alpha:%d,grey:%d,null:%d),"
		"color count:%d, size(%d,%d), sizeinbytes:%lld,"
		"depth:%d, devicePixelRatio:%f, dotsX:%d, dotsY:%d,"
		"offset(%d,%d), rect(%d,%d,%d,%d),"
		"(hMM:%d,wMM:%d,logDpiX:%d,logDpiY:%d,phsDpiX:%d,phsDpiY:%d)",
		img.width(), img.height(), img.format(), img.hasAlphaChannel(), img.isGrayscale(), img.isNull(),
		img.colorCount(), img.size().width(), img.size().height(), img.sizeInBytes(),
		img.depth(), img.devicePixelRatio(), img.dotsPerMeterX(), img.dotsPerMeterY(),
		img.offset().x(), img.offset().y(), img.rect().x(), img.rect().y(), img.rect().width(), img.rect().height(),
		img.heightMM(), img.widthMM(), img.logicalDpiX(), img.logicalDpiY(), img.physicalDpiX(), img.physicalDpiY());
}

void create_image(QImage& img)
{
	image_info(img);

	/*int w = img.width();
	int h = img.height();*/

	// QRgb value;
	//value = qRgb(237, 187, 51); // 0xffedba31
	//image.setPixel(2, 1, value);

	int n = 50;
	QRgb value[50];
	for (int i = 0; i < n; i++)
		value[i] = int(0xff000000 + 0xffffff * i / n);

	Q_ASSERT(img.format() == QImage::Format_RGB32);
	random_image(img);
}

bool draw_img_text(QImage& img, const QString& str, const QRect rt, QPen pen, QFont font)
{
	QPainter p;
	if (!p.begin(&img))
		return false;

	p.setPen(pen);
	p.setFont(font);
	p.drawText(rt, Qt::AlignCenter, str);
	return p.end();
}

bool draw_img_rect(QImage& img, const QRect rt, QPen pen)
{
	QPainter p;
	if (!p.begin(&img))
		return false;

	p.setPen(pen);
	p.drawRect(rt);
	return p.end();
}

void grey_image(QImage& img)
{
#if 1
	img = img.convertToFormat(QImage::Format_Grayscale8);
#else
	int depth = sizeof(QRgb);//4
	for (int i = 0; i < img.height(); i++) {
		uchar* scan = img.scanLine(i);
		for (int j = 0; j < img.width(); j++) {
			QRgb* rgbpixel = reinterpret_cast<QRgb*>(scan + j * depth);
			int gray = qGray(*rgbpixel);
			*rgbpixel = QColor(gray, gray, gray).rgb();
		}
	}
#endif
}

void random_image(QImage& img)
{
#if 1
	int depth = sizeof(QRgb);//img.depth() / 8; 4;
	for (int i = 0; i < img.height(); i++) {
		uchar* scan = img.scanLine(i);
		for (int j = 0; j < img.width(); j++) {
			QRgb* rgbpixel = reinterpret_cast<QRgb*>(scan + j * depth);

			int r = QRandomGenerator::global()->generate();
			int g = QRandomGenerator::global()->generate();
			int b = QRandomGenerator::global()->generate();

			//*rgbpixel = QColor(r % 255, g % 255, b % 255).rgb();
			*rgbpixel = qRgb(r, g, b);

			/*
			QColor color = QColor(r%255, g%255, b%255);
			QRgb ca = color.rgb();
			int ca_r = color.red();
			int ca_g = color.green();
			int ca_b = color.blue();
			QRgb cc = color.rgba();

			QRgb cb = qRgb(r, g, b);
			qDebug("cb=0x%X", cb);
			*/
		}
	}
#else
	for (int i = 0; i < img.height(); i++)
	{
		for (int j = 0; j < img.width(); j++)
		{
			// img.setPixel(j, i, value[w % n]);
			int r = QRandomGenerator::global()->generate();
			int g = QRandomGenerator::global()->generate();
			int b = QRandomGenerator::global()->generate();
			img.setPixel(j, i, qRgb(r % 255, g % 255, b % 255));
		}
	}
#endif
}

void splt_image(QImage& img, QImage& r_img, QImage& b_img, QImage& g_img)
{
	Q_ASSERT(img.size() == r_img.size());
	Q_ASSERT(r_img.size() == b_img.size());
	Q_ASSERT(r_img.size() == g_img.size());

	int depth = sizeof(QRgb);//img.depth() / 8; 4;
	for (int i = 0; i < img.height(); i++) {
		uchar* scan = img.scanLine(i);

		uchar* r_scan = r_img.scanLine(i);
		uchar* b_scan = b_img.scanLine(i);
		uchar* g_scan = g_img.scanLine(i);

		for (int j = 0; j < img.width(); j++) {
			QRgb* all_rgbpixel = reinterpret_cast<QRgb*>(scan + j * depth);

			QRgb* r_rgbpixel = reinterpret_cast<QRgb*>(r_scan + j * depth);
			QRgb* g_rgbpixel = reinterpret_cast<QRgb*>(g_scan + j * depth);
			QRgb* b_rgbpixel = reinterpret_cast<QRgb*>(b_scan + j * depth);

			uint r = qRed(*all_rgbpixel);
			uint g = qGreen(*all_rgbpixel);
			uint b = qBlue(*all_rgbpixel);

			/**r_rgbpixel = qRgb(r, r, r);
			*g_rgbpixel = qRgb(g, g, g);
			*b_rgbpixel = qRgb(b, b, b);*/

			*r_rgbpixel = qRgb(r, 0, 0);
			*g_rgbpixel = qRgb(0, g, 0);
			*b_rgbpixel = qRgb(0, 0, b);
		}
	}
}

void invert_image(QImage& img)
{
	img.invertPixels();
}

void mirro_image(QImage& img, bool horizontal, bool vertical)
{
	img = img.mirrored(horizontal, vertical);
}

void swap_image(QImage& img)
{
	img = img.rgbSwapped();
}

void scale_image(QImage& img, int width, int height)
{
	img = img.scaled(width, height, Qt::IgnoreAspectRatio, Qt::FastTransformation);
}

void transform_image(QImage& img, const QTransform& matrix, Qt::TransformationMode mode)
{

}
