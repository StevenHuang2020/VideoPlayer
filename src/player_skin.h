#ifndef __PLAYER_SKIN_H__
#define __PLAYER_SKIN_H__

#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include <QFile>
#include <QDir>

class PlayerSkin {
public:
	PlayerSkin();
	~PlayerSkin();

private:
	const static QString m_qss_path;

private:
	void clear_skin();
public:
	const QStringList get_style() const;
	void set_system_style(const QString& style);
	void set_custom_style(const QString& filename);
	const QStringList get_custom_styles() const;
};


#endif /*end of __PLAYER_SKIN_H__*/