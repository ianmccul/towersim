/****************************************************************************
** Meta object code from reading C++ file 'select-method.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "select-method.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'select-method.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SelectMethodDialog_t {
    QByteArrayData data[11];
    char stringdata0[137];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SelectMethodDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SelectMethodDialog_t qt_meta_stringdata_SelectMethodDialog = {
    {
QT_MOC_LITERAL(0, 0, 18), // "SelectMethodDialog"
QT_MOC_LITERAL(1, 19, 12), // "StageChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 5), // "Stage"
QT_MOC_LITERAL(4, 39, 13), // "FilterChanged"
QT_MOC_LITERAL(5, 53, 19), // "SearchStringChanged"
QT_MOC_LITERAL(6, 73, 16), // "FilterAllClicked"
QT_MOC_LITERAL(7, 90, 17), // "FilterNoneClicked"
QT_MOC_LITERAL(8, 108, 15), // "ShowContextMenu"
QT_MOC_LITERAL(9, 124, 3), // "pos"
QT_MOC_LITERAL(10, 128, 8) // "Accepted"

    },
    "SelectMethodDialog\0StageChanged\0\0Stage\0"
    "FilterChanged\0SearchStringChanged\0"
    "FilterAllClicked\0FilterNoneClicked\0"
    "ShowContextMenu\0pos\0Accepted"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SelectMethodDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x08 /* Private */,
       4,    0,   52,    2, 0x08 /* Private */,
       5,    0,   53,    2, 0x08 /* Private */,
       6,    0,   54,    2, 0x08 /* Private */,
       7,    0,   55,    2, 0x08 /* Private */,
       8,    1,   56,    2, 0x08 /* Private */,
      10,    0,   59,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint,    9,
    QMetaType::Void,

       0        // eod
};

void SelectMethodDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SelectMethodDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->StageChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->FilterChanged(); break;
        case 2: _t->SearchStringChanged(); break;
        case 3: _t->FilterAllClicked(); break;
        case 4: _t->FilterNoneClicked(); break;
        case 5: _t->ShowContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 6: _t->Accepted(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SelectMethodDialog::staticMetaObject = { {
    &QDialog::staticMetaObject,
    qt_meta_stringdata_SelectMethodDialog.data,
    qt_meta_data_SelectMethodDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SelectMethodDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SelectMethodDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SelectMethodDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int SelectMethodDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
