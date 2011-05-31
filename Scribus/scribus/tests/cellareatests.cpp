/*
 * Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>
 *
 * For general Scribus (>=1.3.2) copyright and licensing information please refer
 * to the COPYING file provided with the program. Following this notice may exist
 * a copyright and/or license notice that predates the release of Scribus 1.3.2
 * for which a new license (GPL+exception) is in place.
 */
#include <QtTest/QtTest>

#include "cellareatests.h"
#include "cellarea.h"

Q_DECLARE_METATYPE(CellArea);

void CellAreaTests::testConstructionAndSetters()
{
	// Default constructor should give invalid area.
	CellArea invalidArea1;
	QVERIFY(!invalidArea1.isValid());

	// Negative width.
	CellArea invalidArea2(1, 1, -3, 3);
	QVERIFY(!invalidArea2.isValid());

	// Negative height.
	CellArea invalidArea3(1, 1, 3, -3);
	QVERIFY(!invalidArea3.isValid());

	// Both width and height negative.
	CellArea invalidArea4(1, 1, -3, -3);
	QVERIFY(!invalidArea4.isValid());

	// Zero width.
	CellArea invalidArea5(1, 1, 0, 3);
	QVERIFY(!invalidArea5.isValid());

	// Zero height.
	CellArea invalidArea6(1, 1, 3, 0);
	QVERIFY(!invalidArea6.isValid());

	// Both width and height zero.
	CellArea invalidArea7(1, 1, 0, 0);
	QVERIFY(!invalidArea7.isValid());

	// Valid cell area.
	CellArea validArea(5, 5, 5, 5);
	QVERIFY(validArea.isValid());

	// Test setters/getters.
	validArea.setRow(3);
	QCOMPARE(validArea.row(), 3);
	QCOMPARE(validArea.bottom(), 7);
	validArea.setColumn(3);
	QCOMPARE(validArea.column(), 3);
	QCOMPARE(validArea.right(), 7);
	validArea.setWidth(3);
	QCOMPARE(validArea.width(), 3);
	QCOMPARE(validArea.right(), 5);
	validArea.setHeight(3);
	QCOMPARE(validArea.height(), 3);
	QCOMPARE(validArea.bottom(), 5);
}

void CellAreaTests::testComparison()
{
	// Identity test.
	CellArea area1(1, 3, 5, 7);
	QVERIFY(area1 == area1);

	// Two equal areas.
	CellArea area2(1, 3, 5, 7);
	QVERIFY(area1 == area2);

	// Row differs.
	CellArea area3(2, 3, 5, 7);
	QVERIFY(!(area1 == area3));
	QVERIFY(area1 != area3);

	// Column differs.
	CellArea area4(1, 4, 5, 7);
	QVERIFY(!(area1 == area4));
	QVERIFY(area1 != area4);

	// Width differs.
	CellArea area5(1, 3, 6, 7);
	QVERIFY(!(area1 == area5));
	QVERIFY(area1 != area5);

	// Height differs.
	CellArea area6(1, 3, 5, 8);
	QVERIFY(!(area1 == area6));
	QVERIFY(area1 != area6);
}

void CellAreaTests::testContainsPoint()
{
	QFETCH(int, row);
	QFETCH(int, column);
	QFETCH(bool, result);

	CellArea area(1, 1, 2, 2);

	QCOMPARE(area.contains(row, column), result);
}

void CellAreaTests::testContainsPoint_data()
{
	QTest::addColumn<int>("row");
	QTest::addColumn<int>("column");
	QTest::addColumn<bool>("result");

	// Tests for area.contains(row, column) where area is (1, 1, 2, 2).

	// Points outside the area.
	QTest::newRow("outside1") << 0 << 0 << false;
	QTest::newRow("outside2") << 0 << 1 << false;
	QTest::newRow("outside3") << 0 << 2 << false;
	QTest::newRow("outside4") << 0 << 3 << false;
	QTest::newRow("outside5") << 1 << 3 << false;
	QTest::newRow("outside6") << 2 << 3 << false;
	QTest::newRow("outside7") << 3 << 3 << false;
	QTest::newRow("outside8") << 3 << 2 << false;
	QTest::newRow("outside9") << 3 << 1 << false;
	QTest::newRow("outside10") << 3 << 0 << false;
	QTest::newRow("outside11") << 2 << 0 << false;
	QTest::newRow("outside12") << 1 << 0 << false;

	// Points inside the area.
	QTest::newRow("inside1") << 1 << 1 << true;
	QTest::newRow("inside2") << 1 << 2 << true;
	QTest::newRow("inside3") << 2 << 1 << true;
	QTest::newRow("inside4") << 2 << 2 << true;
}

void CellAreaTests::testContainsArea()
{
	QFETCH(CellArea, area1);
	QFETCH(CellArea, area2);
	QFETCH(bool, result);

	QCOMPARE(area2.contains(area1), result);
}

