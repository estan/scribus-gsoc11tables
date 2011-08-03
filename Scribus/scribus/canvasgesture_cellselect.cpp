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
#include "scribusview.h"
#include "tableutils.h"

#include "canvasgesture_cellselect.h"

void CellSelect::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
	{
		// Cancel the resize.
		event->accept();

		// TODO: Clear the selection.

		m_view->stopGesture();
	}
}

void CellSelect::mousePressEvent(QMouseEvent* event)
{
	event->accept();

	m_view->registerMousePress(event->globalPos());

	// TODO: Select single cell.
}

void CellSelect::mouseReleaseEvent(QMouseEvent* event)
{
	event->accept();
}

void CellSelect::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_view->moveTimerElapsed())
		return; // This was just a click.

	event->accept();

	// TODO: Select area of cells.
}

void CellSelect::drawControls(QPainter* p)
{
	p->save();
	commonDrawControls(p, false);
	p->restore();

	// TODO: Draw selection overlay.
	// Idea: Use a union of QPainterPaths of slightly expanded cell rects.
}

void CellSelect::setup(PageItem_Table* table)
{
	Q_ASSERT(table);

	setTable(table);
}
