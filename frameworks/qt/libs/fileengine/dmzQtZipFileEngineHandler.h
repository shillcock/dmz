#ifndef DMZ_QT_ZIP_FILE_ENGINE_HANDLER_DOT_H
#define DMZ_QT_ZIP_FILE_ENGINE_HANDLER_DOT_H

#include <dmzQtZipFileEngineExport.h>
#include <QtCore/QAbstractFileEngineHandler>

// class QAbstractFileEngine;


namespace dmz {

   class DMZ_QT_ZIP_FILE_ENGINE_LINK_SYMBOL QtZipFileEngineHandler :
         public QAbstractFileEngineHandler {

      public:
         QtZipFileEngineHandler (const String &ArchiveName);
         virtual ~QtZipFileEngineHandler ();

         QAbstractFileEngine *create (const QString &Filename) const;

      protected:
         struct State;
         State &_state;
   };
};


#endif // DMZ_QT_ZIP_FILE_ENGINE_HANDLER_DOT_H
