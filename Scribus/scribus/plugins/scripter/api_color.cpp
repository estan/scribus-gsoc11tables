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
 * Scripter.activeDocument.colors.getColorNames
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

/**
 * Scripter.activeDocument.colors.getColor(name)
 * Function
 * Returns a list [C, M, Y, K] containing the four color components of the
 * color \"name\" from the current document. If no document is open, returns
 * the value of the named color from the default document colors.
 * May raise NotFoundError if the named color wasn't found.\n\
 * May raise ValueError if an invalid color name is specified.\n\
 * @todo This now returns a list; make it a tuple.
 */
QList<QVariant> ColorAPI::getColor(QString name)
{
    ColorList colors;
    int c, m, y, k;
    if(name.isEmpty())
    {
        RAISE("Cannot get a color with an empty name.");
    }
    colors = ScCore->primaryMainWindow()->HaveDoc ? ScCore->primaryMainWindow()->doc->PageColors : PrefsManager::instance()->colorSet();
    ScribusDoc* currentDoc = ScCore->primaryMainWindow()->HaveDoc ? ScCore->primaryMainWindow()->doc : NULL;
    if(!colors.contains(name))
    {
        RAISE("Color not found");
    }
    CMYKColor cmykValues;
    ScColorEngine::getCMYKValues(colors[name], currentDoc, cmykValues);
    cmykValues.getValues(c, m, y, k);
    QList<QVariant> cmyk;
    cmyk.append(qVariantFromValue(c));
    cmyk.append(qVariantFromValue(m));
    cmyk.append(qVariantFromValue(y));
    cmyk.append(qVariantFromValue(k));
    return cmyk;
}

/**
 * Scripter.activeDocument.colors.getColorAsRGB(name)
 * Returns a list [R,G,B] containing the three color components of the
 * color \"name\" from the current document, converted to the RGB color
 * space. If no document is open, returns the value of the named color
 * from the default document colors.
 * May raise NotFoundError if the named color wasn't found.
 * May raise ValueError if an invalid color name is specified.
 */
QList<QVariant> ColorAPI::getColorAsRGB(QString name)
{
    ColorList colors;
    if(name.isEmpty())
        RAISE("Cannot get a color with an empty name");
    colors = ScCore->primaryMainWindow()->HaveDoc ? ScCore->primaryMainWindow()->doc->PageColors : PrefsManager::instance()->colorSet();
    ScribusDoc* currentDoc = ScCore->primaryMainWindow()->HaveDoc ? ScCore->primaryMainWindow()->doc : NULL;
    if(!colors.contains(name))
        RAISE("Color not found");
    QColor rgb = ScColorEngine::getRGBColor(colors[name], currentDoc);
    QList<QVariant> rgbList;
    rgbList.append(rgb.red());
    rgbList.append(rgb.green());
    rgbList.append(rgb.blue());
    return rgbList;
}

ColorAPI::~ColorAPI()
{
    qDebug() << "ColorAPI deleted";
}



