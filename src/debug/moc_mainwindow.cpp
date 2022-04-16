/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[17];
    char stringdata0[257];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 23), // "on_actionOpen_triggered"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 23), // "on_actionQuit_triggered"
QT_MOC_LITERAL(4, 60, 23), // "on_actionHelp_triggered"
QT_MOC_LITERAL(5, 84, 24), // "on_actionAbout_triggered"
QT_MOC_LITERAL(6, 109, 23), // "on_actionStop_triggered"
QT_MOC_LITERAL(7, 133, 12), // "update_image"
QT_MOC_LITERAL(8, 146, 20), // "handle_decodeContext"
QT_MOC_LITERAL(9, 167, 21), // "const AVCodecContext*"
QT_MOC_LITERAL(10, 189, 6), // "pVideo"
QT_MOC_LITERAL(11, 196, 6), // "pAudio"
QT_MOC_LITERAL(12, 203, 18), // "video_play_stopped"
QT_MOC_LITERAL(13, 222, 10), // "audio_play"
QT_MOC_LITERAL(14, 233, 7), // "bufsize"
QT_MOC_LITERAL(15, 241, 8), // "uint8_t*"
QT_MOC_LITERAL(16, 250, 6) // "buffer"

    },
    "MainWindow\0on_actionOpen_triggered\0\0"
    "on_actionQuit_triggered\0on_actionHelp_triggered\0"
    "on_actionAbout_triggered\0"
    "on_actionStop_triggered\0update_image\0"
    "handle_decodeContext\0const AVCodecContext*\0"
    "pVideo\0pAudio\0video_play_stopped\0"
    "audio_play\0bufsize\0uint8_t*\0buffer"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08 /* Private */,
       3,    0,   60,    2, 0x08 /* Private */,
       4,    0,   61,    2, 0x08 /* Private */,
       5,    0,   62,    2, 0x08 /* Private */,
       6,    0,   63,    2, 0x08 /* Private */,
       7,    1,   64,    2, 0x0a /* Public */,
       8,    2,   67,    2, 0x0a /* Public */,
      12,    0,   72,    2, 0x0a /* Public */,
      13,    2,   73,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QImage,    2,
    QMetaType::Void, 0x80000000 | 9, 0x80000000 | 9,   10,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 15,   14,   16,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_actionOpen_triggered(); break;
        case 1: _t->on_actionQuit_triggered(); break;
        case 2: _t->on_actionHelp_triggered(); break;
        case 3: _t->on_actionAbout_triggered(); break;
        case 4: _t->on_actionStop_triggered(); break;
        case 5: _t->update_image((*reinterpret_cast< const QImage(*)>(_a[1]))); break;
        case 6: _t->handle_decodeContext((*reinterpret_cast< const AVCodecContext*(*)>(_a[1])),(*reinterpret_cast< const AVCodecContext*(*)>(_a[2]))); break;
        case 7: _t->video_play_stopped(); break;
        case 8: _t->audio_play((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uint8_t*(*)>(_a[2]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
