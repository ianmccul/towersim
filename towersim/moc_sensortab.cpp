/****************************************************************************
** Meta object code from reading C++ file 'sensortab.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "sensortab.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sensortab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SensorTab[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      25,   10,   10,   10, 0x08,
      44,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SensorTab[] = {
    "SensorTab\0\0SensorClear()\0AddSensorClicked()\0"
    "RemoveSensorClicked()\0"
};

void SensorTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SensorTab *_t = static_cast<SensorTab *>(_o);
        switch (_id) {
        case 0: _t->SensorClear(); break;
        case 1: _t->AddSensorClicked(); break;
        case 2: _t->RemoveSensorClicked(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SensorTab::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SensorTab::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SensorTab,
      qt_meta_data_SensorTab, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SensorTab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SensorTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SensorTab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SensorTab))
        return static_cast<void*>(const_cast< SensorTab*>(this));
    return QObject::qt_metacast(_clname);
}

int SensorTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
