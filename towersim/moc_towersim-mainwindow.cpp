/****************************************************************************
** Meta object code from reading C++ file 'towersim-mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "towersim-mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'towersim-mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TowerSimMainWindow_t {
    QByteArrayData data[7];
    char stringdata0[110];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TowerSimMainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TowerSimMainWindow_t qt_meta_stringdata_TowerSimMainWindow = {
    {
QT_MOC_LITERAL(0, 0, 18), // "TowerSimMainWindow"
QT_MOC_LITERAL(1, 19, 29), // "ChangesPerMinute_valueChanged"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 5), // "Value"
QT_MOC_LITERAL(4, 56, 21), // "PealSpeed_timeChanged"
QT_MOC_LITERAL(5, 78, 4), // "Time"
QT_MOC_LITERAL(6, 83, 26) // "NumberOfBells_valueChanged"

    },
    "TowerSimMainWindow\0ChangesPerMinute_valueChanged\0"
    "\0Value\0PealSpeed_timeChanged\0Time\0"
    "NumberOfBells_valueChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TowerSimMainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x08 /* Private */,
       4,    1,   32,    2, 0x08 /* Private */,
       6,    1,   35,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Double,    3,
    QMetaType::Void, QMetaType::QTime,    5,
    QMetaType::Void, QMetaType::Int,    3,

       0        // eod
};

void TowerSimMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TowerSimMainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ChangesPerMinute_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->PealSpeed_timeChanged((*reinterpret_cast< QTime(*)>(_a[1]))); break;
        case 2: _t->NumberOfBells_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TowerSimMainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_TowerSimMainWindow.data,
    qt_meta_data_TowerSimMainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TowerSimMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TowerSimMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TowerSimMainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int TowerSimMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
