/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef PAGEITEM_TABLE_H
#define PAGEITEM_TABLE_H

#include <QString>
#include <QList>
#include <QRectF>

#include "scribusapi.h"
#include "pageitem.h"
#include "styles/tablestyle.h"
#include "tablecell.h"
#include "cellarea.h"

class ScPainter;
class ScribusDoc;

/**
 * The TableBorder class represents a border in a table.
 * <p>
 * It is used temporarily during table painting.
 */
class TableBorder
{
public:
	TableBorder() {}
	TableBorder(qreal width, const QString& color) : width(width), color(color) {}

	qreal width; // TODO: Eventually use multiLine.
	QString color;
	QPointF start;
	QPointF end;
};

inline bool operator==(const TableBorder& lhs, const TableBorder& rhs)
{
	return lhs.width == rhs.width && rhs.color == rhs.color;
};

/**
 * The PageItem_Table class represents a table.
 * <p>
 * A table is a group of cells ordered into rows and columns. Each table contains at least
 * one row and one column.
 * <p>
 * A table's size can be changed by using <code>insertRows()</code>,
 * <code>insertColumns()</code>, <code>removeRows()</code>, or <code>removeColumns()</code>.
 * Use <code>cellAt()</code> to retrieve table cells.
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

public:
	/// Construct a new table item with @a numRows rows and @a numColumns columns.
	PageItem_Table(ScribusDoc *pa, double x, double y, double w, double h, double w2, QString fill, QString outline, int numRows = 1, int numColumns = 1);

	/// Destructor.
	~PageItem_Table() {};

	/// Returns the number of rows in the table.
	int rows() const { return m_rows; }

	/// Returns the number of columns in the table.
	int columns() const { return m_columns; }

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
	 * Sets the height of @a row to @a height.
	 *
	 * If @a row does not exists or @a height is less than or equal to 0, this method does nothing.
	 */
	void setRowHeight(int row, qreal height);

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
	 * Sets the width of @a column to @a width.
	 *
	 * If @a column does not exists or @a width is less than or equal to 0, this method does nothing.
	 */
	void setColumnWidth(int column, qreal width);

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

	/**
	 * Returns <code>true</code> if the cell at @a row, @a column is covered by a span.
	 */
	bool isCovered(int row, int column) const;

	/**
	 * Adjusts the rows and columns of the table to fit the frame. Currently this method gives
	 * all rows equal height and all columns equal width.
	 *
	 * TODO: We need much more sophistication/flexibility here, but that will come.
	 */
	void adjustToFrame();

	// Formatting properties setters/getters.

	/// Sets the background color of this table to @a color.
	void setBackgroundColor(const QString& color) { m_style.setBackgroundColor(color); }

	/// Returns the background color of this table.
	QString backgroundColor() const { return m_style.backgroundColor(); }

	/// Sets the width of the left border of this table to @a width.
	void setLeftBorderWidth(qreal width) { m_style.setLeftBorderWidth(width); }

	/// Returns the width of the left border of this table.
	qreal leftBorderWidth() const { return m_style.leftBorderWidth(); }

	/// Sets the width of the right border of this table to @a width.
	void setRightBorderWidth(qreal width) { m_style.setRightBorderWidth(width); }

	/// Returns the width of the right border of this table.
	qreal rightBorderWidth() const { return m_style.rightBorderWidth(); }

	/// Sets the width of the top border of this table to @a width.
	void setTopBorderWidth(qreal width) { m_style.setTopBorderWidth(width); }

	/// Returns the width of the top border of this table.
	qreal topBorderWidth() const { return m_style.topBorderWidth(); }

	/// Sets the width of the bottom border of this table to @a width.
	void setBottomBorderWidth(qreal width) { m_style.setBottomBorderWidth(width); }

	/// Returns the width of the bottom border of this table.
	qreal bottomBorderWidth() const { return m_style.bottomBorderWidth(); }

	/// Sets the color of the left border of this table to @a color.
	void setLeftBorderColor(const QString& color) { m_style.setLeftBorderColor(color); }

	/// Returns the color of the left border of this table.
	QString leftBorderColor() const { return m_style.leftBorderColor(); }

	/// Sets the color of the right border of this table to @a color.
	void setRightBorderColor(const QString& color) { m_style.setRightBorderColor(color); }

	/// Returns the color of the right border of this table.
	QString rightBorderColor() const { return m_style.rightBorderColor(); }

	/// Sets the color of the top border of this table to @a color.
	void setTopBorderColor(const QString& color) { m_style.setTopBorderColor(color); }

	/// Returns the color of the top border of this table.
	QString topBorderColor() const { return m_style.topBorderColor(); }

	/// Sets the color of the bottom border of this table to @a color.
	void setBottomBorderColor(const QString& color) { m_style.setBottomBorderColor(color); }

	/// Returns the color of the bottom border of this table.
	QString bottomBorderColor() const { return m_style.bottomBorderColor(); }

	/// Sets the border model of this table to @a model.
	void setBorderModel(TableStyle::BorderModel model) { m_style.setBorderModel(model); }

	/// Returns the border model of this table.
	TableStyle::BorderModel borderModel() const { return m_style.borderModel(); }

	/// Sets the border drawing options of this table to @a options.
	void setBorderModel(TableStyle::BorderDrawingOptions options) { m_style.setBorderDrawingOptions(options); }

	/// Returns the border drawing options of this table.
	TableStyle::BorderDrawingOptions borderDrawingOptions() const { return m_style.borderDrawingOptions(); }

	/// Sets the table style for this table to @a style.
	void setStyle(const QString& style) { m_style.setParent(style); }

	/// Returns the named table style for this table.
	QString style() const { return m_style.parent(); }

	// Reimplemented from PageItem.
	virtual PageItem_Table* asTable() { return this; }
	virtual bool isTable() const { return true; }
	PageItem::ItemType realItemType() const { return PageItem::Table; }
	virtual void applicableActions(QStringList &actionList) {};
	virtual QString infoDescription() { return QString(); }

