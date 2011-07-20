/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PAGEITEM_TABLE_H
#define PAGEITEM_TABLE_H

#include <QList>
#include <QRectF>
#include <QString>

#include "cellarea.h"
#include "pageitem.h"
#include "scribusapi.h"
#include "styles/tablestyle.h"
#include "tablecell.h"

class ScPainter;
class ScribusDoc;
class TablePainter;

/**
 * The PageItem_Table class represents a table.
 * <p>
 * A table is a group of cells ordered into rows and columns. Each table contains at least
 * one row and one column.
 * <p>
 * A table's dimensions can be changed by using <code>insertRows()</code>,
 * <code>insertColumns()</code>, <code>removeRows()</code>, or <code>removeColumns()</code>.
 * Use <code>cellAt()</code> to retrieve table cells.
 * <p>
 * The width and height of the table may be set using the <code>resize()</code> function.
 * <p>
 * Heights and widths of rows and columns can be set by using <code>setRowHeight()</code>
 * and <code>setColumnWidth()</code>.
 * <p>
 * Cells in the table can be merged by using <code>mergeCells()</code>. Cells that have been
 * previously merged can be split up again by using <code>splitCells()</code>.
 * <p>
 * The style of the table can be controlled by setting a named style for the table using
 * <code>setStyle()</code>. Formatting properties can also be set directly on the table.
 * Directly set properties overrides properties from the style.
 */
class SCRIBUS_API PageItem_Table : public PageItem
{
	Q_OBJECT
	Q_ENUMS(ResizeStrategy)

	Q_PROPERTY(int rows READ rows NOTIFY changed)
	Q_PROPERTY(int columns READ columns NOTIFY changed)
	Q_PROPERTY(QString fillColor READ fillColor WRITE setFillColor RESET unsetFillColor NOTIFY changed)
	Q_PROPERTY(TableBorder leftBorder READ leftBorder WRITE setLeftBorder RESET unsetLeftBorder NOTIFY changed)
	Q_PROPERTY(TableBorder rightBorder READ rightBorder WRITE setRightBorder RESET unsetRightBorder NOTIFY changed)
	Q_PROPERTY(TableBorder topBorder READ topBorder WRITE setTopBorder RESET unsetTopBorder NOTIFY changed)
	Q_PROPERTY(TableBorder bottomBorder READ bottomBorder WRITE setBottomBorder RESET unsetBottomBorder NOTIFY changed)
	Q_PROPERTY(QString style READ style WRITE setStyle RESET unsetStyle NOTIFY changed)

public:
	/**
	 * This enum specifies resize strategies for a table.
	 *
	 * With the Equal resize strategy, any width or height that is added to or removed
	 * from the table during resize is distributed equally to the columns and rows. With
	 * the Proportional resize strategy, the space distributed to a row or column is
	 * proportional to how high or wide the row or column is compared to the table width
	 * or height.
	 */
	enum ResizeStrategy
	{
		Equal,        /**< Distribute width/height equally to columns/rows. */
		Proportional  /**< Distribute width/height proportionally to columns/rows. */
	};

	/// The minimum row height.
	static const qreal MinimumRowHeight;

	/// The minimum column width.
	static const qreal MinimumColumnWidth;

public:
	/// Construct a new table item with @a numRows rows and @a numColumns columns.
	PageItem_Table(ScribusDoc *pa, double x, double y, double w, double h, double w2,
		QString fill, QString outline, int numRows = 1, int numColumns = 1);

	/// Destructor.
	~PageItem_Table();

	/// Returns the number of rows in the table.
	int rows() const { return m_rows; }

	/// Returns the number of columns in the table.
	int columns() const { return m_columns; }

	/**
	 * Returns the width of the table.
	 *
	 * This is the width of the table grid, not including any borders along the left and right side.
	 */
	qreal tableWidth() const { return m_columnPositions.last() + m_columnWidths.last(); }

	/**
	 * Returns the height of the table.
	 *
	 * This is the height of the table grid, not including any borders along the top and left side.
	 */
	qreal tableHeight() const { return m_rowPositions.last() + m_rowHeights.last(); }

	/**
	 * Returns the effective width of the table.
	 *
	 * The effective table width includes the width of the table grid plus half the width of the widest
	 * borders found along its left and right side.
	 */
	qreal effectiveWidth() const { return tableWidth() + (maxLeftBorderWidth() + maxRightBorderWidth()) / 2; }

