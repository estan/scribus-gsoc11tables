/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPointF>

#include "canvas.h"
#include "fpoint.h"
#include "pageitem.h"
#include "pageitem_table.h"
#include "scribusview.h"
#include "selection.h"
#include "tableutils.h"

#include "canvasgesture_rowresize.h"

void RowResize::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
	{
		// Cancel the resize.
		event->accept();
		m_view->stopGesture();
	}
}

void RowResize::mouseReleaseEvent(QMouseEvent* event)
{
	event->accept();

	QPointF gridPoint = globalToTableGrid(event->globalPos());

	// Perform the actual resize of the row.
	table()->setRowHeight(m_row, gridPoint.y() - table()->rowPosition(m_row));
	table()->update();

	m_view->stopGesture();
}

void RowResize::mouseMoveEvent(QMouseEvent* event)
{
	event->accept();

	QPointF gridPoint = globalToTableGrid(event->globalPos());

	// Set height of row for the table outline.
	qreal rowPosition = m_rowPositions[m_row];
	qreal requestedHeight = gridPoint.y() - rowPosition;
	qreal actualHeight = qMax(PageItem_Table::MinimumRowHeight, requestedHeight);
	m_rowHeights[m_row] = actualHeight;

	// Set positions of following rows.
	for (int row = m_row; row < m_rowPositions.size(); ++row)
	{
		m_rowPositions[row] = rowPosition;
		rowPosition += m_rowHeights[row];
	}

	// Display height tooltip.
	m_canvas->displayDoubleHUD(event->globalPos(), tr("Height"), actualHeight);

	m_canvas->update();
}

void RowResize::drawControls(QPainter* p)
{
	p->save();
	commonDrawControls(p, false);
	p->restore();

	// Paint the table outline using the changed row geometries.
	paintTableOutline(m_rowHeights, m_rowPositions,
		table()->columnWidths(), table()->columnPositions(), p);
}

void RowResize::setup(PageItem_Table* table, int row)
{
	Q_ASSERT(table);
	Q_ASSERT(row >= 0 && row < table->rows());

	setTable(table);
	m_row = row;

	// Make copies of the row geometries to be used during resize.
	m_rowHeights = table->rowHeights();
	m_rowPositions = table->rowPositions();
}
