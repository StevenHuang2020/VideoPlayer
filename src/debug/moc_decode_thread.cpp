/****************************************************************************
** Meta object code from reading C++ file 'decode_thread.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.12)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../decode_thread.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'decode_thread.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.12. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DecodeThread_t {
    QByteArrayData data[13];
    char stringdata0[139];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DecodeThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DecodeThread_t qt_meta_stringdata_DecodeThread = {
    {
QT_MOC_LITERAL(0, 0, 12), // "DecodeThread"
QT_MOC_LITERAL(1, 13, 11), // "frame_ready"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 13), // "finish_decode"
QT_MOC_LITERAL(4, 40, 14), // "decode_context"
QT_MOC_LITERAL(5, 55, 21), // "const AVCodecContext*"
QT_MOC_LITERAL(6, 77, 6), // "pVideo"
QT_MOC_LITERAL(7, 84, 6), // "pAudio"
QT_MOC_LITERAL(8, 91, 11), // "audio_ready"
QT_MOC_LITERAL(9, 103, 7), // "bufsize"
QT_MOC_LITERAL(10, 111, 8), // "uint8_t*"
QT_MOC_LITERAL(11, 120, 6), // "buffer"
QT_MOC_LITERAL(12, 127, 11) // "stop_decode"

    },
    "DecodeThread\0frame_ready\0\0finish_decode\0"
    "decode_context\0const AVCodecContext*\0"
    "pVideo\0pAudio\0audio_ready\0bufsize\0"
    "uint8_t*\0buffer\0stop_decode"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DecodeThread[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       3,    0,   42,    2, 0x06 /* Public */,
       4,    2,   43,    2, 0x06 /* Public */,
       8,    2,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      12,    0,   53,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QImage,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 5, 0x80000000 | 5,    6,    7,
    QMetaType::Void, QMetaType::Int, 0x80000000 | 10,    9,   11,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void DecodeThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DecodeThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->frame_ready((*reinterpret_cast< const QImage(*)>(_a[1]))); break;
        case 1: _t->finish_decode(); break;
        case 2: _t->decode_context((*reinterpret_cast< const AVCodecContext*(*)>(_a[1])),(*reinterpret_cast< const AVCodecContext*(*)>(_a[2]))); break;
        case 3: _t->audio_ready((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< uint8_t*(*)>(_a[2]))); break;
        case 4: _t->stop_decode(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DecodeThread::*)(const QImage & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DecodeThread::frame_ready)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DecodeThread::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DecodeThread::finish_decode)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DecodeThread::*)(const AVCodecContext * , const AVCodecContext * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DecodeThread::decode_context)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (DecodeThread::*)(int , uint8_t * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DecodeThread::audio_ready)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DecodeThread::staticMetaObject = { {
    &QThread::staticMetaObject,
    qt_meta_stringdata_DecodeThread.data,
    qt_meta_data_DecodeThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DecodeThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DecodeThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DecodeThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int DecodeThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DecodeThread::frame_ready(const QImage & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DecodeThread::finish_decode()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DecodeThread::decode_context(const AVCodecContext * _t1, const AVCodecContext * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void DecodeThread::audio_ready(int _t1, uint8_t * _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
