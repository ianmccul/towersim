/****************************************************************************
** Meta object code from reading C++ file 'methodcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "methodcontroller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'methodcontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MethodController_t {
    QByteArrayData data[17];
    char stringdata0[224];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MethodController_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MethodController_t qt_meta_stringdata_MethodController = {
    {
QT_MOC_LITERAL(0, 0, 16), // "MethodController"
QT_MOC_LITERAL(1, 17, 26), // "NumberOfBells_valueChanged"
QT_MOC_LITERAL(2, 44, 0), // ""
QT_MOC_LITERAL(3, 45, 5), // "Value"
QT_MOC_LITERAL(4, 51, 12), // "RingingStart"
QT_MOC_LITERAL(5, 64, 11), // "RingingAuto"
QT_MOC_LITERAL(6, 76, 11), // "RingingStop"
QT_MOC_LITERAL(7, 88, 13), // "RingingRounds"
QT_MOC_LITERAL(8, 102, 12), // "RingingStand"
QT_MOC_LITERAL(9, 115, 15), // "CCButtonClicked"
QT_MOC_LITERAL(10, 131, 1), // "n"
QT_MOC_LITERAL(11, 133, 15), // "CallModeChanged"
QT_MOC_LITERAL(12, 149, 18), // "SelectMethodPushed"
QT_MOC_LITERAL(13, 168, 14), // "MethodGoPushed"
QT_MOC_LITERAL(14, 183, 20), // "MethodGoDelayToggled"
QT_MOC_LITERAL(15, 204, 7), // "Checked"
QT_MOC_LITERAL(16, 212, 11) // "CallClicked"

    },
    "MethodController\0NumberOfBells_valueChanged\0"
    "\0Value\0RingingStart\0RingingAuto\0"
    "RingingStop\0RingingRounds\0RingingStand\0"
    "CCButtonClicked\0n\0CallModeChanged\0"
    "SelectMethodPushed\0MethodGoPushed\0"
    "MethodGoDelayToggled\0Checked\0CallClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MethodController[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   74,    2, 0x08 /* Private */,
       4,    0,   77,    2, 0x08 /* Private */,
       5,    0,   78,    2, 0x08 /* Private */,
       6,    0,   79,    2, 0x08 /* Private */,
       7,    0,   80,    2, 0x08 /* Private */,
       8,    0,   81,    2, 0x08 /* Private */,
       9,    1,   82,    2, 0x08 /* Private */,
      11,    0,   85,    2, 0x08 /* Private */,
      12,    0,   86,    2, 0x08 /* Private */,
      13,    0,   87,    2, 0x08 /* Private */,
      14,    1,   88,    2, 0x08 /* Private */,
      16,    1,   91,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   15,
    QMetaType::Void, QMetaType::Int,   10,

       0        // eod
};

void MethodController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MethodController *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->NumberOfBells_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->RingingStart(); break;
        case 2: _t->RingingAuto(); break;
        case 3: _t->RingingStop(); break;
        case 4: _t->RingingRounds(); break;
        case 5: _t->RingingStand(); break;
        case 6: _t->CCButtonClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->CallModeChanged(); break;
        case 8: _t->SelectMethodPushed(); break;
        case 9: _t->MethodGoPushed(); break;
        case 10: _t->MethodGoDelayToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->CallClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MethodController::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_MethodController.data,
    qt_meta_data_MethodController,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MethodController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MethodController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MethodController.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MethodController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
