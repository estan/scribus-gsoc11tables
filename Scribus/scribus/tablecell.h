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
#include <QDebug>
#include <QExplicitlySharedDataPointer>
#include <QSharedData>

#include "commonstrings.h"
#include "text/frect.h"
#include "styles/cellstyle.h"

class PageItem_Table;
class CellStyle;
class TableCell;
class ScPainter;

/**
 * Shared data class for TableCell.
 */
class TableCellData : public QSharedData
{
public:
	/// Construct data for an invalid cell.
	TableCellData() :
		isValid(false),
		row(-1),
		column(-1),
		rowSpan(-1),
		columnSpan(-1),
		table(0) {}
	/// Construct data from data in @a other.
	TableCellData(const TableCellData& other) : QSharedData(other),
		isValid(other.isValid),
		row(other.row),
		column(other.column),
		rowSpan(other.rowSpan),
		columnSpan(other.columnSpan),
		style(other.style),
		table(other.table) {}
	~TableCellData() {}

public:
	/// Cell valid state.
	bool isValid;
	/// Row of the table the cell is in.
	int row;
	/// Column of the table the cell is in.
	int column;
	/// Number of rows the cell spans.
	int rowSpan;
	/// Number of columns the cell spans.
	int columnSpan;

	/// Style of the cell.
	CellStyle style;
	/// Table containing the cell.
	PageItem_Table *table;
};

/**
 * The TableCell class represents a cell in a table.
 * <p>
 * It provides an interface for setting and getting cell properties. A cell may be
 * marked as invalid if the row or column containing the cell is removed. The valid
 * state of a cell may be queried using the <code>isValid()</code> function.
 */
class TableCell
{
public:
	/// Construct a new table cell as a shallow copy of @a other.
	TableCell(const TableCell& other) : d(other.d) {}

	/// Returns <code>true</code> if this cell is valid.
	bool isValid() const { return d->isValid && d->table; }

	/// Returns the row in the table that contains this cell.
	int row() const { return d->row; }

	/// Returns the column in the table that contains this cell.
	int column() const { return d->column; }

	/// Returns the number of rows this cell spans.
	int rowSpan() const { return d->rowSpan; }

	/// Returns the number of columns this cell spans.
	int columnSpan() const { return d->columnSpan; }

	/// Sets the background color of this cell to @a color.
	void setBackgroundColor(const QString& color) { d->style.setBackgroundColor(color); }

	/// Returns the background color of this cell.
	QString backgroundColor() const { return d->style.backgroundColor(); }

	/// Sets the width of the left border of this cell to @a width.
	void setLeftBorderWidth(qreal width) { d->style.setLeftBorderWidth(width); }

	/// Returns the width of the left border of this cell.
	qreal leftBorderWidth() const { return d->style.leftBorderWidth(); }

	/// Sets the width of the right border of this cell to @a width.
	void setRightBorderWidth(qreal width) { d->style.setRightBorderWidth(width); }

	/// Returns the width of the right border of this cell.
	qreal rightBorderWidth() const { return d->style.rightBorderWidth(); }

	/// Sets the width of the top border of this cell to @a width.
	void setTopBorderWidth(qreal width) { d->style.setTopBorderWidth(width); }

	/// Returns the width of the top border of this cell.
	qreal topBorderWidth() const { return d->style.topBorderWidth(); }

	/// Sets the width of the bottom border of this cell to @a width.
	void setBottomBorderWidth(qreal width) { d->style.setBottomBorderWidth(width); }

	/// Returns the width of the bottom border of this cell.
	qreal bottomBorderWidth() const { return d->style.bottomBorderWidth(); }

	/// Sets the color of the left border of this cell to @a color.
	void setLeftBorderColor(const QString& color) { d->style.setLeftBorderColor(color); }

	/// Returns the color of the left border of this cell.
	QString leftBorderColor() const { return d->style.leftBorderColor(); }

	/// Sets the color of the right border of this cell to @a color.
	void setRightBorderColor(const QString& color) { d->style.setRightBorderColor(color); }

	/// Returns the color of the right border of this cell.
	QString rightBorderColor() const { return d->style.rightBorderColor(); }

	/// Sets the color of the top border of this cell to @a color.
	void setTopBorderColor(const QString& color) { d->style.setTopBorderColor(color); }

	/// Returns the color of the top border of this cell.
	QString topBorderColor() const { return d->style.topBorderColor(); }

	/// Sets the color of the bottom border of this cell to @a color.
	void setBottomBorderColor(const QString& color) { d->style.setBottomBorderColor(color); }

	/// Returns the color of the bottom border of this cell.
	QString bottomBorderColor() const { return d->style.bottomBorderColor(); }

	/// Sets the cell style for this cell to @a style.
	void setStyle(const QString& style) { d->style.setParent(style); }

	/// Returns the named cell style for this cell.
	QString style() const { return d->style.parent(); }

	/// Returns the cell as a string. Useful for debugging. The format is subject to change.
	QString asString() const;

private:
	/**
	 * Construct a new valid table cell at @a row, @a column in the table @a table.
	 * The new cell will span <code>1</code> row and <code>1</code> column.
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

	/// "Move" the cell down by @a numRows. E.g. increase its row by @a numRows.
	void moveDown(int numRows) { d->row += numRows; }
	/// "Move" the cell up by @a numRows. E.g. decrease its row by @a numRows.
	void moveUp(int numRows) { d->row -= numRows; }
	/// "Move" the cell right by @a numColumns. E.g. increase its column by @a numColumns.
	void moveRight(int numColumns) { d->column += numColumns; }
	/// "Move" the cell left by @a numColumns. E.g. decrease its column by @a numColumns.
	void moveLeft(int numColumns) { d->column -= numColumns; }

	void drawLeftBorder(ScPainter *p) const;
	void drawRightBorder(ScPainter *p) const;
	void drawTopBorder(ScPainter *p) const;
	void drawBottomBorder(ScPainter *p) const;
	void drawBackground(ScPainter *p) const;

private:
	friend class PageItem_Table;
	QExplicitlySharedDataPointer<TableCellData> d;
};

QDebug operator<<(QDebug debug, const TableCell& cell);

#endif // TABLECELL_H
