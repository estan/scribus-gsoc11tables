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
#include <QHash>
#include <QVariant>
#include <QRectF>

#include "scribusapi.h"
#include "pageitem.h"
#include "tablecell.h"
#include "cellarea.h"

class ScPainter;
class ScribusDoc;
class FRect;

/**
 * The PageItem_Table class represents a table item.
 */
class SCRIBUS_API PageItem_Table : public PageItem
{
	Q_OBJECT

public:
	/// Table formatting properties.
	enum Property
	{
		BackgroundColor,
		LeftBorderWidth,
		RightBorderWidth,
		TopBorderWidth,
		BottomBorderWidth,
		LeftBorderColor,
		RightBorderColor,
		TopBorderColor,
		BottomBorderColor
	};

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
	 * not in this table, an invalid cell is returned.
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

	/// Sets the width of the left border of this table to @a width.
	void setLeftBorderWidth(qreal width) { setProperty(LeftBorderWidth, QVariant(width)); }

	/// Returns the width of the left border of this table.
	qreal leftBorderWidth() const { return property(LeftBorderWidth).toReal(); }

	/// Sets the width of the right border of this table to @a width.
	void setRightBorderWidth(qreal width) { setProperty(RightBorderWidth, QVariant(width)); }

	/// Returns the width of the right border of this table.
	qreal rightBorderWidth() const { return property(RightBorderWidth).toReal(); }

	/// Sets the width of the top border of this table to @a width.
	void setTopBorderWidth(qreal width) { setProperty(TopBorderWidth, QVariant(width)); }

	/// Returns the width of the top border of this table.
	qreal topBorderWidth() const { return property(TopBorderWidth).toReal(); }

	/// Sets the width of the bottom border of this table to @a width.
	void setBottomBorderWidth(qreal width) { setProperty(BottomBorderWidth, QVariant(width)); }

	/// Returns the width of the bottom border of this table.
	qreal bottomBorderWidth() const { return property(BottomBorderWidth).toReal(); }

	/// Sets the color of the left border of this table to @a color.
	void setLeftBorderColor(const QString& color) { setProperty(LeftBorderColor, QVariant(color)); }

	/// Returns the color of the left border of this table.
	QString leftBorderColor() const { return property(LeftBorderColor).toString(); }

	/// Sets the color of the right border of this table to @a color.
	void setRightBorderColor(const QString& color) { setProperty(RightBorderColor, QVariant(color)); }

	/// Returns the color of the right border of this table.
	QString rightBorderColor() const { return property(RightBorderColor).toString(); }

	/// Sets the color of the top border of this table to @a color.
	void setTopBorderColor(const QString& color) { setProperty(TopBorderColor, QVariant(color)); }

	/// Returns the color of the top border of this table.
	QString topBorderColor() const { return property(TopBorderColor).toString(); }

	/// Sets the color of the bottom border of this table to @a color.
	void setBottomBorderColor(const QString& color) { setProperty(BottomBorderColor, QVariant(color)); }

	/// Returns the color of the bottom border of this table.
	QString bottomBorderColor() const { return property(BottomBorderColor).toString(); }

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

	/// Sets the property with key @a key to @a value.
	void setProperty(Property key, const QVariant& value) { m_properties.insert(key, value); }
	/// Returns the property with key @a key.
	const QVariant property(Property key) const { return m_properties.value(key); }
	/// Returns <code>true</code> if the cell has the property with key @a key set.
	bool hasProperty(Property key) const { return m_properties.contains(key); }
	/// Clears the property with key @a key.
	void clearProperty(Property key) { m_properties.remove(key); }
	/// Clears all set properties of the cell.
	void clearProperties() { m_properties.clear(); }

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
	FRect cellRect(int row, int column) const;

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

	/// Inserts @a numRows new row of cells at @a index.
	void insertCellRows(int index, int numRows);
	/// Removes @a numRows row of cells starting with the row at @a index.
	void removeCellRows(int index, int numRows);
	/// Inserts @a numColumns new columns of cells at @a index.
	void insertCellColumns(int index, int numColumns);
	/// Removes @a numColumns columns of cells starting with the column at @a index.
	void removeCellColumns(int index, int numColumns);

	/// Draws a decorative dotted grid line from @a start to @a end.
	void drawGridLine(const FPoint& start, const FPoint& end, ScPainter *p) const;

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

	/// Table formatting properties.
	QHash<int, QVariant> m_properties;
};

#endif // PAGEITEM_TABLE_H