void CellAreaTests::testContainsArea_data()
{
	QTest::addColumn<CellArea>("area1");
	QTest::addColumn<CellArea>("area2");
	QTest::addColumn<bool>("result");

	// Tests for area2.contains(area1).
	QTest::newRow("area1 left of area2") << CellArea(1, 3, 2, 2) << CellArea(1, 1, 2, 2) << false;
	QTest::newRow("area1 above area2") << CellArea(1, 1, 2, 2) << CellArea(3, 1, 2, 2) << false;
	QTest::newRow("area1 intersects top right of area2") << CellArea(0, 2, 2, 2) << CellArea(1, 1, 2, 2) << false;
	QTest::newRow("area1 intersects top left of area2") << CellArea(0, 0, 2, 2) << CellArea(1, 1, 2, 2) << false;
	QTest::newRow("area1 same as area2") << CellArea(1, 1, 2, 2) << CellArea(1, 1, 2, 2) << true;
	QTest::newRow("area1 inside area2") << CellArea(1, 1, 3, 3) << CellArea(0, 0, 6, 6) << true;
	QTest::newRow("area1 inside area2") << CellArea(1, 1, 2, 2) << CellArea(1, 1, 4, 4) << true;
	QTest::newRow("area1 inside area2") << CellArea(2, 2, 3, 3) << CellArea(1, 1, 4, 4) << true;
	QTest::newRow("area2 inside area1") << CellArea(0, 0, 6, 6) << CellArea(1, 1, 3, 3) << false;
	QTest::newRow("area2 inside area1") << CellArea(1, 1, 4, 4) << CellArea(1, 1, 2, 2) << false;
	QTest::newRow("area2 inside area1") << CellArea(1, 1, 4, 4) << CellArea(2, 2, 3, 3) << false;
}

void CellAreaTests::testIntersects()
{
	QFETCH(CellArea, area1);
	QFETCH(CellArea, area2);
	QFETCH(bool, result);

	QCOMPARE(area1.intersects(area2), result);
	QCOMPARE(area2.intersects(area1), result);
}

void CellAreaTests::testIntersects_data()
{
	QTest::addColumn<CellArea>("area1");
	QTest::addColumn<CellArea>("area2");
	QTest::addColumn<bool>("result");

	// Tests for area1.intersects(area2) and area2.intersects(area1).
	QTest::newRow("area1 left of area2") << CellArea(1, 3, 2, 2) << CellArea(1, 1, 2, 2) << false;
	QTest::newRow("area1 above area2") << CellArea(1, 1, 2, 2) << CellArea(3, 1, 2, 2) << false;
	QTest::newRow("area1 same as area2") << CellArea(1, 1, 2, 2) << CellArea(1, 1, 2, 2) << true;
	QTest::newRow("area1 intersects top right of area2") << CellArea(0, 2, 2, 2) << CellArea(1, 1, 2, 2) << true;
	QTest::newRow("area1 intersects top left of area2") << CellArea(0, 0, 2, 2) << CellArea(1, 1, 2, 2) << true;
	QTest::newRow("area1 inside area2") << CellArea(1, 1, 3, 3) << CellArea(0, 0, 6, 6) << true;
}

void CellAreaTests::testTranslated()
{
	QFETCH(CellArea, area);
	QFETCH(int, columns);
	QFETCH(int, rows);
	QFETCH(CellArea, result);

	QCOMPARE(area.translated(rows, columns), result);
}

void CellAreaTests::testTranslated_data()
{
	QTest::addColumn<CellArea>("area");
	QTest::addColumn<int>("columns");
	QTest::addColumn<int>("rows");
	QTest::addColumn<CellArea>("result");

	// Tests for area.translated(rows, columns).
	QTest::newRow("no translation") << CellArea(1, 1, 2, 2) << 0 << 0 << CellArea(1, 1, 2, 2);
	QTest::newRow("translate right 1") << CellArea(1, 1, 2, 2) << 1 << 0 << CellArea(1, 2, 2, 2);
	QTest::newRow("translate left 1") << CellArea(1, 1, 2, 2) << -1 << 0 << CellArea(1, 0, 2, 2);
	QTest::newRow("translate down 1") << CellArea(1, 1, 2, 2) << 0 << 1 << CellArea(2, 1, 2, 2);
	QTest::newRow("translate up 1") << CellArea(1, 1, 2, 2) << 0 << -1 << CellArea(0, 1, 2, 2);
}

void CellAreaTests::testTranslate()
{
	QFETCH(CellArea, area);
	QFETCH(int, columns);
	QFETCH(int, rows);
	QFETCH(CellArea, result);

	area.translate(rows, columns);

	QCOMPARE(area, result);
}

void CellAreaTests::testTranslate_data()
{
	QTest::addColumn<CellArea>("area");
	QTest::addColumn<int>("columns");
	QTest::addColumn<int>("rows");
	QTest::addColumn<CellArea>("result");

	// Tests for area.translate(rows, columns).
	QTest::newRow("no translation") << CellArea(1, 1, 2, 2) << 0 << 0 << CellArea(1, 1, 2, 2);
	QTest::newRow("translate right 1") << CellArea(1, 1, 2, 2) << 1 << 0 << CellArea(1, 2, 2, 2);
	QTest::newRow("translate left 1") << CellArea(1, 1, 2, 2) << -1 << 0 << CellArea(1, 0, 2, 2);
	QTest::newRow("translate down 1") << CellArea(1, 1, 2, 2) << 0 << 1 << CellArea(2, 1, 2, 2);
	QTest::newRow("translate up 1") << CellArea(1, 1, 2, 2) << 0 << -1 << CellArea(0, 1, 2, 2);
}

QTEST_APPLESS_MAIN(CellAreaTests)
