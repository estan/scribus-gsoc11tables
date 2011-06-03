/*
 * Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>
 *
 * For general Scribus (>=1.3.2) copyright and licensing information please refer
 * to the COPYING file provided with the program. Following this notice may exist
 * a copyright and/or license notice that predates the release of Scribus 1.3.2
 * for which a new license (GPL+exception) is in place.
 */

#include <QtGlobal>

#include "cellarea.h"

CellArea::CellArea() :
	m_row(-1), m_column(-1), m_width(-1), m_height(-1)
{
}

CellArea::CellArea(int row, int column, int width, int height) :
	m_row(row), m_column(column), m_width(width), m_height(height)
{
}

bool CellArea::contains(int row, int column) const
{
	if (!isValid())
		return false;

	return row >= this->row() && row <= bottom() &&
			column >= this->column() && column <= right();
}

bool CellArea::contains(const CellArea& area) const
{
	if (!isValid() && !area.isValid())
		return false;

	return area.column() >= column() && area.right() <= right() &&
			area.row() >= row() && area.bottom() <= bottom();
}

bool CellArea::intersects(const CellArea& area)
{
	if (!isValid() && !area.isValid())
		return false;

	// !(outside) || contains(area)
	return !(area.column() > right() || area.right() < column() ||
			 area.bottom() < row() || area.row() > bottom()) || contains(area);
}

CellArea CellArea::translated(int rows, int columns) const
{
	CellArea area(row() + rows, column() + columns, width(), height());
	return area;
}

void CellArea::translate(int rows, int columns)
{
	m_row += rows;
	m_column += columns;
}

CellArea CellArea::adjusted(int rows, int columns, int width, int height) const
{
	CellArea area(row() + rows, column() + columns, this->width() + width, this->height() + height);
	return area;
}

CellArea CellArea::united(CellArea& area) const
{
	int newRow = qMin(row(), area.row());
	int newColumn = qMin(column(), area.column());
	int newWidth = qMax(right() - newColumn + 1, area.right() - newColumn + 1);
	int newHeight = qMax(bottom() - newRow + 1, area.bottom() - newRow + 1);
	CellArea unitedArea(newRow, newColumn, newWidth, newHeight);
	return unitedArea;
}

bool operator==(const CellArea& lhs, const CellArea& rhs)
{
	return lhs.row() == rhs.row() && lhs.column() == rhs.column() &&
			lhs.width() == rhs.width() && lhs.height() == rhs.height();
}

bool operator!=(const CellArea& lhs, const CellArea& rhs)
{
	return lhs.row() != rhs.row() || lhs.column() != rhs.column() ||
			lhs.width() != rhs.width() || lhs.height() != rhs.height();
}
