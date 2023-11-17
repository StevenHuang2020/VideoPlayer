// ***********************************************************/
// common.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Common QT funtions
// *

#include "common.h"

QString appendPath(const QString& path, const QString& sub_path)
{
    return QDir::cleanPath(path + QDir::separator() + sub_path);
}

QString toNativePath(const QString& path)
{
    return QDir::toNativeSeparators(path);
}
