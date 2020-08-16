/****************************************************************************
** Meta object code from reading C++ file 'sensorinputserver.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "sensorinputserver.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sensorinputserver.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SensorInputServer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   19,   18,   18, 0x08,
      48,   18,   18,   18, 0x08,
      64,   19,   18,   18, 0x08,
      90,   18,   18,   18, 0x08,
      98,   19,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SensorInputServer[] = {
    "SensorInputServer\0\0Socket\0"
    "ReadCommand(QObject*)\0NewConnection()\0"
    "RetryConnection(QObject*)\0Retry()\0"
    "CompleteConnection(QObject*)\0"
};

void SensorInputServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SensorInputServer *_t = static_cast<SensorInputServer *>(_o);
        switch (_id) {
        case 0: _t->ReadCommand((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 1: _t->NewConnection(); break;
        case 2: _t->RetryConnection((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 3: _t->Retry(); break;
        case 4: _t->CompleteConnection((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SensorInputServer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SensorInputServer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SensorInputServer,
      qt_meta_data_SensorInputServer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SensorInputServer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SensorInputServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SensorInputServer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SensorInputServer))
        return static_cast<void*>(const_cast< SensorInputServer*>(this));
    return QObject::qt_metacast(_clname);
}

int SensorInputServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
