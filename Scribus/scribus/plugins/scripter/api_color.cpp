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

ColorAPI::ColorAPI(QObject *parent) : QObject(COLLECTOR)
{
    qDebug() << "ColorAPI loaded";
    setObjectName("colors");
}



ColorAPI::~ColorAPI()
{
    qDebug() << "ColorAPI deleted";
}



