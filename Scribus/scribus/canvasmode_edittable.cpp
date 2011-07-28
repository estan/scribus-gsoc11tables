/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QDebug>
#include <QTransform>
#include <QPointF>

#include "canvas.h"
#include "fpoint.h"
#include "pageitem_table.h"
#include "scribusdoc.h"
#include "selection.h"
#include "util_icon.h"

#include "canvasmode_edittable.h"

void CanvasMode_EditTable::activate(bool fromGesture)
{
	qDebug() << "CanvasMode_EditTable::activate(" << fromGesture << ")";

	m_table = m_doc->m_Selection->itemAt(0)->asTable();
}

void CanvasMode_EditTable::deactivate(bool forGesture)
{
	qDebug() << "CanvasMode_EditTable::deactivate(" << forGesture << ")";
}

void CanvasMode_EditTable::keyPressEvent(QKeyEvent* event)
{
	qDebug() << "CanvasMode_EditTable::keyPressEvent()";
}

void CanvasMode_EditTable::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_table)
		return;

	FPoint canvasPoint = m_canvas->globalToCanvas(event->globalPos());
	QPointF canvasQPoint(canvasPoint.x(), canvasPoint.y());

	// Set an appropriate cursor.
	QCursor cursor(Qt::ArrowCursor);
	switch (m_table->hitTest(canvasQPoint))
	{
		case PageItem_Table::Top:
			cursor = loadIcon("select_column.png");
			break;
		case PageItem_Table::Left:
			cursor = loadIcon("select_row.png");
			break;
		case PageItem_Table::BottomLeft:
			cursor = Qt::SizeFDiagCursor;
			break;
		case PageItem_Table::ColumnRight:
			cursor = Qt::SizeHorCursor;
			break;
		case PageItem_Table::RowBottom:
			cursor = Qt::SizeVerCursor;
			break;
		case PageItem_Table::Cell:
		case PageItem_Table::Outside:
			break;
		default:
			qWarning("Unknown hit target");
			break;
	}
	qApp->changeOverrideCursor(cursor);
}
