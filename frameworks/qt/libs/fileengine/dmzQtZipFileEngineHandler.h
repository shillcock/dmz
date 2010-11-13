#ifndef DMZ_QT_ZIP_FILE_ENGINE_HANDLER_DOT_H
#define DMZ_QT_ZIP_FILE_ENGINE_HANDLER_DOT_H

#include <dmzQtZipFileEngineExport.h>
#include <dmzTypesString.h>
#include <QtCore/QAbstractFileEngineHandler>


namespace dmz {

   class Log;

   class DMZ_QT_ZIP_FILE_ENGINE_LINK_SYMBOL QtZipFileEngineHandler :
         public QAbstractFileEngineHandler {

      public:
         QtZipFileEngineHandler (const String &ArchiveName, Log *log = 0);
         virtual ~QtZipFileEngineHandler ();

         QAbstractFileEngine *create (const QString &Filename) const;

      protected:
         struct State;
         State &_state;
   };
};


#endif // DMZ_QT_ZIP_FILE_ENGINE_HANDLER_DOT_H
