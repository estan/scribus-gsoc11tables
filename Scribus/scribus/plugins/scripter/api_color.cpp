/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#include "api_color.h"
#include "sccolorengine.h"
#include "scribusdoc.h"
#include "scribuscore.h"
#include "commonstrings.h"
#include "sccolor.h"

ColorAPI::ColorAPI(QObject *parent) : QObject(COLLECTOR)
{
    qDebug() << "ColorAPI loaded";
    setObjectName("colors");
}

/**
 * Scripter.activeDocument.color.getColorNames
 * Property
 * Return a list containing the names of all defined colors in the document
 * if no document is open, returns a  list of the default document colors
 */
QList<QVariant> ColorAPI::getColorNames()
{
    ColorList names;
    QList<QVariant> colorNames;
    names = ScCore->primaryMainWindow()->HaveDoc ? ScCore->primaryMainWindow()->doc->PageColors : PrefsManager::instance()->colorSet();
    ColorList::Iterator it;
    for(it = names.begin(); it != names.end(); ++it)
    {
        colorNames.append(qVariantFromValue(it.key()));
    }
    return colorNames;
}

ColorAPI::~ColorAPI()
{
    qDebug() << "ColorAPI deleted";
}



