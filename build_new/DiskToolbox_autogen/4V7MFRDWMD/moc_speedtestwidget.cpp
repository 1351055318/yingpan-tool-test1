/****************************************************************************
** Meta object code from reading C++ file 'speedtestwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/speedtest/speedtestwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'speedtestwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SpeedTestWidget_t {
    QByteArrayData data[11];
    char stringdata0[166];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SpeedTestWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SpeedTestWidget_t qt_meta_stringdata_SpeedTestWidget = {
    {
QT_MOC_LITERAL(0, 0, 15), // "SpeedTestWidget"
QT_MOC_LITERAL(1, 16, 22), // "onDiskSelectionChanged"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 5), // "index"
QT_MOC_LITERAL(4, 46, 18), // "onStartTestClicked"
QT_MOC_LITERAL(5, 65, 19), // "onCancelTestClicked"
QT_MOC_LITERAL(6, 85, 18), // "onBlockSizeChanged"
QT_MOC_LITERAL(7, 104, 5), // "value"
QT_MOC_LITERAL(8, 110, 17), // "onFileSizeChanged"
QT_MOC_LITERAL(9, 128, 22), // "onExportResultsClicked"
QT_MOC_LITERAL(10, 151, 14) // "updateProgress"

    },
    "SpeedTestWidget\0onDiskSelectionChanged\0"
    "\0index\0onStartTestClicked\0onCancelTestClicked\0"
    "onBlockSizeChanged\0value\0onFileSizeChanged\0"
    "onExportResultsClicked\0updateProgress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SpeedTestWidget[] = {

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
       6,    1,   54,    2, 0x08 /* Private */,
       8,    1,   57,    2, 0x08 /* Private */,
       9,    0,   60,    2, 0x08 /* Private */,
      10,    0,   61,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SpeedTestWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SpeedTestWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onDiskSelectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->onStartTestClicked(); break;
        case 2: _t->onCancelTestClicked(); break;
        case 3: _t->onBlockSizeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->onFileSizeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->onExportResultsClicked(); break;
        case 6: _t->updateProgress(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SpeedTestWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_SpeedTestWidget.data,
    qt_meta_data_SpeedTestWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SpeedTestWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpeedTestWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SpeedTestWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SpeedTestWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
