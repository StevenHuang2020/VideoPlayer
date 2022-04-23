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
    QByteArrayData data[27];
    char stringdata0[539];
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
QT_MOC_LITERAL(5, 77, 23), // "stop_read_packet_thread"
QT_MOC_LITERAL(6, 101, 10), // "pause_play"
QT_MOC_LITERAL(7, 112, 27), // "wait_stop_audio_play_thread"
QT_MOC_LITERAL(8, 140, 27), // "wait_stop_video_play_thread"
QT_MOC_LITERAL(9, 168, 23), // "on_actionOpen_triggered"
QT_MOC_LITERAL(10, 192, 23), // "on_actionQuit_triggered"
QT_MOC_LITERAL(11, 216, 23), // "on_actionHelp_triggered"
QT_MOC_LITERAL(12, 240, 24), // "on_actionAbout_triggered"
QT_MOC_LITERAL(13, 265, 23), // "on_actionStop_triggered"
QT_MOC_LITERAL(14, 289, 23), // "on_actionHide_triggered"
QT_MOC_LITERAL(15, 313, 29), // "on_actionFullscreen_triggered"
QT_MOC_LITERAL(16, 343, 26), // "on_actionYoutube_triggered"
QT_MOC_LITERAL(17, 370, 12), // "update_image"
QT_MOC_LITERAL(18, 383, 19), // "print_decodeContext"
QT_MOC_LITERAL(19, 403, 21), // "const AVCodecContext*"
QT_MOC_LITERAL(20, 425, 6), // "pVideo"
QT_MOC_LITERAL(21, 432, 6), // "bVideo"
QT_MOC_LITERAL(22, 439, 20), // "decode_video_stopped"
QT_MOC_LITERAL(23, 460, 20), // "decode_audio_stopped"
QT_MOC_LITERAL(24, 481, 18), // "audio_play_stopped"
QT_MOC_LITERAL(25, 500, 18), // "video_play_stopped"
QT_MOC_LITERAL(26, 519, 19) // "read_packet_stopped"

    },
    "MainWindow\0stop_audio_play_thread\0\0"
    "stop_video_play_thread\0stop_decode_thread\0"
    "stop_read_packet_thread\0pause_play\0"
    "wait_stop_audio_play_thread\0"
    "wait_stop_video_play_thread\0"
    "on_actionOpen_triggered\0on_actionQuit_triggered\0"
    "on_actionHelp_triggered\0"
    "on_actionAbout_triggered\0"
    "on_actionStop_triggered\0on_actionHide_triggered\0"
    "on_actionFullscreen_triggered\0"
    "on_actionYoutube_triggered\0update_image\0"
    "print_decodeContext\0const AVCodecContext*\0"
    "pVideo\0bVideo\0decode_video_stopped\0"
    "decode_audio_stopped\0audio_play_stopped\0"
    "video_play_stopped\0read_packet_stopped"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  129,    2, 0x06 /* Public */,
       3,    0,  130,    2, 0x06 /* Public */,
       4,    0,  131,    2, 0x06 /* Public */,
       5,    0,  132,    2, 0x06 /* Public */,
       6,    0,  133,    2, 0x06 /* Public */,
       7,    0,  134,    2, 0x06 /* Public */,
       8,    0,  135,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    0,  136,    2, 0x08 /* Private */,
      10,    0,  137,    2, 0x08 /* Private */,
      11,    0,  138,    2, 0x08 /* Private */,
      12,    0,  139,    2, 0x08 /* Private */,
      13,    0,  140,    2, 0x08 /* Private */,
      14,    0,  141,    2, 0x08 /* Private */,
      15,    0,  142,    2, 0x08 /* Private */,
      16,    0,  143,    2, 0x08 /* Private */,
      17,    1,  144,    2, 0x0a /* Public */,
      18,    2,  147,    2, 0x0a /* Public */,
      18,    1,  152,    2, 0x2a /* Public | MethodCloned */,
      22,    0,  155,    2, 0x0a /* Public */,
      23,    0,  156,    2, 0x0a /* Public */,
      24,    0,  157,    2, 0x0a /* Public */,
      25,    0,  158,    2, 0x0a /* Public */,
      26,    0,  159,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QImage,    2,
    QMetaType::Void, 0x80000000 | 19, QMetaType::Bool,   20,   21,
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
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
        case 3: _t->stop_read_packet_thread(); break;
        case 4: _t->pause_play(); break;
        case 5: _t->wait_stop_audio_play_thread(); break;
        case 6: _t->wait_stop_video_play_thread(); break;
        case 7: _t->on_actionOpen_triggered(); break;
        case 8: _t->on_actionQuit_triggered(); break;
        case 9: _t->on_actionHelp_triggered(); break;
        case 10: _t->on_actionAbout_triggered(); break;
        case 11: _t->on_actionStop_triggered(); break;
        case 12: _t->on_actionHide_triggered(); break;
        case 13: _t->on_actionFullscreen_triggered(); break;
        case 14: _t->on_actionYoutube_triggered(); break;
        case 15: _t->update_image((*reinterpret_cast< const QImage(*)>(_a[1]))); break;
        case 16: _t->print_decodeContext((*reinterpret_cast< const AVCodecContext*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 17: _t->print_decodeContext((*reinterpret_cast< const AVCodecContext*(*)>(_a[1]))); break;
        case 18: _t->decode_video_stopped(); break;
        case 19: _t->decode_audio_stopped(); break;
        case 20: _t->audio_play_stopped(); break;
        case 21: _t->video_play_stopped(); break;
        case 22: _t->read_packet_stopped(); break;
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
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::stop_read_packet_thread)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::pause_play)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::wait_stop_audio_play_thread)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::wait_stop_video_play_thread)) {
                *result = 6;
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
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 23;
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
void MainWindow::stop_read_packet_thread()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MainWindow::pause_play()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void MainWindow::wait_stop_audio_play_thread()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void MainWindow::wait_stop_video_play_thread()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
