/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#ifndef TABLECELL_H
#define TABLECELL_H

#include <QString>

class PageItem_Table;
class CellStyle;

/**
 * The TableCell class represents a cell in a table.
 *
 * The class provides an interface for setting and getting cell properties.
 *
 * If the table structure is changed after a cell has been retrieved from the
 * table, trying to use the cell will result in undefined behavior. This may
 * change in the future.
 */
class TableCell {
public:
	/// Returns <code>true</code> if this cell is valid.
	bool isValid() const { return m_table != 0; }
	/// Returns the row span of this cell.
	int rowSpan() const { return m_rowSpan; }
	/// Returns the column span of this cell.
	int columnSpan() const { return m_columnSpan; }
	/// Sets the cell style for this cell to @a style.
	void setStyle(const QString& style) { m_style = style; }
	/// Returns the cell style for this cell.
	QString style() const { return m_style; }

private:
	/// Construct a new cell with parent table @a table.
	TableCell(PageItem_Table *table);
	/// Set the row span of this cell to @a rowSpan.
	void setRowSpan(int rowSpan) { m_rowSpan = rowSpan; }
	/// Set the column span of this cell to @a columnSpan.
	void setColumnSpan(int columnSpan) { m_columnSpan = columnSpan; }

private:
	friend class PageItem_Table;
	PageItem_Table *m_table;
	QString m_style;
	int m_rowSpan;
	int m_columnSpan;
};

#endif // TABLECELL_H
