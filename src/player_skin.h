#pragma once
#include <QApplication>
#include <QStyleFactory>
#include <QStyle>
#include <QFile>
#include <QDir>

class PlayerSkin {
public:
	explicit PlayerSkin() {};
	~PlayerSkin() {};

public:
	QStringList get_style() const;
	QStringList get_custom_styles() const;
	void set_system_style(const QString& style);
	void set_custom_style(const QString& filename);

private:
	void clear_skin();

private:
	const static QString m_qss_path;
};