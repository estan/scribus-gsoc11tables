/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
#ifndef TABLEBORDER_H
#define TABLEBORDER_H

#include <QPointF>
#include <QString>
#include <QStringList>
#include <QList>

#include "commonstrings.h"

/**
 * The TableBorderLine class represents a single line in a table border.
 */
class TableBorderLine
{
public:
	/// Creates a new solid table border line with 0 width and no color.
	TableBorderLine()
	{
		setWidth(0.0);
		setStyle(Qt::SolidLine);
		setColor(CommonStrings::None);
	}

	/// Creates a new table border line with @a width, @a style and @a color.
	TableBorderLine(qreal width, Qt::PenStyle style, const QString& color)
	{
		setWidth(width);
		setStyle(style);
		setColor(color);
	}

	/// Returns the width of this table border line.
	qreal width() const { return m_width; }

	/// Sets the width of this table border line to @a width.
	void setWidth(qreal width) { m_width = width; }

	/// Returns the pen style of this table border line.
	Qt::PenStyle style() const { return m_style; }

	/// Sets the pen style of this table border line to @a style.
	void setStyle(Qt::PenStyle style) { m_style = style; }

	/// Returns the color of this table border line.
	QString color() const { return m_color; }

	/// Sets the color of this table border line to @a color.
	void setColor(const QString& color) { m_color = color; }

	/// Returns a string representation of the border line.
	QString asString() const { return QString("(%1,%2,%3)").arg(width()).arg(style()).arg(color()); }

	/// Returns <code>true</code> if this table border line is equal to @a other.
	bool operator==(const TableBorderLine& other) const
	{
		return width() == other.width() && color() == other.color() && style() == other.style();
	}

private:
	/// The width of the table border line.
	qreal m_width;
	/// The pen style of the table border line.
	Qt::PenStyle m_style;
	/// The color of the table border line.
	QString m_color;
};

/**
 * The TableBorder class represents a border on a table or table cell.
 *
 * It consists of a list of table border lines.
 */
class TableBorder
{
public:
	/// Creates a new null table border. Null borders are used to indicate the absence of a border.
	TableBorder() {}

	/// Creates a new table border with a single border line with @a width, @a style and @a color.
	TableBorder(qreal width, Qt::PenStyle style, const QString& color) { appendLine(TableBorderLine(width, style, color)); }

	/// Returns the width of the first table border line, or 0.0 if this is a null border.
	qreal width() const { return isNull() ? 0.0 : m_borderLines.first().width(); }

	/// Returns the list of border lines for this border in the order they should be painted.
	QList<TableBorderLine> borderLines() const { return m_borderLines; }

	/// Appends @a line to the list of border lines for this border.
	void appendLine(const TableBorderLine& borderLine) { m_borderLines.append(borderLine); }

	/**
	 * Returns <code>true</code> if this border can be joined with @a other.
	 * Two borders can be joined if they are both not null and their list of border lines are equal.
	 */
	bool joinsWith(const TableBorder& other) const { return !isNull() && !other.isNull() && borderLines() == other.borderLines(); }

	/// Returns <code>true</code> if this border has no border lines.
	bool isNull() const { return m_borderLines.size() == 0; }

	/// Returns a string representation of the border.
	QString asString() const {
		QStringList lines;
		foreach (TableBorderLine line, m_borderLines)
			lines << line.asString();
		return QString("TableBorder(%1)").arg(lines.join(","));
	}
private:
	/// List of border lines in the order they should be painted.
	QList<TableBorderLine> m_borderLines;
};

inline bool operator==(const TableBorder& lhs, const TableBorder& rhs)
{
	return lhs.borderLines() == rhs.borderLines();
}

#endif // TABLEBORDER_H
