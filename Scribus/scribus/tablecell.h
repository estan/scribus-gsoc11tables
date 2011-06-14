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
#include <QSharedData>

class PageItem_Table;
class CellStyle;

/**
 * Shared data class for TableCell.
 */
class TableCellData : public QSharedData
{
public:
	TableCellData() : isValid(false), row(-1), column(-1), rowSpan(-1), columnSpan(-1), table(0) {}
	TableCellData(const TableCellData& other) : QSharedData(other), isValid(other.isValid), row(other.row),
		column(other.column), rowSpan(other.rowSpan), columnSpan(other.columnSpan) {}
	~TableCellData() {}

public:
	bool isValid;
	int row;
	int column;
	int rowSpan;
	int columnSpan;
	QString style;
	PageItem_Table *table;
};

/**
 * The TableCell class represents a cell in a table.
 *
 * It provides an interface for setting and getting cell properties. A cell may be
 * marked as invalid if the row or column it is in is removed or if it is covered by
 * a spanning cell. The valid state of a cell may be queried for using the
 * <code>isValid()</code> function.
 */
class TableCell
{
public:
	/// Construct a new table cell as a shallow copy of @a other.
	TableCell(const TableCell& other) : d(other.d) {}

	/**
	 * Returns <code>true</code> if this cell is valid.
	 */
	bool isValid() const { return d->isValid && d->table; }

	/**
	 * Returns the number of the row in the table that contains this cell, or <code>-1</code> if
	 * the cell is invalid.
	 */
	int row() const { return isValid() ? d->row : -1; }

	/**
	 * Returns the number of the column in the table that contains this cell, or <code>-1</code> if
	 * the cell is invalid.
	 */
	int column() const { return isValid() ? d->column : -1; }

	/**
	 * Returns the number of rows this cell spans, or <code>-1</code> if the cell is invalid.
	 */
	int rowSpan() const { return isValid() ? d->rowSpan : -1; }

	/**
	 * Returns the number of columns this cell spans, or <code>-1</code> if the cell is invalid.
	 */
	int columnSpan() const { return isValid() ? d->columnSpan : -1; }

	/**
	 * Sets the cell style for this cell to @a style, or do nothing if the cell is invalid.
	 */
	void setStyle(const QString& style) { if (isValid()) d->style = style; }

	/**
	 * Returns the cell style for this cell, or QString() if the cell is invalid.
	 */
	QString style() const { return isValid() ? d->style : QString(); }

private:
	/**
	 * Construct a new valid table cell at @a row, @a column in the table @a table, spanning
	 * @a rowSpan rows and @a columnSpan columns.
	 */
	TableCell(int row, int column, PageItem_Table *table);
	/// Constructs a new invalid table cell.
	TableCell() : d(new TableCellData) { setValid(false); }

	/// Set the row of the table that contains this cell to @a row.
	void setRow(int row) { d->row = row; }
	/// Set the column of the table that contains this cell to @a row.
	void setColumn(int column) { d->column = column; }
	/// Set the number of rows this cell is spanning to @a rowSpan.
	void setRowSpan(int rowSpan) { d->rowSpan = rowSpan; }
	/// Set the number of columns this cell is spanning to @a columnSpan.
	void setColumnSpan(int columnSpan) { d->columnSpan = columnSpan; }
	/// Sets the valid state of the cell to @a isValid.
	void setValid(bool isValid) { d->isValid = isValid; }

private:
	friend class PageItem_Table;
	QSharedDataPointer<TableCellData> d;
};

#endif // TABLECELL_H
