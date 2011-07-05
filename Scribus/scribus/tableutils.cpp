/*
Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/

#include <QPointF>

#include "tableborder.h"
#include "tableutils.h"

namespace TableUtils
{

TableBorder collapseBorders(const TableBorder& firstBorder, const TableBorder& secondBorder)
{
	TableBorder collapsedBorder;

	if (firstBorder.isNull() && secondBorder.isNull())
	{
		// Both borders are null, so return a null border.
		return collapsedBorder;
	}
	else if (firstBorder.isNull())
	{
		// First border is null, so return second border.
		collapsedBorder = secondBorder;
	}
	else if (secondBorder.isNull())
	{
		// Second border is null, so return first border.
		collapsedBorder = firstBorder;
	}
	else
	{
		if (firstBorder.width() > secondBorder.width())
		{
			// First border is wider than second border, so return first border.
			collapsedBorder = firstBorder; // (4)
		}
		else if (firstBorder.width() < secondBorder.width())
		{
			// Second border is wider than first border, so return second border.
			collapsedBorder = secondBorder; // (5)
		}
		else
		{
			if (firstBorder.borderLines().size() > secondBorder.borderLines().size())
			{
				// First border has more border lines than second border, so return first border.
				collapsedBorder = firstBorder;
			}
			else
			{
				// Second border has more or equal border lines than first border, so return second border.
				collapsedBorder = secondBorder;
			}
		}
	}

	return collapsedBorder;
}

void joinVertical(const TableBorder& border, const TableBorder& topLeft, const TableBorder& top,
				  const TableBorder& topRight, const TableBorder& bottomLeft, const TableBorder& bottom,
				  const TableBorder& bottomRight, QPointF* start, QPointF* end, QPointF* startOffsetFactors,
				  QPointF* endOffsetFactors)
{
	Q_ASSERT(start);
	Q_ASSERT(end);
	Q_ASSERT(startOffsetFactors);
	Q_ASSERT(endOffsetFactors);

	// Reset offset coefficients.
	startOffsetFactors->setX(0.0);
	startOffsetFactors->setY(0.0);
	endOffsetFactors->setX(0.0);
	endOffsetFactors->setY(0.0);

	/*
	 * The numbered cases in the code below refers to the 41 possible join cases illustrated
	 * in the picture at http://wiki.scribus.net/canvas/File:Table_border_join_cases.png
	 */

	/*
	 * Adjust start point(s).
	 */
	if (top.isNull() && !border.joinsWith(topLeft) && !border.joinsWith(topRight))
	{
		// Cases: 14, 18, 35, 37.
		start->setY(start->y() - 0.5 * qMax(topLeft.width(), topRight.width()));
	}
	else if (!border.joinsWith(topLeft) && !border.joinsWith(topRight) && !border.joinsWith(top))
	{
		// Cases: 1, 2, 3, 6, 12, 24, 26, 29, 31.
		start->setY(start->y() + 0.5 * qMax(topLeft.width(), topRight.width()));
	}
	else if (!border.joinsWith(top) && (border.joinsWith(topLeft) || border.joinsWith(topRight)))
	{
		// Cases: 4, 5, 8, 15, 16, 17, 25, 30, 34, 36.
		startOffsetFactors->setY(-0.5);
	}
	else if (topLeft.joinsWith(topRight) && !topLeft.joinsWith(top) && !topRight.joinsWith(top))
	{
		// Cases: 11.
		start->setY(start->y() + 0.5 * qMax(topLeft.width(), topRight.width()));
	}
	// Cases: 7, 9, 10, 13, 27, 28, 32, 33 - No adjustment to start point needed.
	// Cases: 19, 20, 21, 22, 23, 38, 39, 40, 41 - N/A.

	/*
	 * Adjust end point(s).
	 */
	if (bottom.isNull() && !border.joinsWith(bottomLeft) && !border.joinsWith(bottomRight))
	{
		// Cases: 19, 23, 39, 41.
		end->setY(end->y() + 0.5 * qMax(bottomLeft.width(), bottomRight.width()));
	}
	else if (!border.joinsWith(bottomLeft) && !border.joinsWith(bottomRight) && !border.joinsWith(bottom))
	{
		// Cases: 1, 4, 5, 8, 12, 24, 25, 29, 30.
		end->setY(end->y() - 0.5 * qMax(bottomLeft.width(), bottomRight.width()));
	}
	else if (!border.joinsWith(bottom) && (border.joinsWith(bottomLeft) || border.joinsWith(bottomRight)))
	{
		// Cases: 2, 3, 6, 20, 21, 22, 26, 31, 38, 40.
		endOffsetFactors->setY(0.5);
	}
	else if (bottomLeft.joinsWith(bottomRight) && !bottomLeft.joinsWith(bottom) && !bottomRight.joinsWith(bottom))
	{
		// Cases: 11.
		end->setY(end->y() - 0.5 * qMax(bottomLeft.width(), bottomRight.width()));
	}
	// Cases: 7, 9, 10, 13, 27, 28, 32, 33 - No adjustment to end point needed.
	// Cases: 14, 15, 16, 17, 18, 34, 35, 36, 37 - N/A.
}

