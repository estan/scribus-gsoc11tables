#!/usr/bin/env python

"""
Test script for tables.

Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.

Instructions for adding a new test:

  1) Write the test method.
  2) Add the method to the 'tests' list.
  3) Done.

Use check() to check a condition and fail(msg) to manually fail a test. The
tests are runned in a "fail fast" fashion; on failure, the test method will
stop executing and testing move on to the next test method.
"""

from scribus import *
from traceback import print_exc
from sys import stdout

""" Test for createTable(...) """
def test_construction():
    # Test valid construction.
    table = createTable(50, 50, 90, 90, 3, 3)
    check(objectExists(table))
    check(getTableColumns(table) == 3)
    check(getTableRows(table) == 3)
    check(getTableColumnWidth(0, table) == 30)
    check(getTableColumnWidth(1, table) == 30)
    check(getTableColumnWidth(2, table) == 30)
    check(getTableRowHeight(0, table) == 30)
    check(getTableRowHeight(1, table) == 30)
    check(getTableRowHeight(2, table) == 30)
    deleteObject(table)

    # Test invalid construction.
    bad_args = [(-1, 3), (3, -1), (0, 3), (3, 0)]
    for args in bad_args:
        try:
            table = createTable(50, 50, 90, 90, args[0], args[1]);
            fail('Expected ValueError, numRows=%i, numColumns=%i' % (args[0], args[1]))
        except ValueError:
            pass # Expected.
        finally:
            if objectExists(table):
                deleteObject(table)

""" Test for setTableRowHeight(...) """
def test_set_row_height():
    pass # TODO

""" Test for setTableColumnWidth(...) """
def test_set_column_width():
    pass # TODO

""" Test for mergeTableCells(...) """
def test_merge_cells():
    pass # TODO

""" List of test methods """
tests = [
    test_construction,
    test_set_row_height,
    test_set_column_width,
    test_merge_cells
]

#
# Test "framework" code below.
#
class TestFailure(Exception):
    """ Raised by fail() """
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return repr(self.msg)

def check(condition):
    """ Fails test if condition is false """
    if not condition:
        fail('Check failed')

def fail(msg):
    """ Fails test with msg """
    raise TestFailure(msg);

if __name__ == '__main__':
    print 'Running table tests...'
    ntests = len(tests)
    nfailed = 0
    for testnr, test in enumerate(tests):
        print '\t%i/%i: %s()%s' % (testnr + 1, ntests, test.__name__, '.' * (30 - len(test.__name__))),
        try:
            test()
        except:
            print 'Failed'
            print_exc(file=stdout)
            nfailed += 1
        else:
            print 'Passed'
    print '%i%% passed, %i tests failed out of %i' % (int(round((float(ntests - nfailed)/ntests)*100)), nfailed, ntests)
