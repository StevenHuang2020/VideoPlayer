// ***********************************************************/
// player_skin.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Player skin module.
// ***********************************************************/

#include "player_skin.h"
#include "common.h"

const QString PlayerSkin::m_qss_path = "./res/qss";

void PlayerSkin::clear_skin()
{
    qApp->setStyleSheet("");
}

QStringList PlayerSkin::get_style() const
{
    return QStyleFactory::keys();
}

void PlayerSkin::set_system_style(const QString& style)
{
    clear_skin();
    qApp->setStyle(QStyleFactory::create(style));
    qApp->setPalette(QApplication::style()->standardPalette());
}

void PlayerSkin::set_custom_style(const QString& name)
{
    QString file = appendPath(m_qss_path, name + ".qss");
    QFile qss(file);

    if (QFileInfo::exists(file))
    {
        clear_skin();
        qss.open(QFile::ReadOnly);
        qApp->setStyleSheet(qss.readAll());
    }
}

QStringList PlayerSkin::get_custom_styles() const
{
    QDir directory(m_qss_path);
    return directory.entryList(QStringList() << "*.qss", QDir::Files);
}
