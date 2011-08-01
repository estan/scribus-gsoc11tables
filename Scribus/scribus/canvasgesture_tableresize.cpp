/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <algorithm>

#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPointF>

#include "canvas.h"
#include "fpoint.h"
#include "pageitem.h"
#include "pageitem_table.h"
#include "selection.h"
#include "tableutils.h"

#include "canvasgesture_tableresize.h"

void TableResize::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
	{
		// Cancel the resize.
		event->accept();
		m_view->stopGesture();
	}
}

void TableResize::mouseReleaseEvent(QMouseEvent* event)
{
	event->accept();

	// Snap to grid and guides.
	FPoint canvasPoint = m_doc->ApplyGridF(m_canvas->globalToCanvas(event->globalPos()));
	m_doc->ApplyGuides(&canvasPoint);

	// Convert to table grid coordinates.
	QPointF gridPoint = m_table->getTransform().inverted().map(canvasPoint.toQPointF()) - m_table->gridOffset();

	// Perform the actual resize of the table.
	m_table->resize(gridPoint.x(), gridPoint.y());
	m_table->update();

	m_view->stopGesture();
}

void TableResize::mouseMoveEvent(QMouseEvent* event)
{
	event->accept();

	// Snap to grid and guides.
	FPoint canvasPoint = m_doc->ApplyGridF(m_canvas->globalToCanvas(event->globalPos()));
	m_doc->ApplyGuides(&canvasPoint);

	// Convert to table grid coordinates.
	QPointF gridPoint = m_table->getTransform().inverted().map(canvasPoint.toQPointF()) - m_table->gridOffset();

	// Set the column  geometries for the table outline.
	qreal requestedWidthFactor = gridPoint.x() / m_table->tableWidth();
	qreal newMinWidth = qMax(m_minWidth * requestedWidthFactor, PageItem_Table::MinimumColumnWidth);
	qreal actualWidthFactor = newMinWidth / m_minWidth;
	for (int col = 0; col < m_columnWidths.size(); ++col)
	{
		m_columnWidths[col] = m_table->columnWidth(col) * actualWidthFactor;
		m_columnPositions[col] = m_table->columnPosition(col) * actualWidthFactor;
	}

	// Set the row geometries for the table outline.
	qreal requestedHeightFactor = gridPoint.y() / m_table->tableHeight();
	qreal newMinHeight = qMax(m_minHeight * requestedHeightFactor, PageItem_Table::MinimumRowHeight);
	qreal actualHeightFactor = newMinHeight / m_minHeight;
	for (int row = 0; row < m_rowHeights.size(); ++row)
	{
		m_rowHeights[row] = m_table->rowHeight(row) * actualHeightFactor;
		m_rowPositions[row] = m_table->rowPosition(row) * actualHeightFactor;
	}

	m_canvas->update();
}

void TableResize::drawControls(QPainter* p)
{
	p->save();
	commonDrawControls(p, false);
	p->restore();

	p->save();
	p->scale(m_canvas->scale(), m_canvas->scale());
	p->translate(-m_doc->minCanvasCoordinate.x(), -m_doc->minCanvasCoordinate.y());
	p->setTransform(m_table->getTransform(), true);

	// Paint the table outline using the changed table geometries.
	TableUtils::paintOutline(m_table, m_rowHeights, m_rowPositions,
		m_columnWidths, m_columnPositions, m_canvas, p);

	p->restore();
}

void TableResize::setup(PageItem_Table* table)
{
	Q_ASSERT(table);

	m_table = table;

	// Make copies of the row and column geometries to be used during resize.
	m_rowHeights = m_table->rowHeights();
	m_rowPositions = m_table->rowPositions();
	m_columnWidths = m_table->columnWidths();
	m_columnPositions = m_table->columnPositions();

	// Save the minimum row height and column width.
	m_minHeight = *std::min_element(m_rowHeights.begin(), m_rowHeights.end());
	m_minWidth = *std::min_element(m_columnWidths.begin(), m_columnWidths.end());
}
