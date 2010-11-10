#include <dmzQtVersion.h>
#include <dmzRuntimeVersion.h>
#include <QtGui/QAction>
#include <QtGui/QPixmap>
#include <ui_dmzQtVersion.h>


struct dmz::QtVersion::State {

   QPixmap pix;
   Ui::QtVersionForm ui;
   Version version;

   State (const Config &Source, const String &Prefix) : version (Source, Prefix) {;}
};


dmz::QtVersion::QtVersion (QWidget *parent, Config &local, const String &Prefix) :
      QDialog (parent),
      _state (*(new State (local, Prefix))) {

   _state.ui.setupUi (this);

   const String Name (_state.version.get_name ());
   const String Build (_state.version.get_build ());
   const String Image (_state.version.get_image_name ());
   const String Version (_state.version.get_version ());

   if (Image) {

      _state.pix.load (Image.get_buffer ());
      if (!_state.pix.isNull ()) { _state.ui.imageLabel->setPixmap (_state.pix); }
   }

   if (Name) { _state.ui.nameLabel->setText (Name.get_buffer ()); }
   if (Version) { _state.ui.versionLabel->setText (Version.get_buffer ()); }
   if (Build) { _state.ui.buildLabel->setText (Build.get_buffer ()); }

   // hit Ctrl+V to display a aboutQt message box
   QAction *action = new QAction (this);
   action->setShortcut (Qt::CTRL + Qt::Key_V);
   addAction (action);
   connect (action, SIGNAL (triggered ()), qApp, SLOT (aboutQt ()));
}


dmz::QtVersion::~QtVersion () { delete &_state; }

void DataFileEngine::setFileName(const QString &file)
{
    currentFilename = file;
}

bool DataFileEngine::open(QIODevice::OpenMode flags)
{
    if (currentFilename.isEmpty()) {
        qWarning("QResourceFileEngine::open: Missing file name");
        return false;
    }
    if(flags & QIODevice::WriteOnly)
        return false;
    if(!exists())
       return false;
    buffer = fileEngine->getArchiveFile(currentFilename);
    offset = 0;
    return true;
}

bool DataFileEngine::exists() const
{
    return !buffer.isNull() || fileEngine->exists(currentFilename);
}

bool DataFileEngine::close()
{
    offset = 0;
    buffer.clear();
    Q_ASSERT(buffer.isNull());
    return true;
}

bool DataFileEngine::flush()
{
    return true;
}

qint64 DataFileEngine::read(char *data, qint64 len)
{
    if(len > size() - offset)
        len = size() - offset;
    if(len <= 0)
        return 0;
    memcpy(data, buffer.data() + offset, len);
    offset += len;
    return len;
}

qint64 DataFileEngine::write(const char *, qint64)
{
    return -1;
}

bool DataFileEngine::remove()
{
    return false;
}

bool DataFileEngine::copy(const QString &)
{
    return false;
}

bool DataFileEngine::rename(const QString &)
{
    return false;
}

bool DataFileEngine::link(const QString &)
{
    return false;
}

qint64 DataFileEngine::size() const
{
    if(!exists())
        return 0;
    return fileEngine->size(currentFilename);
}

qint64 DataFileEngine::pos() const
{
    return offset;
}

bool DataFileEngine::atEnd() const
{
    if(!exists())
        return true;
    return offset == size();
}

bool DataFileEngine::seek(qint64 pos)
{
    if(!exists())
        return false;

    if(offset > size())
        return false;
    offset = pos;
    return true;
}

bool DataFileEngine::isSequential() const
{
    return false;
}

QAbstractFileEngine::FileFlags DataFileEngine::fileFlags(QAbstractFileEngine::FileFlags type) const
{
    QAbstractFileEngine::FileFlags ret = 0;
    if(!exists())
        return ret;

    if(type & PermsMask)
        ret |= QAbstractFileEngine::FileFlags(ReadOwnerPerm|ReadUserPerm|ReadGroupPerm|ReadOtherPerm);
    if(type & TypesMask) {
        ret |= FileType; // Directories are not mapped correctly
    }
    if(type & FlagsMask) {
        ret |= ExistsFlag;
    }
    return ret;
}

bool DataFileEngine::setPermissions(uint)
{
    return false;
}

QString DataFileEngine::fileName(FileName file) const
{
    if(file == BaseName) {
        int slash = currentFilename.lastIndexOf(QLatin1Char('/'));
        if (slash == -1)
            return currentFilename;
        return currentFilename.mid(slash + 1);
    } else if(file == PathName || file == AbsolutePathName) {
        const int slash = currentFilename.lastIndexOf(QLatin1Char('/'));
        if (slash != -1)
            return currentFilename.left(slash);
    } else if(file == CanonicalName || file == CanonicalPathName) {
        const QString absoluteFilePath = currentFilename;
        if(file == CanonicalPathName) {
            const int slash = absoluteFilePath.lastIndexOf(QLatin1Char('/'));
            if (slash != -1)
                return absoluteFilePath.left(slash);
        }
        return absoluteFilePath;
    }
    return currentFilename;
}

bool DataFileEngine::isRelativePath() const
{
    return false;
}

uint DataFileEngine::ownerId(FileOwner) const
{
    static const uint nobodyID = (uint) -2;
    return nobodyID;
}

QString DataFileEngine::owner(FileOwner) const
{
    return QString();
}

QDateTime DataFileEngine::fileTime(FileTime) const
{
    return QDateTime();
}

QAbstractFileEngine::Iterator *DataFileEngine::beginEntryList(QDir::Filters filters,
                                                                   const QStringList &filterNames)
{
    // return new QResourceFileEngineIterator(filters, filterNames);
    return 0; // ...
}

QAbstractFileEngine::Iterator *DataFileEngine::endEntryList()
{
    return 0;
}

bool DataFileEngine::extension(Extension extension, const ExtensionOption *option, ExtensionReturn *output)
{
    if (extension == MapExtension) {
        const MapExtensionOption *options = (MapExtensionOption*)(option);
        MapExtensionReturn *returnValue = static_cast<MapExtensionReturn*>(output);
        returnValue->address = map(options->offset, options->size, options->flags);
        return (returnValue->address != 0);
    }
    if (extension == UnMapExtension) {
        return true; // No unmapping needs to be done.
    }
    return false;
}

uchar *DataFileEngine::map(qint64 offset, qint64 size, QFile::MemoryMapFlags flags)
{
    Q_UNUSED(flags);
    if (offset < 0 || size <= 0 || !exists() || offset + size > buffer.size()) {
        setError(QFile::UnspecifiedError, QString());
        return 0;
    }
    uchar *address = reinterpret_cast<uchar *>(buffer.data());
    return (address + offset);
}

bool DataFileEngine::supportsExtension(Extension extension) const
{
    return (extension == UnMapExtension || extension == MapExtension);
}
