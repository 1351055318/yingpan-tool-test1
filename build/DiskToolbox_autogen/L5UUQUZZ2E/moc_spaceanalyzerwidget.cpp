/****************************************************************************
** Meta object code from reading C++ file 'spaceanalyzerwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/spaceanalyzer/spaceanalyzerwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'spaceanalyzerwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DirSizeWorker_t {
    QByteArrayData data[12];
    char stringdata0[100];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DirSizeWorker_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DirSizeWorker_t qt_meta_stringdata_DirSizeWorker = {
    {
QT_MOC_LITERAL(0, 0, 13), // "DirSizeWorker"
QT_MOC_LITERAL(1, 14, 11), // "resultReady"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 4), // "path"
QT_MOC_LITERAL(4, 32, 4), // "size"
QT_MOC_LITERAL(5, 37, 9), // "fileCount"
QT_MOC_LITERAL(6, 47, 8), // "dirCount"
QT_MOC_LITERAL(7, 56, 8), // "progress"
QT_MOC_LITERAL(8, 65, 11), // "currentPath"
QT_MOC_LITERAL(9, 77, 5), // "level"
QT_MOC_LITERAL(10, 83, 8), // "finished"
QT_MOC_LITERAL(11, 92, 7) // "process"

    },
    "DirSizeWorker\0resultReady\0\0path\0size\0"
    "fileCount\0dirCount\0progress\0currentPath\0"
    "level\0finished\0process"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DirSizeWorker[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   34,    2, 0x06 /* Public */,
       7,    2,   43,    2, 0x06 /* Public */,
      10,    0,   48,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    0,   49,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::LongLong, QMetaType::Int, QMetaType::Int,    3,    4,    5,    6,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    8,    9,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void DirSizeWorker::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DirSizeWorker *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->resultReady((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 1: _t->progress((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: _t->finished(); break;
        case 3: _t->process(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DirSizeWorker::*)(const QString & , qint64 , int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirSizeWorker::resultReady)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (DirSizeWorker::*)(const QString & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirSizeWorker::progress)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (DirSizeWorker::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DirSizeWorker::finished)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DirSizeWorker::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_DirSizeWorker.data,
    qt_meta_data_DirSizeWorker,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DirSizeWorker::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DirSizeWorker::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DirSizeWorker.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DirSizeWorker::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void DirSizeWorker::resultReady(const QString & _t1, qint64 _t2, int _t3, int _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t4))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DirSizeWorker::progress(const QString & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void DirSizeWorker::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_SpaceAnalyzerWidget_t {
    QByteArrayData data[24];
    char stringdata0[327];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SpaceAnalyzerWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SpaceAnalyzerWidget_t qt_meta_stringdata_SpaceAnalyzerWidget = {
    {
QT_MOC_LITERAL(0, 0, 19), // "SpaceAnalyzerWidget"
QT_MOC_LITERAL(1, 20, 24), // "onVolumeSelectionChanged"
QT_MOC_LITERAL(2, 45, 0), // ""
QT_MOC_LITERAL(3, 46, 5), // "index"
QT_MOC_LITERAL(4, 52, 19), // "onScanButtonClicked"
QT_MOC_LITERAL(5, 72, 19), // "onStopButtonClicked"
QT_MOC_LITERAL(6, 92, 16), // "onDirResultReady"
QT_MOC_LITERAL(7, 109, 4), // "path"
QT_MOC_LITERAL(8, 114, 4), // "size"
QT_MOC_LITERAL(9, 119, 9), // "fileCount"
QT_MOC_LITERAL(10, 129, 8), // "dirCount"
QT_MOC_LITERAL(11, 138, 14), // "onScanProgress"
QT_MOC_LITERAL(12, 153, 11), // "currentPath"
QT_MOC_LITERAL(13, 165, 5), // "level"
QT_MOC_LITERAL(14, 171, 14), // "onScanFinished"
QT_MOC_LITERAL(15, 186, 17), // "onTreeItemClicked"
QT_MOC_LITERAL(16, 204, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(17, 221, 4), // "item"
QT_MOC_LITERAL(18, 226, 6), // "column"
QT_MOC_LITERAL(19, 233, 18), // "onTreeItemExpanded"
QT_MOC_LITERAL(20, 252, 13), // "onPathChanged"
QT_MOC_LITERAL(21, 266, 22), // "onRefreshButtonClicked"
QT_MOC_LITERAL(22, 289, 21), // "onExportButtonClicked"
QT_MOC_LITERAL(23, 311, 15) // "onFilterChanged"

    },
    "SpaceAnalyzerWidget\0onVolumeSelectionChanged\0"
    "\0index\0onScanButtonClicked\0"
    "onStopButtonClicked\0onDirResultReady\0"
    "path\0size\0fileCount\0dirCount\0"
    "onScanProgress\0currentPath\0level\0"
    "onScanFinished\0onTreeItemClicked\0"
    "QTreeWidgetItem*\0item\0column\0"
    "onTreeItemExpanded\0onPathChanged\0"
    "onRefreshButtonClicked\0onExportButtonClicked\0"
    "onFilterChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SpaceAnalyzerWidget[] = {

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
       6,    4,   79,    2, 0x08 /* Private */,
      11,    2,   88,    2, 0x08 /* Private */,
      14,    0,   93,    2, 0x08 /* Private */,
      15,    2,   94,    2, 0x08 /* Private */,
      19,    1,   99,    2, 0x08 /* Private */,
      20,    0,  102,    2, 0x08 /* Private */,
      21,    0,  103,    2, 0x08 /* Private */,
      22,    0,  104,    2, 0x08 /* Private */,
      23,    0,  105,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::LongLong, QMetaType::Int, QMetaType::Int,    7,    8,    9,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   12,   13,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 16, QMetaType::Int,   17,   18,
    QMetaType::Void, 0x80000000 | 16,   17,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SpaceAnalyzerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SpaceAnalyzerWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->onVolumeSelectionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->onScanButtonClicked(); break;
        case 2: _t->onStopButtonClicked(); break;
        case 3: _t->onDirResultReady((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< qint64(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4]))); break;
        case 4: _t->onScanProgress((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->onScanFinished(); break;
        case 6: _t->onTreeItemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->onTreeItemExpanded((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 8: _t->onPathChanged(); break;
        case 9: _t->onRefreshButtonClicked(); break;
        case 10: _t->onExportButtonClicked(); break;
        case 11: _t->onFilterChanged(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SpaceAnalyzerWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_SpaceAnalyzerWidget.data,
    qt_meta_data_SpaceAnalyzerWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SpaceAnalyzerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpaceAnalyzerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SpaceAnalyzerWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int SpaceAnalyzerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