void joinHorizontal(const TableBorder& border, const TableBorder& topLeft, const TableBorder& left,
				  const TableBorder& bottomLeft, const TableBorder& topRight, const TableBorder& right,
				  const TableBorder& bottomRight, QPointF* start, QPointF* end, QPointF* startOffsetFactors,
				  QPointF* endOffsetFactors)
{
	Q_ASSERT(start);
	Q_ASSERT(end);
	Q_ASSERT(startOffsetFactors);
	Q_ASSERT(endOffsetFactors);

	// Reset offset coefficients.
	startOffsetFactors->setX(0.0);
	startOffsetFactors->setY(0.0);
	endOffsetFactors->setX(0.0);
	endOffsetFactors->setY(0.0);

	/*
	 * The numbered cases in the code below refers to the 41 possible join cases illustrated
	 * in the picture at http://wiki.scribus.net/canvas/File:Table_border_join_cases.png
	 */

	/*
	 * Adjust start point.
	 */
	if (left.isNull() && (topLeft.isNull() || bottomLeft.isNull()))
	{
		if (border.joinsWith(topLeft) || border.joinsWith(bottomLeft))
		{
			// Cases: 34, 38.
			startOffsetFactors->setX(0.5);
		}
		else
		{
			// Cases: 35, 39.
			start->setX(start->x() + 0.5 * qMax(topLeft.width(), bottomLeft.width()));
		}
	}
	else if (left.isNull() && !topLeft.isNull() && !bottomLeft.isNull() &&
			!border.joinsWith(topLeft) && !border.joinsWith(bottomLeft))
	{
		if (topLeft.joinsWith(bottomLeft))
		{
			// Cases: 33.
			start->setX(start->x() + 0.5 * qMax(topLeft.width(), bottomLeft.width()));
		}
		else
		{
			// Cases: 29.
			start->setX(start->x() - 0.5 * qMax(topLeft.width(), bottomLeft.width()));
		}
	}
	else if ((!border.joinsWith(topLeft) && !border.joinsWith(bottomLeft) && !border.joinsWith(left)) &&
			(topLeft.joinsWith(left) || bottomLeft.joinsWith(left) || topLeft.joinsWith(bottomLeft)))
	{
		// Cases: 2, 5, 9, 13, 15, 20.
		start->setX(start->x() + 0.5 * qMax(topLeft.width(), bottomLeft.width()));
	}
	else if (border.joinsWith(topLeft) || border.joinsWith(bottomLeft))
	{
		// Cases: 3, 4, 6, 7, 8, 10, 16, 17, 21, 22, 30, 31, 32.
		startOffsetFactors->setX(0.5);
	}
	// Cases: 1, 11, 12, 14, 18, 19, 23 - No adjustment to start point needed.
	// Cases: 24, 25, 26, 27, 28, 36, 37, 40, 41 - N/A.

	/*
	 * Adjust end point.
	 */
	if (right.isNull() && (topRight.isNull() || bottomRight.isNull()))
	{
		if (border.joinsWith(topRight) || border.joinsWith(bottomRight))
		{
			// Cases: 36, 40.
			endOffsetFactors->setX(-0.5);
		}
		else
		{
			// Cases: 37, 41.
			end->setX(end->x() - 0.5 * qMax(topRight.width(), bottomRight.width()));
		}
	}
	else if (right.isNull() && !topRight.isNull() && !bottomRight.isNull() &&
			!border.joinsWith(topRight) && !border.joinsWith(bottomRight))
	{
		if (topRight.joinsWith(bottomRight))
		{
			// Cases: 28.
			end->setX(end->x() - 0.5 * qMax(topRight.width(), bottomRight.width()));
		}
		else
		{
			// Cases: 24.
			end->setX(end->x() - 0.5 * qMax(topRight.width(), bottomRight.width()));
		}
	}
	else if ((!border.joinsWith(topRight) && !border.joinsWith(bottomRight) && !border.joinsWith(right)) &&
			 (topRight.joinsWith(right) || bottomRight.joinsWith(right) || topRight.joinsWith(bottomRight)))
	{
		// Cases: 3, 4, 7, 13, 16, 21, 28.
		end->setX(end->x() - 0.5 * qMax(topRight.width(), bottomRight.width()));
	}
	else if (border.joinsWith(topRight) || border.joinsWith(bottomRight))
	{
		// Cases: 2, 5, 6, 8, 9, 10, 15, 17, 20, 22, 25, 26, 27.
		endOffsetFactors->setX(-0.5);
	}
	// Cases: 1, 11, 12, 14, 18, 19, 23 - No adjustment to end point needed.
	// Cases: 29, 30, 31, 32, 33, 34, 35, 38, 39 - N/A.
}

} // namespace TableUtils
