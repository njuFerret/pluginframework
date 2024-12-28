#pragma once

#include <qglobal.h>

#if defined(PLUGINSYSTEM_LIBRARY)
#  define PLUGINSYSTEM_EXPORT Q_DECL_EXPORT
#else
#  define PLUGINSYSTEM_EXPORT Q_DECL_IMPORT
#endif
