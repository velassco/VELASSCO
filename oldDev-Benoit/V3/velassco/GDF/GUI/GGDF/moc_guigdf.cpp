/****************************************************************************
** Meta object code from reading C++ file 'guigdf.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "guigdf.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'guigdf.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_GuiGDF_t {
    QByteArrayData data[10];
    char stringdata[104];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GuiGDF_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GuiGDF_t qt_meta_stringdata_GuiGDF = {
    {
QT_MOC_LITERAL(0, 0, 6),
QT_MOC_LITERAL(1, 7, 14),
QT_MOC_LITERAL(2, 22, 0),
QT_MOC_LITERAL(3, 23, 13),
QT_MOC_LITERAL(4, 37, 12),
QT_MOC_LITERAL(5, 50, 10),
QT_MOC_LITERAL(6, 61, 11),
QT_MOC_LITERAL(7, 73, 10),
QT_MOC_LITERAL(8, 84, 10),
QT_MOC_LITERAL(9, 95, 8)
    },
    "GuiGDF\0startAllButton\0\0stopAllButton\0"
    "statusButton\0pingButton\0writeButton\0"
    "grepButton\0readButton\0lsButton"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GuiGDF[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08 /* Private */,
       3,    0,   55,    2, 0x08 /* Private */,
       4,    0,   56,    2, 0x08 /* Private */,
       5,    0,   57,    2, 0x08 /* Private */,
       6,    0,   58,    2, 0x08 /* Private */,
       7,    0,   59,    2, 0x08 /* Private */,
       8,    0,   60,    2, 0x08 /* Private */,
       9,    0,   61,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GuiGDF::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GuiGDF *_t = static_cast<GuiGDF *>(_o);
        switch (_id) {
        case 0: _t->startAllButton(); break;
        case 1: _t->stopAllButton(); break;
        case 2: _t->statusButton(); break;
        case 3: _t->pingButton(); break;
        case 4: _t->writeButton(); break;
        case 5: _t->grepButton(); break;
        case 6: _t->readButton(); break;
        case 7: _t->lsButton(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject GuiGDF::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_GuiGDF.data,
      qt_meta_data_GuiGDF,  qt_static_metacall, 0, 0}
};


const QMetaObject *GuiGDF::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GuiGDF::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GuiGDF.stringdata))
        return static_cast<void*>(const_cast< GuiGDF*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int GuiGDF::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
