/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef CANVASGESTURE_ROWRESIZE_H
#define CANVASGESTURE_ROWRESIZE_H

#include "canvasgesture.h"
#include "scribusview.h"

class CanvasMode;
class PageItem_Table;
class QMouseEvent;
class QKeyEvent;

/**
 * Handles resizing of table rows on behalf of the table editing canvas mode.
 *
 * The user presses the mouse button on the boundary at the bottom of a row, drags it
 * to a new location and then releases the button. The resize may be cancelled by
 * pressing the Escape key.
 */
class SCRIBUS_API RowResize : public CanvasGesture
{
	Q_OBJECT

public:
	explicit RowResize(CanvasMode* parent) : CanvasGesture(parent), m_table(0), m_row(0) {}
	virtual ~RowResize() {}

	virtual void activate(bool fromGesture);
	virtual void deactivate(bool forGesture) {}
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual bool handleKeyEvents() { return true; }

	/// Sets the row that should be resized.
	void setRow(int row) { m_row = row; }

private:
	/// Table being edited.
	PageItem_Table* m_table;
	/// Row being resized.
	int m_row;
	/// Height of the row before the resizing started.
	qreal m_oldRowHeight;
};

#endif // CANVASGESTURE_ROWRESIZE_H
