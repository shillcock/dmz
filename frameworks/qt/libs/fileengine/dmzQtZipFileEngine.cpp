#include <dmzFoundationReaderWriterZip.h>
#include <dmzQtZipFileEngine.h>
#include <dmzRuntimeLog.h>
#include <QtCore/QDateTime>


struct dmz::QtZipFileEngine::State {

   String name;
   ReaderZip &zip;
   Log *log;

   State (const String &TheFileName, ReaderZip &theZip, Log *theLog) :
      name (TheFileName),
      zip (theZip),
      log (theLog) {;}
};


dmz::QtZipFileEngine::QtZipFileEngine (
      const String &FileName,
      ReaderZip &zip,
      Log *log) :
      QAbstractFileEngine (),
      _state (*(new State (FileName, zip, log))) {

   if (_state.log) {

      _state.log->info << "QtZipFileEngine for : " << _state.name << endl;
   }
}


dmz::QtZipFileEngine::~QtZipFileEngine () { delete &_state; }


bool
dmz::QtZipFileEngine::caseSensitive () const { return False; }


bool
dmz::QtZipFileEngine::close () { return _state.zip.close_file (); }


QStringList
dmz::QtZipFileEngine::entryList (
		QDir::Filters filters,
		const QStringList &NameFilters) const {

	return QStringList ();
}


QAbstractFileEngine::FileFlags
dmz::QtZipFileEngine::fileFlags (FileFlags type) const {

   FileFlags flags (0);

   if ((type.testFlag (FileType) || type.testFlag (ExistsFlag))) {

      flags |= FileType;
      flags |= ReadOtherPerm;
      flags |= ExistsFlag;
   }

   return flags;
}


QString
dmz::QtZipFileEngine::fileName (FileName type) const {

   QString name (_state.name.get_buffer ());
   Int32 slash = name.lastIndexOf ("/");

   switch (type) {

      case BaseName:
         if (slash != -1) { name.remove (0, slash + 1); }
         break;

      case PathName:
      case AbsolutePathName:
         if (slash != -1) { name = name.left (slash); }
         break;

      default:
         break;
   }

    return name;
}


QDateTime
dmz::QtZipFileEngine::fileTime (FileTime time) const { return QDateTime (); }


bool
dmz::QtZipFileEngine::isRelativePath () const { return False; }


bool
dmz::QtZipFileEngine::mkdir (const QString &, bool createParentDirectories) const {

   return False;
}


//bool
//dmz::QtZipFileEngine::isSequential() const { return True; }


bool
dmz::QtZipFileEngine::open (QIODevice::OpenMode flags) {

   bool result (False);
   if (flags & QIODevice::ReadOnly) { result = _state.zip.open_file (_state.name); }
   return result;
}


QString
dmz::QtZipFileEngine::owner (FileOwner owner) const { return QString (); }


uint
dmz::QtZipFileEngine::ownerId (FileOwner owner) const { return -2; }


qint64
dmz::QtZipFileEngine::read (char *data, qint64 len) {

_state.log->error << "read: " << len << endl;
   return _state.zip.read_file (data, len);
}


bool
dmz::QtZipFileEngine::remove () { return False; }


bool
dmz::QtZipFileEngine::rename (const QString &) { return False; }


bool
dmz::QtZipFileEngine::rmdir (
      const QString &DirName,
      bool recurseParentDirectories) const {

   return False;
}


void
dmz::QtZipFileEngine::setFileName (const QString &TheFileName) {

   if (_state.log) {

      _state.log->warn << "setFileName not supported" << endl;
   }

//   _state.name = qPrintable (TheFileName);
}


bool
dmz::QtZipFileEngine::setPermissions (uint perms) { return false; }


bool
dmz::QtZipFileEngine::setSize (qint64 size) { return false; }


//qint64
//dmz::QtZipFileEngine::size () const {

//   return (qint64)_state.zip.get_file_size ();
//}


qint64
dmz::QtZipFileEngine::write (const char *data, qint64 len) { return -1; }
