#include <dmzFoundationReaderWriterZip.h>
#include <dmzQtZipFileEngineHandler.h>
#include <dmzQtZipFileEngine.h>
#include <dmzRuntimeLog.h>
#include <dmzTypesStringContainer.h>


struct dmz::QtZipFileEngineHandler::State {
   
   ReaderZip zip;
   StringContainer files;
   Log *log;
};


dmz::QtZipFileEngineHandler::QtZipFileEngineHandler (
      const String &ArchiveName,
      Log *log) :
      QAbstractFileEngineHandler (),
      _state (*(new State)) {

   _state.log = log;

   if (is_zip_file (ArchiveName)) {

      if (_state.zip.open_zip_file (ArchiveName)) {

         _state.zip.get_file_list (_state.files);

         if (_state.log) {

            _state.log->info
               << "QtFileEngineHandler for: " << ArchiveName
               << " (" << _state.files.get_count () << " files)"
               << endl;
         }
      }
   }
   else {

      if (_state.log) {

         _state.log->warn << "QtFileEngineHandle: unsupported file format: "
                          << ArchiveName
                          << endl;
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

   if (_state.log) {

      _state.log->warn << "create: " << FileName << endl;
   }

   if (_state.files.contains (FileName)) {

      engine = new QtZipFileEngine (FileName, _state.zip, _state.log);
   }
   
   return engine;
}

