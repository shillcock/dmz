#include <dmzFoundationReaderWriterZip.h>
#include <dmzQtZipFileEngineHandler.h>
// #include <dmzQtZipFileEngine.h>
#include <dmzTypesStringContainer.h>


struct dmz::QtZipFileEngineHandler::State {
   
   ReaderZip zip;
   StringContainer files;
};


dmz::QtZipFileEngineHandler::QtZipFileEngineHandler (const String &ArchiveName) :
      QAbstractFileEngineHandler (),
      _state (*(new State)) {

   if (is_zip_file (ArchiveName)) {
      
      if (_state.zip.open_zip_file (ArchiveName)) {
         
         _state.zip.get_file_list (_state.files);
      }
   }
}


dmz::QtZipFileEngineHandler::~QtZipFileEngineHandler () {
   
   delete &_state;
}


QAbstractFileEngine *
dmz::QtZipFileEngineHandler::create (const QString &TheFileName) const {
   
   QAbstractFileEngine *engine (0);
   
   const String FileName (qPrintable (TheFileName));
   
   if (_state.files.contains (FileName)) {
      
      // engine = new QtZipFileEngine (FileName, _zip);
   }
   
   return engine;
}