	/**
	 * Returns the effective height of the table.
	 *
	 * The effective table height includes the height of the table grid plus half the width of the widest
	 * borders found along its top and bottom side.
	 */
	qreal effectiveHeight() const { return tableHeight() + (maxTopBorderWidth() + maxBottomBorderWidth()) / 2; }

	/**
	 * Resizes the table grid to @a width, @a height.
	 *
	 * The change in width and height will be distributed to each column and row in the table
	 * according to the resize strategy specified by @a strategy.
	 *
	 * If any row or column hits the limit MinimumRowHeight or MinimumColumnWidth, respectively,
	 * then resizing in that direction will stop for the entire table.
	 */
	void resize(qreal width, qreal height, ResizeStrategy strategy = Proportional);

	/**
	 * Inserts @a numRows rows before the row at @a index.
	 *
	 * If @a index is rows(), a row is appended to the table.
	 *
	 * If @a index is less than 0 or greater than rows(), or if @a numRows is less than 1,
	 * this method does nothing.
	 */
	void insertRows(int index, int numRows);

	/**
	 * Removes @a numRows rows from the table, starting with the row at @a index.
	 *
	 * If the specified range falls outside the table or the number of rows is
	 * less than 1 then this method does nothing.
	 */
	void removeRows(int index, int numRows);

	/**
	 * Returns the height of @a row, or 0 if @a row does not exist.
	 */
	qreal rowHeight(int row) const;

	/**
	 * Sets the height of @a row to <code>qMax(MinimumRowHeight, height)</code>.
	 *
	 * If @a row does not exists or @a height is less than or equal to 0, this method does nothing.
	 */
	void setRowHeight(int row, qreal height);

	/**
	 * Returns the position of @a row, or 0 if @a row does not exist.
	 */
	qreal rowPosition(int row) const;

	/**
	 * Inserts @a numColumns columns before the column at @a index.
	 *
	 * If @a index is columns(), a column is appended to the table.
	 *
	 * If @a index is less than 0 or greater than columns(), or if @a numColumns is less than 1,
	 * this method does nothing.
	 */
	void insertColumns(int index, int numColumns);

	/**
	 * Removes @a numColumns columns from the table, starting with the column at @a index.
	 *
	 * If the specified range falls outside the table or the number of columns is
	 * less than 1 then this method does nothing.
	 */
	void removeColumns(int index, int numColumns);

	/**
	 * Returns the width of @a column, or 0 if @a column does not exist.
	 */
	qreal columnWidth(int column) const;

	/**
	 * Sets the width of @a column to <code>qMax(MinimumColumnWidth, width)</code>.
	 *
	 * If @a column does not exists or @a width is less than or equal to 0, this method does nothing.
	 */
	void setColumnWidth(int column, qreal width);

	/**
	 * Returns the position of @a column, or 0 if @a column does not exist.
	 */
	qreal columnPosition(int column) const;

	/**
	 * Merges the cell at the specified @a row and @a column with the adjacent cells into
	 * one cell.
	 *
	 * The new cell will span @a numRows rows and @a numCols columns. If @a numRows or
	 * @a numCols is less than the current number of rows or columns the cell spans then
	 * this method does nothing.
	 *
	 * If any of the merged cells are already merged with other cells, the two areas of
	 * merged cells will be united.
	 */
	void mergeCells(int row, int column, int numRows, int numCols);

	/**
	 * Splits the specified cell at @a row and @a column into an array of multiple cells with
	 * dimensions specified by @a numRows and @a numCols.
	 */
	void splitCell(int row, int column, int numRows, int numCols);

	/**
	 * Returns the cell at @a row, @a column.
	 *
	 * If the cell is covered by a spanning cell, the spanning cell is returned. If the cell is
	 * not in this table, an invalid cell is returned. The table may later mark cells returned
	 * by this function as invalid, if the row or column containing the cell is removed.
	 */
	TableCell cellAt(int row, int column) const;

	/// Resizes the table to fit the frame, using a Proportional resize strategy.
	void adjustTableToFrame();

	/// Resizes the frame to fit the table.
	void adjustFrameToTable();

	/// Sets the fill color of this table to @a color.
	void setFillColor(const QString& color);

	/// Unsets the fill color of this table.
	void unsetFillColor();

	/// Returns the fill color of this table.
	QString fillColor() const;

	/// Sets the left border of this table to @a border.
	void setLeftBorder(const TableBorder& border);

	/// Unsets the left border of this table.
	void unsetLeftBorder();

	/// Returns the left border of this table.
	TableBorder leftBorder() const;

	/// Sets the right border of this table to @a border.
	void setRightBorder(const TableBorder& border);

