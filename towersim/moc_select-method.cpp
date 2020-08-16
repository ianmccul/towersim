/****************************************************************************
** Meta object code from reading C++ file 'select-method.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "select-method.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'select-method.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SelectMethodDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   20,   19,   19, 0x08,
      44,   19,   19,   19, 0x08,
      60,   19,   19,   19, 0x08,
      82,   19,   19,   19, 0x08,
     101,   19,   19,   19, 0x08,
     125,  121,   19,   19, 0x08,
     149,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SelectMethodDialog[] = {
    "SelectMethodDialog\0\0Stage\0StageChanged(int)\0"
    "FilterChanged()\0SearchStringChanged()\0"
    "FilterAllClicked()\0FilterNoneClicked()\0"
    "pos\0ShowContextMenu(QPoint)\0Accepted()\0"
};

void SelectMethodDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SelectMethodDialog *_t = static_cast<SelectMethodDialog *>(_o);
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

const QMetaObjectExtraData SelectMethodDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SelectMethodDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SelectMethodDialog,
      qt_meta_data_SelectMethodDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SelectMethodDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SelectMethodDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SelectMethodDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SelectMethodDialog))
        return static_cast<void*>(const_cast< SelectMethodDialog*>(this));
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
    }
    return _id;
}
QT_END_MOC_NAMESPACE
