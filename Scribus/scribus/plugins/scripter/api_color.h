/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef API_COLOR_H_
#define API_COLOR_H_

#include <QObject>
#include <QtDebug>
#include <QApplication>

#include "scripterimpl.h"

class ColorAPI : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QVariant> getColorNames READ getColorNames);
public:
    ColorAPI(QObject *parent);
    virtual ~ColorAPI();

public slots:
    // XXX: put your methods here

private:
    QList<QVariant> getColorNames();
    

};

#endif /*API_COLOR_H_*/
