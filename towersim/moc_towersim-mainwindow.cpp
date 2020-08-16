/****************************************************************************
** Meta object code from reading C++ file 'towersim-mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "towersim-mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'towersim-mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TowerSimMainWindow[] = {

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
      26,   20,   19,   19, 0x08,
      69,   64,   19,   19, 0x08,
      98,   20,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TowerSimMainWindow[] = {
    "TowerSimMainWindow\0\0Value\0"
    "ChangesPerMinute_valueChanged(double)\0"
    "Time\0PealSpeed_timeChanged(QTime)\0"
    "NumberOfBells_valueChanged(int)\0"
};

void TowerSimMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TowerSimMainWindow *_t = static_cast<TowerSimMainWindow *>(_o);
        switch (_id) {
        case 0: _t->ChangesPerMinute_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->PealSpeed_timeChanged((*reinterpret_cast< QTime(*)>(_a[1]))); break;
        case 2: _t->NumberOfBells_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TowerSimMainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TowerSimMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_TowerSimMainWindow,
      qt_meta_data_TowerSimMainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TowerSimMainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TowerSimMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TowerSimMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TowerSimMainWindow))
        return static_cast<void*>(const_cast< TowerSimMainWindow*>(this));
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
    }
    return _id;
}
QT_END_MOC_NAMESPACE
