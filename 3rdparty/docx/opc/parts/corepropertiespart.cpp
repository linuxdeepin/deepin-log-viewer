#include "corepropertiespart.h"

using namespace Docx;

CorePropertiesPart::CorePropertiesPart(const QString &partName, const QString &contentType
                                       , const QByteArray &blob, Package *package)
    : XmlPart(partName, contentType, blob, package)
{

}

void CorePropertiesPart::load(const QString &partName, const QString &contentType, const QByteArray &blob, Package *package)
{
    Q_UNUSED(partName)
    Q_UNUSED(contentType)
    Q_UNUSED(blob)
    Q_UNUSED(package)
}

CorePropertiesPart::~CorePropertiesPart()
{

}



void Docx::CorePropertiesPart::afterUnmarshal()
{
}
