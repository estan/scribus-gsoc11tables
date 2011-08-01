/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QCursor>
#include <QDebug>
#include <QPointF>

#include "canvas.h"
#include "canvasgesture_rowresize.h"
#include "cellarea.h"
#include "fpoint.h"
#include "pageitem_table.h"
#include "scribusdoc.h"
#include "selection.h"
#include "util_icon.h"

#include "canvasmode_edittable.h"

CanvasMode_EditTable::CanvasMode_EditTable(ScribusView* view) : CanvasMode(view),
	m_table(0),
	m_selectRowCursor(loadIcon("select_row.png")),
	m_selectColumnCursor(loadIcon("select_column.png")),
	m_rowResizeGesture(new RowResize(this))
{
}

CanvasMode_EditTable::~CanvasMode_EditTable()
{
	delete m_rowResizeGesture;
}

void CanvasMode_EditTable::activate(bool fromGesture)
{
	PageItem *item = m_doc->m_Selection->itemAt(0);
	Q_ASSERT(item && item->isTable());
	m_table = item->asTable();

	if (fromGesture)
		qApp->changeOverrideCursor(Qt::ArrowCursor);
}

void CanvasMode_EditTable::deactivate(bool forGesture)
{
}

void CanvasMode_EditTable::mouseMoveEvent(QMouseEvent* event)
{
	event->accept();
	// Set an appropriate cursor.
	QCursor cursor(Qt::ArrowCursor);
	switch (m_table->hitTest(
		m_canvas->globalToCanvas(event->globalPos()).toQPointF(),
		m_doc->guidesPrefs().grabRadius / m_canvas->scale()).type())
	{
		case PageItem_Table::Handle::RowSelect:
			cursor = m_selectRowCursor;
			break;
		case PageItem_Table::Handle::RowResize:
			cursor = Qt::SizeVerCursor;
			break;
		case PageItem_Table::Handle::ColumnSelect:
			cursor = m_selectColumnCursor;
			break;
		case PageItem_Table::Handle::ColumnResize:
			cursor = Qt::SizeHorCursor;
			break;
		case PageItem_Table::Handle::TableResize:
			cursor = Qt::SizeFDiagCursor;
			break;
		case PageItem_Table::Handle::CellSelect:
		case PageItem_Table::Handle::None:
			break;
		default:
			qWarning("Unknown hit target");
			break;
	}
	qApp->changeOverrideCursor(cursor);
}

void CanvasMode_EditTable::mousePressEvent(QMouseEvent* event)
{
	event->accept();
	PageItem_Table::Handle handle = m_table->hitTest(
		m_canvas->globalToCanvas(event->globalPos()).toQPointF(),
		m_doc->guidesPrefs().grabRadius / m_canvas->scale());

	switch (handle.type())
	{
		case PageItem_Table::Handle::RowSelect:
			// Not implemented.
			break;
		case PageItem_Table::Handle::RowResize:
			m_rowResizeGesture->setup(m_table, handle.index());
			m_view->startGesture(m_rowResizeGesture);
			break;
		case PageItem_Table::Handle::ColumnSelect:
			// Not implemented.
			break;
		case PageItem_Table::Handle::ColumnResize:
			// Not implemented.
			break;
		case PageItem_Table::Handle::TableResize:
			// Not implemented.
			break;
		case PageItem_Table::Handle::CellSelect:
			// Not implemented.
			break;
		case PageItem_Table::Handle::None:
			// Not implemented.
			break;
		default:
			qWarning("Unknown hit target");
			break;
	}
}
