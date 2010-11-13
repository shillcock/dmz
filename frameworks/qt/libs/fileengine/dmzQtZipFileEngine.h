#ifndef DMZ_QT_ZIP_FILE_ENGINE_DOT_H
#define DMZ_QT_ZIP_FILE_ENGINE_DOT_H

#include <dmzQtZipFileEngineExport.h>
#include <QtCore/QAbstractFileEngine>


namespace dmz {

   class Log;
   class ReaderZip;
   class String;

   class DMZ_QT_ZIP_FILE_ENGINE_LINK_SYMBOL QtZipFileEngine :
         public QAbstractFileEngine {

      public:
         QtZipFileEngine (const String &FileName, ReaderZip &zip, Log *log);
         virtual ~QtZipFileEngine ();

         bool caseSensitive () const;

         bool close ();

         QStringList entryList (
            QDir::Filters filters,
            const QStringList &filterNames) const;

         FileFlags fileFlags (FileFlags type) const;
         QString fileName (FileName type) const;
         QDateTime fileTime (FileTime time) const;
         bool isRelativePath () const;
//         bool isSequential () const;
         bool mkdir (const QString &DirName, bool createParentDirectories) const;
         bool open (QIODevice::OpenMode flags);
         QString owner (FileOwner owner) const;
         uint ownerId (FileOwner owner) const;
         qint64 read (char *data, qint64 len);
         bool remove ();
         bool rename (const QString &NewName);
         bool rmdir (const QString &DirName, bool recurseParentDirectories) const;
         void setFileName (const QString &File);
         bool setPermissions (uint perms);
         bool setSize (qint64 size);
//         qint64 size () const;
         qint64 write (const char *data, qint64 len);

      protected:
         struct State;
         State &_state;
   };
};

#endif // DMZ_QT_ZIP_FILE_ENGINE_DOT_H
