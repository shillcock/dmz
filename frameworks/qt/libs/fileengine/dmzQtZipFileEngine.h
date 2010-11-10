#ifndef DMZ_QT_ZIP_FILE_ENGINE_DOT_H
#define DMZ_QT_ZIP_FILE_ENGINE_DOT_H

#include <dmzQtZipFileEngineExport.h>
#include <QtCore/QAbstractFileEngine>


namespace dmz {

   class DMZ_QT_ZIP_FILE_ENGINE_LINK_SYMBOL QtZipFileEngine : public QAbstractFileEngine {

      public:
         QtZipFileEngine (const String &FileName, ReaderZip &zip);
         virtual ~QtZipFileEngine ();

         void setFileName (const QString &File);
         Iterator *endEntryList();
         uchar *map (qint64 offset, qint64 size, QFile::MemoryMapFlags flags);
         bool extension (Extension extension, const ExtensionOption *option, ExtensionReturn *output);
         bool supportsExtension (Extension extension) const;
         bool open (QIODevice::OpenMode flags);
         bool close ();
         bool flush ();
         qint64 read (char *data, qint64 len);
         qint64 write (const char *, qint64);
         bool remove ();
         bool copy (const QString &);
         bool rename (const QString &);
         bool link (const QString &);
         qint64 size () const;
         qint64 pos () const;
         bool atEnd () const;
         bool seek (qint64 pos);
         bool isSequential () const;
         FileFlags fileFlags (FileFlags type) const;
         bool setPermissions (uint);
         QString fileName (FileName file) const;
         bool isRelativePath () const;
         uint ownerId (FileOwner) const;
         QString owner (FileOwner) const;
         QDateTime fileTime (FileTime) const;
         Iterator *beginEntryList (QDir::Filters filters, const QStringList &FilterNames);

      protected:
         struct State;
         State &_state;
   };
};

#endif // DMZ_QT_ZIP_FILE_ENGINE_DOT_H