protected:
	// Reimplemented from PageItem.
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

	/**
	 * Returns the rectangle of the cell in the given @a row and @a column.
	 *
	 * If the cell is part of a merged area, the rectangle of the entire area is returned.
	 * If the cell does not exist, a null rectangle is returned.
	 */
	QRectF cellRect(int row, int column) const;

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

	/// Returns the collapsed left border of @a cell.
	TableBorder collapsedLeftBorder(const TableCell& cell) const;
	/// Returns the collapsed right border of @a cell.
	TableBorder collapsedRightBorder(const TableCell& cell) const;
	/// Returns the collapsed top border of @a cell.
	TableBorder collapsedTopBorder(const TableCell& cell) const;
	/// Returns the collapsed bottom border of @a cell.
	TableBorder collapsedBottomBorder(const TableCell& cell) const;

	/**
	 * Returns @a firstBorder collapsed with @a secondBorder.
	 *
	 * If only one of the borders has a color, the border with a color is returned.
	 * If none of the borders has a color, a border with no color and width 0.0 is returned.
	 * If both borders has a color, the wider of the two is returned.
	 * If both borders has a color and are equally wide, @a firstBorder is returned.
	 */
	TableBorder collapseBorders(const TableBorder& firstBorder, const TableBorder& secondBorder) const;

	/// Draws the background of the table.
	void drawTableBackground(ScPainter* p);

	/// Draws the background of @a cell.
	void drawCellBackground(const TableCell& cell, ScPainter* p);

	/// Draws the borders in @a borders.
	void drawBorders(const QList<TableBorder>& borders, ScPainter* p);

	/// Draws the entire table using the collapsed border model.
	void drawTableCollapsed(ScPainter* p);

	/// Prints internal table information. For internal use.
	void debug() const;

	/// Table sanity check. Very slow. For internal use.
	void assertValid() const;

private:
	friend class TableCell;

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
};

#endif // PAGEITEM_TABLE_H
