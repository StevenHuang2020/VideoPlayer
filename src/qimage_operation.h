#ifndef QIMAGE_OPERATION_H
#define QIMAGE_OPERATION_H
#include <QImage>
#include <QRandomGenerator>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QGraphicsEffect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsColorizeEffect>

void image_info(const QImage& img);
void create_image(QImage& img);
bool draw_img_text(QImage& img, const QString& str, const QRect rt,
                   QPen pen = QPen(Qt::red), QFont font = QFont("Times", 48, QFont::Bold));

bool draw_img_rect(QImage& img, const QRect rt, QPen pen = QPen(Qt::red));
void grey_image(QImage& img);
void random_image(QImage& img);
void split_image(QImage& img, QImage& r_img, QImage& b_img, QImage& g_img);
void invert_image(QImage& img);
void mirro_image(QImage& img, bool horizontal = true, bool vertical = false);
void swap_image(QImage& img); //rbg<==>bgr
void scale_image(QImage& img, int width, int height);

void transform_image(QImage& img, const QTransform& matrix, Qt::TransformationMode mode = Qt::FastTransformation);
QImage gamma_image(const QImage& img, double exp = 1 / 2.0);

/*
QImage applyEffectToImage(QImage& src, QGraphicsEffect* effect, int extent = 0);
QImage blur_img(QImage& img, int radius = 5, int extent = 0);
QImage dropshadow_img(QImage& img, int radius = 5, int offsetX = 0, int offsetY = 0, QColor color = QColor(40, 40, 40, 255), int extent = 0);
QImage colorize_img(QImage& img, QColor color = QColor(40, 40, 40, 255), double strength = 0.5);
QImage opacity_img(QImage& img, double opacity = 0.5);
*/
#endif // QIMAGE_OPERATION_H
