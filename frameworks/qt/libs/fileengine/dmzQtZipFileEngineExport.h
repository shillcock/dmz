#ifndef DMZ_QT_ZIP_FILE_ENGINE_EXPORT_DOT_H
#define DMZ_QT_ZIP_FILE_ENGINE_EXPORT_DOT_H

#ifdef _WIN32
#   ifdef DMZ_QT_ZIP_FILE_ENGINE_EXPORT
#      define DMZ_QT_ZIP_FILE_ENGINE_LINK_SYMBOL __declspec (dllexport)
#   else
#      define DMZ_QT_ZIP_FILE_ENGINE_LINK_SYMBOL __declspec (dllimport)
#   endif
#else
#   define DMZ_QT_ZIP_FILE_ENGINE_LINK_SYMBOL
#endif

#endif // DMZ_QT_ZIP_FILE_ENGINE_EXPORT_DOT_H
