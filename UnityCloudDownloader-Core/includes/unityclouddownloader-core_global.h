#ifndef UCD_CORE_GLOBAL_H
#define UCD_CORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UCD_CORE_LIBRARY)
#  define UCD_SHARED_EXPORT Q_DECL_EXPORT
#else
#  define UCD_SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // UCD_CORE_GLOBAL_H
