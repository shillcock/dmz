#include <dmzFoundationReaderWriterZip.h>
#include <dmzQtZipFileEngine.h>
#include <QtCore/QDateTime>


struct dmz::QtZipFileEngine::State {

   String name;
   ReaderZip &zip;

   State (const String &TheFileName, ReaderZip &theZip) :
      name (TheFileName),
      zip (theZip) {;}
};


dmz::QtZipFileEngine::QtZipFileEngine (const String &TheFileName, ReaderZip &theZip) :
      QAbstractFileEngine (),
      _state (*(new State (TheFileName, theZip))) {

}


dmz::QtZipFileEngine::~QtZipFileEngine () {

   delete &_state;
}


bool
dmz::QtZipFileEngine::caseSensitive () const {

   return false;
}


bool
dmz::QtZipFileEngine::close () {

	return _state.zip.close_file ();
}


QStringList
dmz::QtZipFileEngine::entryList (
		QDir::Filters filters,
		const QStringList &NameFilters) const {

	QStringList result;
	return result;
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

   QString result;
   QFileInfo fi (_state.name.get_buffer ());

   switch (type) {

      case BaseName: result = fi.baseName (); break;
      case PathName: result = fi.path (); break;
      case AbsoluteName: result = fi.absoluteFilePath (); break;
      case AbsolutePathName: result = fi.absolutePath (); break;
      default: result = fi.filePath (); break;
    }

    return result;
}


QDateTime
dmz::QtZipFileEngine::fileTime (FileTime time) const {

   QDateTime result;
   return result;
}


bool
dmz::QtZipFileEngine::isRelativePath () const {

    return false;
}


bool
dmz::QtZipFileEngine::mkdir (const QString &, bool createParentDirectories) const {

   return false;
}


//bool
//dmz::QtZipFileEngine::isSequential() const {

//    return true;
//}


bool
dmz::QtZipFileEngine::open (QIODevice::OpenMode flags) {

   bool result (False);

   if (flags & QIODevice::ReadOnly) {

      result = _state.zip.open_file (_state.name);
   }

   return result;
}


QString
dmz::QtZipFileEngine::owner (FileOwner owner) const {

   QString result;
   return result;
}


uint
dmz::QtZipFileEngine::ownerId (FileOwner owner) const {

   uint result (-2);
   return result;
}


qint64
dmz::QtZipFileEngine::read (char *data, qint64 len) {

   return _state.zip.read_file (data, len);
}


bool
dmz::QtZipFileEngine::remove () {

    return false;
}


bool
dmz::QtZipFileEngine::rename (const QString &) {

    return false;
}


bool
dmz::QtZipFileEngine::rmdir (
      const QString &DirName,
      bool recurseParentDirectories) const {

   return false;
}


void
dmz::QtZipFileEngine::setFileName (const QString &TheFileName){

   _state.name = qPrintable (TheFileName);
}


bool
dmz::QtZipFileEngine::setPermissions (uint perms) {

    return false;
}


bool
dmz::QtZipFileEngine::setSize (qint64 size) {

   return false;
}


qint64
dmz::QtZipFileEngine::size () const {

   return (qint64)_state.zip.get_file_size ();
}


qint64
dmz::QtZipFileEngine::write (const char *data, qint64 len) {

    return -1;
}
