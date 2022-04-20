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
    QByteArrayData data[23];
    char stringdata0[423];
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
QT_MOC_LITERAL(6, 88, 27), // "wait_stop_audio_play_thread"
QT_MOC_LITERAL(7, 116, 27), // "wait_stop_video_play_thread"
QT_MOC_LITERAL(8, 144, 20), // "receive_image_signal"
QT_MOC_LITERAL(9, 165, 19), // "audio_receive_sinal"
QT_MOC_LITERAL(10, 185, 8), // "uint8_t*"
QT_MOC_LITERAL(11, 194, 6), // "buffer"
QT_MOC_LITERAL(12, 201, 7), // "bufsize"
QT_MOC_LITERAL(13, 209, 23), // "on_actionOpen_triggered"
QT_MOC_LITERAL(14, 233, 23), // "on_actionQuit_triggered"
QT_MOC_LITERAL(15, 257, 23), // "on_actionHelp_triggered"
QT_MOC_LITERAL(16, 281, 24), // "on_actionAbout_triggered"
QT_MOC_LITERAL(17, 306, 23), // "on_actionStop_triggered"
QT_MOC_LITERAL(18, 330, 12), // "update_image"
QT_MOC_LITERAL(19, 343, 20), // "decode_video_stopped"
QT_MOC_LITERAL(20, 364, 20), // "decode_audio_stopped"
QT_MOC_LITERAL(21, 385, 18), // "audio_play_stopped"
QT_MOC_LITERAL(22, 404, 18) // "video_play_stopped"

    },
    "MainWindow\0stop_audio_play_thread\0\0"
    "stop_video_play_thread\0stop_decode_thread\0"
    "pause_play\0wait_stop_audio_play_thread\0"
    "wait_stop_video_play_thread\0"
    "receive_image_signal\0audio_receive_sinal\0"
    "uint8_t*\0buffer\0bufsize\0on_actionOpen_triggered\0"
    "on_actionQuit_triggered\0on_actionHelp_triggered\0"
    "on_actionAbout_triggered\0"
    "on_actionStop_triggered\0update_image\0"
    "decode_video_stopped\0decode_audio_stopped\0"
    "audio_play_stopped\0video_play_stopped"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  104,    2, 0x06 /* Public */,
       3,    0,  105,    2, 0x06 /* Public */,
       4,    0,  106,    2, 0x06 /* Public */,
       5,    0,  107,    2, 0x06 /* Public */,
       6,    0,  108,    2, 0x06 /* Public */,
       7,    0,  109,    2, 0x06 /* Public */,
       8,    1,  110,    2, 0x06 /* Public */,
       9,    2,  113,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    0,  118,    2, 0x08 /* Private */,
      14,    0,  119,    2, 0x08 /* Private */,
      15,    0,  120,    2, 0x08 /* Private */,
      16,    0,  121,    2, 0x08 /* Private */,
      17,    0,  122,    2, 0x08 /* Private */,
      18,    1,  123,    2, 0x0a /* Public */,
      19,    0,  126,    2, 0x0a /* Public */,
      20,    0,  127,    2, 0x0a /* Public */,
      21,    0,  128,    2, 0x0a /* Public */,
      22,    0,  129,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QImage,    2,
    QMetaType::Void, 0x80000000 | 10, QMetaType::Int,   11,   12,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QImage,    2,
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
        case 3: _t->pause_play(); break;
        case 4: _t->wait_stop_audio_play_thread(); break;
        case 5: _t->wait_stop_video_play_thread(); break;
        case 6: _t->receive_image_signal((*reinterpret_cast< const QImage(*)>(_a[1]))); break;
        case 7: _t->audio_receive_sinal((*reinterpret_cast< uint8_t*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 8: _t->on_actionOpen_triggered(); break;
        case 9: _t->on_actionQuit_triggered(); break;
        case 10: _t->on_actionHelp_triggered(); break;
        case 11: _t->on_actionAbout_triggered(); break;
        case 12: _t->on_actionStop_triggered(); break;
        case 13: _t->update_image((*reinterpret_cast< const QImage(*)>(_a[1]))); break;
        case 14: _t->decode_video_stopped(); break;
        case 15: _t->decode_audio_stopped(); break;
        case 16: _t->audio_play_stopped(); break;
        case 17: _t->video_play_stopped(); break;
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
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::wait_stop_audio_play_thread)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::wait_stop_video_play_thread)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(const QImage & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::receive_image_signal)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(uint8_t * , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::audio_receive_sinal)) {
                *result = 7;
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
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 18)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 18;
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

// SIGNAL 4
void MainWindow::wait_stop_audio_play_thread()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void MainWindow::wait_stop_video_play_thread()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void MainWindow::receive_image_signal(const QImage & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void MainWindow::audio_receive_sinal(uint8_t * _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
