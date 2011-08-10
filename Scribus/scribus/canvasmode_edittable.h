/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef CANVASMODE_EDITTABLE_H
#define CANVASMODE_EDITTABLE_H

#include <QCursor>
#include <QObject>

#include "canvasmode.h"
#include "cellarea.h"

class CanvasGesture;
class CellSelect;
class ColumnResize;
class PageItem_Table;
class QMouseEvent;
class QPainter;
class RowResize;
class ScribusView;
class TableResize;

/**
 * This is a canvas mode for editing tables.
 */
class CanvasMode_EditTable :  public CanvasMode
{
	Q_OBJECT

public:
	explicit CanvasMode_EditTable(ScribusView* view);
	virtual ~CanvasMode_EditTable();

	virtual void activate(bool fromGesture);
	virtual void deactivate(bool forGesture);
	virtual bool handleKeyEvents() { return true; }
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void drawControls(QPainter* p);

private:
	/// Table being edited.
	PageItem_Table *m_table;

	/// The "select row" cursor.
	QCursor m_selectRowCursor;
	/// The "select column" cursor.
	QCursor m_selectColumnCursor;

	/// Gesture for resizing a table.
	TableResize* m_tableResizeGesture;
	/// Gesture for resizing a row.
	RowResize* m_rowResizeGesture;
	/// Gesture for resizing a column.
	ColumnResize* m_columnResizeGesture;
	/// Gesture for selecting cells.
	CellSelect* m_cellSelectGesture;
};

#endif // CANVASMODE_EDITTABLE_H
