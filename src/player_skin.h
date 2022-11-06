#ifndef __PLAYER_SKIN_H__
#define __PLAYER_SKIN_H__

#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include <QFile>
#include <QDir>

class PlayerSkin {
public:
	explicit PlayerSkin();
	~PlayerSkin();

public:
	QStringList get_style() const;
	void set_system_style(const QString& style);
	void set_custom_style(const QString& filename);
	QStringList get_custom_styles() const;

private:
	void clear_skin();

private:
	const static QString m_qss_path;
};


#endif /*end of __PLAYER_SKIN_H__*/