#ifndef PLAYER_SKIN_H
#define PLAYER_SKIN_H
#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include <QFile>
#include <QDir>

const QStringList get_style();
const QStringList get_custom_styles();
void clear_skin();
void set_system_style(int id = 0);
void set_system_style(const QString& style);
void set_custom_style(const QString& file);

#endif /*end of PLAYER_SKIN_H*/