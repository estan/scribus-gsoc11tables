/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef COLLAPSEDBORDERCOLLECTOR_H
#define COLLAPSEDBORDERCOLLECTOR_H

#include <QList>

#include "tablebordercollector.h"
#include "tablecell.h"

class CollapsedBorderCollector : public TableBorderCollector
{
public:
	/**
	 * Constructs a new border collector configured to collect borders from @a table.
	 */
	explicit CollapsedBorderCollector(PageItem_Table *table) : TableBorderCollector(table) {}

	/// Collects the left border of @a cell in @a verticalBorders.
	void collectLeftBorder(QList<TableBorder>& verticalBorders, TableCell& cell);
	/// Collects the right border of @a cell in @a verticalBorders.
	void collectRightBorder(QList<TableBorder>& verticalBorders, TableCell& cell);
	/// Collects the top border of @a cell in @a horizontalBorders.
	void collectTopBorder(QList<TableBorder>& horizontalBorders, TableCell& cell);
	/// Collects the bottom border of @a cell in @a horizontalBorders.
	void collectBottomBorder(QList<TableBorder>& horizontalBorders, TableCell& cell);
};

#endif // COLLAPSEDBORDERCOLLECTOR_H
