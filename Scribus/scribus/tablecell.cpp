/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include "tablecell.h"

#include "pageitem_table.h"

TableCell::TableCell(int row, int column, PageItem_Table *table) : d(new TableCellData)
{
	setValid(true);
	setRow(row);
	setColumn(column);
	setRowSpan(1);
	setColumnSpan(1);

	d->table = table;
}
