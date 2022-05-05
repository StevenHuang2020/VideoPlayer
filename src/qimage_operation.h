#ifndef QIMAGE_OPERATION_H
#define QIMAGE_OPERATION_H
#include <QImage>
#include <QRandomGenerator>
#include <QPainter>
#include <QPen>
#include <QFont>

void image_info(const QImage& img);
void create_image(QImage& img);
bool draw_img_text(QImage& img, const QString& str, const QRect rt,
	QPen pen = QPen(Qt::red), QFont font = QFont("Times", 48, QFont::Bold));

bool draw_img_rect(QImage& img, const QRect rt, QPen pen = QPen(Qt::red));
void grey_image(QImage& img);
void random_image(QImage& img);
void splt_image(QImage& img, QImage& r_img, QImage& b_img, QImage& g_img);
void invert_image(QImage& img);
void mirro_image(QImage& img, bool horizontal = false, bool vertical = true);
void swap_image(QImage& img); //rbg<==>bgr
void scale_image(QImage& img, int width, int height);

void transform_image(QImage& img, const QTransform& matrix, Qt::TransformationMode mode = Qt::FastTransformation);

#endif // !QIMAGE_OPERATION_H
