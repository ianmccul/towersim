/****************************************************************************
** Meta object code from reading C++ file 'bellsim-mainwindow.h'
**
** Created: Sun Jun 24 12:07:17 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "bellsim-mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'bellsim-mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BellsimMainWindow[] = {

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
      25,   19,   18,   18, 0x08,
      68,   63,   18,   18, 0x08,
      97,   19,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_BellsimMainWindow[] = {
    "BellsimMainWindow\0\0Value\0"
    "ChangesPerMinute_valueChanged(double)\0"
    "Time\0PealSpeed_timeChanged(QTime)\0"
    "NumberOfBells_valueChanged(int)\0"
};

void BellsimMainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BellsimMainWindow *_t = static_cast<BellsimMainWindow *>(_o);
        switch (_id) {
        case 0: _t->ChangesPerMinute_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->PealSpeed_timeChanged((*reinterpret_cast< QTime(*)>(_a[1]))); break;
        case 2: _t->NumberOfBells_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BellsimMainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BellsimMainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_BellsimMainWindow,
      qt_meta_data_BellsimMainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BellsimMainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BellsimMainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BellsimMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BellsimMainWindow))
        return static_cast<void*>(const_cast< BellsimMainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int BellsimMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
