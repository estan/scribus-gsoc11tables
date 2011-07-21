/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef CANVASMODE_EDITTABLE_H
#define CANVASMODE_EDITTABLE_H

#include <QObject>

#include "canvasmode.h"

class ScribusView;
class QKeyEvent;

/**
 * This is a canvas mode for editing tables.
 */
class CanvasMode_EditTable :  public CanvasMode
{
	Q_OBJECT

public:
	explicit CanvasMode_EditTable(ScribusView* view) : CanvasMode(view) {}

	virtual void activate(bool fromGesture);
	virtual void deactivate(bool forGesture);
	virtual void keyPressEvent(QKeyEvent* event);
};

#endif // CANVASMODE_EDITTABLE_H
