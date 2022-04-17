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
    QByteArrayData data[24];
    char stringdata0[384];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 22), // "stop_audio_play_thread"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 22), // "stop_video_play_thread"
QT_MOC_LITERAL(4, 58, 18), // "stop_decode_thread"
QT_MOC_LITERAL(5, 77, 10), // "pause_play"
QT_MOC_LITERAL(6, 88, 23), // "on_actionOpen_triggered"
QT_MOC_LITERAL(7, 112, 23), // "on_actionQuit_triggered"
QT_MOC_LITERAL(8, 136, 23), // "on_actionHelp_triggered"
QT_MOC_LITERAL(9, 160, 24), // "on_actionAbout_triggered"
QT_MOC_LITERAL(10, 185, 23), // "on_actionStop_triggered"
QT_MOC_LITERAL(11, 209, 13), // "receive_image"
QT_MOC_LITERAL(12, 223, 12), // "update_image"
QT_MOC_LITERAL(13, 236, 20), // "handle_decodeContext"
QT_MOC_LITERAL(14, 257, 21), // "const AVCodecContext*"
QT_MOC_LITERAL(15, 279, 6), // "pVideo"
QT_MOC_LITERAL(16, 286, 6), // "pAudio"
QT_MOC_LITERAL(17, 293, 14), // "decode_stopped"
QT_MOC_LITERAL(18, 308, 13), // "audio_receive"
QT_MOC_LITERAL(19, 322, 7), // "bufsize"
QT_MOC_LITERAL(20, 330, 8), // "uint8_t*"
QT_MOC_LITERAL(21, 339, 6), // "buffer"
QT_MOC_LITERAL(22, 346, 18), // "audio_play_stopped"
QT_MOC_LITERAL(23, 365, 18) // "video_play_stopped"

    },
    "MainWindow\0stop_audio_play_thread\0\0"
    "stop_video_play_thread\0stop_decode_thread\0"
    "pause_play\0on_actionOpen_triggered\0"
    "on_actionQuit_triggered\0on_actionHelp_triggered\0"
    "on_actionAbout_triggered\0"
    "on_actionStop_triggered\0receive_image\0"
    "update_image\0handle_decodeContext\0"
    "const AVCodecContext*\0pVideo\0pAudio\0"
    "decode_stopped\0audio_receive\0bufsize\0"
    "uint8_t*\0buffer\0audio_play_stopped\0"
    "video_play_stopped"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,
       3,    0,   95,    2, 0x06 /* Public */,
       4,    0,   96,    2, 0x06 /* Public */,
       5,    0,   97,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   98,    2, 0x08 /* Private */,
       7,    0,   99,    2, 0x08 /* Private */,
       8,    0,  100,    2, 0x08 /* Private */,
       9,    0,  101,    2, 0x08 /* Private */,
      10,    0,  102,    2, 0x08 /* Private */,
      11,    1,  103,    2, 0x0a /* Public */,
      12,    1,  106,    2, 0x0a /* Public */,
      13,    2,  109,    2, 0x0a /* Public */,
      17,    0,  114,    2, 0x0a /* Public */,
      18,    2,  115,    2, 0x0a /* Public */,
      22,    0,  120,    2, 0x0a /* Public */,
      23,    0,  121,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QImage,    2,
    QMetaType::Void, QMetaType::QImage,    2,
    QMetaType::Void, 0x80000000 | 14, 0x80000000 | 14,   15,   16,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 20,   19,   21,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->stop_audio_play_thread(); break;
        case 1: _t->stop_video_play_thread(); break;
        case 2: _t->stop_decode_thread(); break;
        case 3: _t->pause_play(); break;
        case 4: _t->on_actionOpen_triggered(); break;
        case 5: _t->on_actionQuit_triggered(); break;
        case 6: _t->on_actionHelp_triggered(); break;
        case 7: _t->on_actionAbout_triggered(); break;
        case 8: _t->on_actionStop_triggered(); break;
        case 9: _t->receive_image((*reinterpret_cast< const QImage(*)>(_a[1]))); break;
        case 10: _t->update_image((*reinterpret_cast< const QImage(*)>(_a[1]))); break;
        case 11: _t->handle_decodeContext((*reinterpret_cast< const AVCodecContext*(*)>(_a[1])),(*reinterpret_cast< const AVCodecContext*(*)>(_a[2]))); break;
        case 12: _t->decode_stopped(); break;
        case 13: _t->audio_receive((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uint8_t*(*)>(_a[2]))); break;
        case 14: _t->audio_play_stopped(); break;
        case 15: _t->video_play_stopped(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::stop_audio_play_thread)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::stop_video_play_thread)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::stop_decode_thread)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::pause_play)) {
                *result = 3;
                return;
            }
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
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::stop_audio_play_thread()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MainWindow::stop_video_play_thread()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MainWindow::stop_decode_thread()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void MainWindow::pause_play()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
