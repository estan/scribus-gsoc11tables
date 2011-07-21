/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QDebug>

#include "canvasmode_edittable.h"

void CanvasMode_EditTable::activate(bool fromGesture)
{
	qDebug() << "CanvasMode_EditTable::activate(" << fromGesture << ")";
}

void CanvasMode_EditTable::deactivate(bool forGesture)
{
	qDebug() << "CanvasMode_EditTable::deactivate(" << forGesture << ")";
}

void CanvasMode_EditTable::keyPressEvent(QKeyEvent* event)
{
	qDebug() << "CanvasMode_EditTable::keyPressEvent()";
}
