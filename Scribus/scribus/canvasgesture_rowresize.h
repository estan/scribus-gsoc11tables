/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef CANVASGESTURE_ROWRESIZE_H
#define CANVASGESTURE_ROWRESIZE_H

#include <QList>

#include "canvasgesture.h"
#include "scribusview.h"

class CanvasMode;
class PageItem_Table;
class QMouseEvent;
class QKeyEvent;
class QPainter;

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

	virtual void activate(bool fromGesture) {};
	virtual void deactivate(bool forGesture) {}
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual bool handleKeyEvents() { return true; }
	virtual void drawControls(QPainter* p);

	/// Sets up this row resize gesture to resize @a row in @a table.
	void setup(PageItem_Table* table, int row);

private:
	/// Table being edited.
	PageItem_Table* m_table;
	/// Row being resized.
	int m_row;
	/// List of row heights for the table outline.
	QList<qreal> m_rowHeights;
	/// List of row positions for the table outline.
	QList<qreal> m_rowPositions;
};

#endif // CANVASGESTURE_ROWRESIZE_H
