
#include "DocxFactory/opc/OpcPackage.h"
#include "DocxFactory/opc/OpcPart.h"
#include "DocxFactory/opc/OpcImageCache.h"
#include "DocxFactory/opc/OpcImageFile.h"
#include "DocxFactory/opc/OpcFunc.h"

#include "DocxFactory/zip/ZipFile.h"
#include "DocxFactory/os/OsFunc.h"

#include "DocxFactory/str/StrFunc.h"
#include "DocxFactory/util/DocxFactoryDefs.h"

#include "minizip/zip.h"

#include <cstring>

using namespace DocxFactory;
using namespace std;



OpcImageFile::OpcImageFile(
    OpcImageCache  *p_imageCache,
    const string   &p_sourceFullPath,
    const string   &p_fileExt)
{
    string  l_ext;
    bool    l_convert = false;

    if (p_fileExt == ".jpeg")   l_ext = ".jpeg";
    else if (p_fileExt == ".jpg")        l_ext = ".jpeg";
    else if (p_fileExt == ".jpe")        l_ext = ".jpeg";
    else if (p_fileExt == ".jfif")   l_ext = ".jpeg";
    else if (p_fileExt == ".png")        l_ext = ".png";
    else if (p_fileExt == ".gif")        l_ext = ".gif";
    else if (p_fileExt == ".tiff")   l_ext = ".tiff";
    else if (p_fileExt == ".tif")        l_ext = ".tiff";
    else if (p_fileExt == ".bmp")      { l_ext = ".png";  l_convert = true; }
    else if (p_fileExt == ".dib")      { l_ext = ".png";  l_convert = true; }
    else                                { l_ext = ".jpeg"; l_convert = true; }

    m_package           = p_imageCache ->getPackage();
    m_imageCache        = p_imageCache;
    m_sourceFullPath    = p_sourceFullPath;
    m_ext               = l_ext;
    m_converted         = false;

    m_imageSource       = SOURCE_EXTERNAL_FILE;
    m_imageBuf          = NULL;
    m_imageBufSize      = 0;

    m_emuWidth          = 0;
    m_emuHeight         = 0;
} // c'tor

OpcImageFile::~OpcImageFile()
{
    if (m_imageBuf)
        delete[] m_imageBuf;
} // d'tor

void OpcImageFile::save(const string &p_partFullPath)
{
    if (m_imageSource == SOURCE_EXTERNAL_FILE)
        m_package ->getZipFile() ->addEntryFromFile(m_sourceFullPath,
                                                    OpcFunc::opcToZipPath(p_partFullPath));

    else
        m_package ->getZipFile() ->addEntryFromBuf(
            OpcFunc::opcToZipPath(p_partFullPath), m_imageBuf, m_imageBufSize);
} // save



OpcPackage *OpcImageFile::getPackage() const
{
    return m_package;
} // getPackage

OpcImageCache *OpcImageFile::getImageCache() const
{
    return m_imageCache;
} // getImageCache

string OpcImageFile::getSourceFullPath() const
{
    return m_sourceFullPath;
} // getSourceFullPath

string OpcImageFile::getExt() const
{
    return m_ext;
} // getExt

bool OpcImageFile::isConverted() const
{
    return m_converted;
} // isConverted

unsigned int OpcImageFile::getEmuWidth() const
{
    // if the image is not yet loaded into the object, the first
    // call to this function will have to load it using loadImage()
    // which is a non-const member function. we want to keep getEmuWidth()
    // a const function because the change in the state of the object
    // doesn't concern the user of the object, so we cast the const away to call loadImage().
    // the same applies for getEmuHeight().


    return m_emuWidth;
} // getPixelWidth

unsigned int OpcImageFile::getEmuHeight() const
{

    return m_emuHeight;
} // getPixelHeight
