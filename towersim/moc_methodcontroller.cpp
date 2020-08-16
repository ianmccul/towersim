/****************************************************************************
** Meta object code from reading C++ file 'methodcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "methodcontroller.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'methodcontroller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MethodController[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   18,   17,   17, 0x08,
      56,   17,   17,   17, 0x08,
      71,   17,   17,   17, 0x08,
      85,   17,   17,   17, 0x08,
      99,   17,   17,   17, 0x08,
     115,   17,   17,   17, 0x08,
     132,  130,   17,   17, 0x08,
     153,   17,   17,   17, 0x08,
     171,   17,   17,   17, 0x08,
     192,   17,   17,   17, 0x08,
     217,  209,   17,   17, 0x08,
     244,  130,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MethodController[] = {
    "MethodController\0\0Value\0"
    "NumberOfBells_valueChanged(int)\0"
    "RingingStart()\0RingingAuto()\0RingingStop()\0"
    "RingingRounds()\0RingingStand()\0n\0"
    "CCButtonClicked(int)\0CallModeChanged()\0"
    "SelectMethodPushed()\0MethodGoPushed()\0"
    "Checked\0MethodGoDelayToggled(bool)\0"
    "CallClicked(int)\0"
};

void MethodController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MethodController *_t = static_cast<MethodController *>(_o);
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

const QMetaObjectExtraData MethodController::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MethodController::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MethodController,
      qt_meta_data_MethodController, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MethodController::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MethodController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MethodController::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MethodController))
        return static_cast<void*>(const_cast< MethodController*>(this));
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
    }
    return _id;
}
QT_END_MOC_NAMESPACE