	/// Unsets the right border of this table.
	void unsetRightBorder();

	/// Returns the right border of this table.
	TableBorder rightBorder() const;

	/// Sets the top border of this table to @a border.
	void setTopBorder(const TableBorder& border);

	/// Unsets the top border of this table.
	void unsetTopBorder();

	/// Returns the top border of this table.
	TableBorder topBorder() const;

	/// Sets the bottom border of this table to @a border.
	void setBottomBorder(const TableBorder& border);

	/// Unsets the bottom border of this table.
	void unsetBottomBorder();

	/// Returns the bottom border of this table.
	TableBorder bottomBorder() const;

	/// Sets the table style of this table to @a style.
	void setStyle(const QString& style);

	/// Unsets the style of this table.
	void unsetStyle();

	/// Returns the style of this table.
	QString style() const;

	/// Returns the width of the widest border along the left side of this table.
	qreal maxLeftBorderWidth() const;

	/// Returns the width of the widest border along the right side of this table.
	qreal maxRightBorderWidth() const;

	/// Returns the width of the widest border along the top side of this table.
	qreal maxTopBorderWidth() const;

	/// Returns the width of the widest border along the bottom side of this table.
	qreal maxBottomBorderWidth() const;

	/// Returns <code>true</code> if the table is overflowing its frame.
	bool isOverflowing() const { return effectiveWidth() > width() || effectiveHeight() > height(); }

	/// Returns this item as a PageItem_Table.
	virtual PageItem_Table* asTable() { return this; }

	/// Returns <code>true</code>.
	virtual bool isTable() const { return true; }

	/// Returns PageItem::Table.
	virtual ItemType realItemType() const { return PageItem::Table; }

	/// Adds the applicable actions for this table to @a actionList.
	virtual void applicableActions(QStringList& actionList);

	/// Returns a textual description of this item.
	virtual QString infoDescription() { return QString(); }

signals:
	/// This signal is emitted whenever something in the table changes.
	void changed();

protected:
	/// Paints this item.
	virtual void DrawObj_Item(ScPainter *p, QRectF clipRect);

private:
	/// Enum describing types of changes on a table. For internal use.
	enum ChangeType
	{
		RowsInserted,    /**< Rows were inserted. */
		RowsRemoved,     /**< Rows were removed. */
		ColumnsInserted, /**< Columns were inserted. */
		ColumnsRemoved   /**< Columns were removed. */
	};

	/// Returns true if @a row is a row in this table.
	bool validRow(int row) const { return row >= 0 && row < m_rows; }
	/// Returns true if @a column is a column in this table.
	bool validColumn(int column) const { return column >= 0 && column < m_columns; }
	/// Returns true if there is a cell at @a row, @a column in this table.
	bool validCell(int row, int column) const { return validRow(row) && validColumn(column); }

	/// Resizes the table to @a width, @a height, affecting rows and columns equally.
	void resizeEqual(qreal width, qreal height);

	/// Resizes the table to @a width, @a height, affecting rows and columns proportionally.
	void resizeProportional(qreal width, qreal height);

	/**
	 * Updates row and column spans following a change in rows or columns.
	 *
	 * If @a changeType is <code>RowsInserted</code> or <code>ColumnsInserted</code>, @a index
	 * and @a number specifies that @a number rows or columns were inserted before the row or
	 * or column at @a index. Similarly, if @a changeType is <code>RowsRemoved</code> or
	 * <code>ColumnsRemoved</code>, @a index and @a number specifies that @a number rows or
	 * columns were removed, starting with the row or column at @a index.
	 */
	void updateSpans(int index, int number, ChangeType changeType);

	/// Prints internal table information. For internal use.
	void debug() const;

	/// Table sanity check. Very slow. For internal use.
	void assertValid() const;

private:
	/// List of rows of cells in the table.
	QList<QList<TableCell> > m_cellRows;

	/// Number of rows.
	int m_rows;
	/// Number of columns.
	int m_columns;

	/// Vertical positions of rows.
	QList<qreal> m_rowPositions;
	/// Height of rows.
	QList<qreal> m_rowHeights;

	/// Horizontal positions of columns.
	QList<qreal> m_columnPositions;
	/// Width of columns.
	QList<qreal> m_columnWidths;

	/// List of areas of merged cells.
	QList<CellArea> m_cellAreas;

	/// Style of the table.
	TableStyle m_style;

	/// The table painter to paint the table with.
	TablePainter* m_tablePainter;
};

#endif // PAGEITEM_TABLE_H
