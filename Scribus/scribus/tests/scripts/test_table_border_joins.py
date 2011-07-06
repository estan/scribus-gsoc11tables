#!/usr/bin/env python

"""
Test script for table border joining.

Copyright (C) 2011 Elvis Stansvik <elvstone@gmail.com>

For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.

The script will create each of the 43 join cases illustrated in the picture at
http://wiki.scribus.net/canvas/File:Table_border_join_cases.png.
"""

from scribus import *


# Fill the page with 43 tables.
page_size = getPageSize()                                                               
table_width = (page_size[0] / 6) - 10                                                   
table_height = (page_size[1] / 8) - 10
for i in range(43):
    col = i % 6
    row = i / 6
    x = 5 + ((table_width + 10) * col)
    y = 5 + ((table_height + 10) * row)
    table_name = "tbl" + str(i)
    createTable(x, y, table_width, table_height, 2, 2, table_name)

# Define the three borders to use.
defineColorRGB("Red", 255, 128, 128)
defineColorRGB("Green", 128, 255, 136)
defineColorRGB("Yellow", 255, 253, 128)
defineColorRGB("Blue", 128, 146, 255)
defineColorRGB("Gray", 200, 200, 200)
red = [(24, LINE_SOLID, "Gray"), (8, LINE_SOLID, "Red")]
green = [(24, LINE_SOLID, "Gray"), (8, LINE_SOLID, "Green")]
yellow = [(24, LINE_SOLID, "Gray"), (8, LINE_SOLID, "Yellow")]
blue = [(24, LINE_SOLID, "Gray"), (8, LINE_SOLID, "Blue")]

# Set up the borders for each of the 43 test cases.

# Case 1
setCellRightBorder(0, 0, red, "tbl0")
setCellRightBorder(1, 0, blue, "tbl0")
setCellBottomBorder(0, 1, yellow, "tbl0")
setCellBottomBorder(0, 0, green, "tbl0")

# Case 2
setCellRightBorder(0, 0, green, "tbl1")
setCellRightBorder(1, 0, blue, "tbl1")
setCellBottomBorder(0, 1, yellow, "tbl1")
setCellBottomBorder(0, 0, green, "tbl1")

# Case 3
setCellRightBorder(0, 0, red, "tbl2")
setCellRightBorder(1, 0, blue, "tbl2")
setCellBottomBorder(0, 1, red, "tbl2")
setCellBottomBorder(0, 0, green, "tbl2")

# Case 4
setCellRightBorder(0, 0, red, "tbl3")
setCellRightBorder(1, 0, yellow, "tbl3")
setCellBottomBorder(0, 1, yellow, "tbl3")
setCellBottomBorder(0, 0, green, "tbl3")

# Case 5
setCellRightBorder(0, 0, red, "tbl4")
setCellRightBorder(1, 0, blue, "tbl4")
setCellBottomBorder(0, 1, yellow, "tbl4")
setCellBottomBorder(0, 0, blue, "tbl4")

# Case 6
setCellRightBorder(0, 0, green, "tbl5")
setCellRightBorder(1, 0, blue, "tbl5")
setCellBottomBorder(0, 1, green, "tbl5")
setCellBottomBorder(0, 0, green, "tbl5")

# Case 7
setCellRightBorder(0, 0, red, "tbl6")
setCellRightBorder(1, 0, red, "tbl6")
setCellBottomBorder(0, 1, red, "tbl6")
setCellBottomBorder(0, 0, green, "tbl6")

# Case 8
setCellRightBorder(0, 0, red, "tbl7")
setCellRightBorder(1, 0, yellow, "tbl7")
setCellBottomBorder(0, 1, yellow, "tbl7")
setCellBottomBorder(0, 0, yellow, "tbl7")

# Case 9
setCellRightBorder(0, 0, blue, "tbl8")
setCellRightBorder(1, 0, blue, "tbl8")
setCellBottomBorder(0, 1, yellow, "tbl8")
setCellBottomBorder(0, 0, blue, "tbl8")

# Case 10
setCellRightBorder(0, 0, green, "tbl9")
setCellRightBorder(1, 0, green, "tbl9")
setCellBottomBorder(0, 1, green, "tbl9")
setCellBottomBorder(0, 0, green, "tbl9")

# Case 11
setCellRightBorder(0, 0, blue, "tbl10")
setCellRightBorder(1, 0, blue, "tbl10")
setCellBottomBorder(0, 1, green, "tbl10")
setCellBottomBorder(0, 0, green, "tbl10")

# Case 12
setCellRightBorder(0, 0, yellow, "tbl11")
setCellRightBorder(1, 0, red, "tbl11")
setCellBottomBorder(0, 1, green, "tbl11")
setCellBottomBorder(0, 0, green, "tbl11")

# Case 13
setCellRightBorder(0, 0, green, "tbl12")
setCellRightBorder(1, 0, green, "tbl12")
setCellBottomBorder(0, 1, yellow, "tbl12")
setCellBottomBorder(0, 0, red, "tbl12")

# Case 14
setCellRightBorder(0, 0, blue, "tbl13")
setCellRightBorder(1, 0, green, "tbl13")
setCellBottomBorder(0, 1, green, "tbl13")
setCellBottomBorder(0, 0, blue, "tbl13")

# Case 15
setCellRightBorder(0, 0, blue, "tbl14")
setCellRightBorder(1, 0, green, "tbl14")
setCellBottomBorder(0, 1, blue, "tbl14")
setCellBottomBorder(0, 0, green, "tbl14")

# Case 16
setCellRightBorder(1, 0, green, "tbl15")
setCellBottomBorder(0, 1, yellow, "tbl15")
setCellBottomBorder(0, 0, red, "tbl15")

# Case 17
setCellRightBorder(1, 0, green, "tbl16")
setCellBottomBorder(0, 1, yellow, "tbl16")
setCellBottomBorder(0, 0, green, "tbl16")

# Case 18
setCellRightBorder(1, 0, green, "tbl17")
setCellBottomBorder(0, 1, green, "tbl17")
setCellBottomBorder(0, 0, red, "tbl17")

# Case 19
setCellRightBorder(1, 0, green, "tbl18")
setCellBottomBorder(0, 1, green, "tbl18")
setCellBottomBorder(0, 0, green, "tbl18")

# Case 20
setCellRightBorder(1, 0, red, "tbl19")
setCellBottomBorder(0, 1, green, "tbl19")
setCellBottomBorder(0, 0, green, "tbl19")

# Case 21
setCellRightBorder(0, 0, green, "tbl20")
setCellBottomBorder(0, 1, yellow, "tbl20")
setCellBottomBorder(0, 0, red, "tbl20")

# Case 22
setCellRightBorder(0, 0, green, "tbl21")
setCellBottomBorder(0, 1, yellow, "tbl21")
setCellBottomBorder(0, 0, green, "tbl21")

# Case 23
setCellRightBorder(0, 0, green, "tbl22")
setCellBottomBorder(0, 1, green, "tbl22")
setCellBottomBorder(0, 0, red, "tbl22")

# Case 24
setCellRightBorder(0, 0, green, "tbl23")
setCellBottomBorder(0, 1, green, "tbl23")
setCellBottomBorder(0, 0, green, "tbl23")

# Case 25
setCellRightBorder(0, 0, red, "tbl24")
setCellBottomBorder(0, 1, green, "tbl24")
setCellBottomBorder(0, 0, green, "tbl24")

# Case 26
setCellRightBorder(0, 0, yellow, "tbl25")
setCellRightBorder(1, 0, red, "tbl25")
setCellBottomBorder(0, 0, green, "tbl25")

# Case 27
setCellRightBorder(0, 0, yellow, "tbl26")
setCellRightBorder(1, 0, green, "tbl26")
setCellBottomBorder(0, 0, green, "tbl26")

# Case 28
setCellRightBorder(0, 0, green, "tbl27")
setCellRightBorder(1, 0, red, "tbl27")
setCellBottomBorder(0, 0, green, "tbl27")

# Case 29
setCellRightBorder(0, 0, green, "tbl28")
setCellRightBorder(1, 0, green, "tbl28")
setCellBottomBorder(0, 0, green, "tbl28")

# Case 30
setCellRightBorder(0, 0, green, "tbl29")
setCellRightBorder(1, 0, green, "tbl29")
setCellBottomBorder(0, 0, red, "tbl29")

# Case 31
setCellRightBorder(0, 0, yellow, "tbl30")
setCellRightBorder(1, 0, red, "tbl30")
setCellBottomBorder(0, 1, green, "tbl30")

# Case 32
setCellRightBorder(0, 0, yellow, "tbl31")
setCellRightBorder(1, 0, green, "tbl31")
setCellBottomBorder(0, 1, green, "tbl31")

# Case 33
setCellRightBorder(0, 0, green, "tbl32")
setCellRightBorder(1, 0, red, "tbl32")
setCellBottomBorder(0, 1, green, "tbl32")

# Case 34
setCellRightBorder(0, 0, green, "tbl33")
setCellRightBorder(1, 0, green, "tbl33")
setCellBottomBorder(0, 1, green, "tbl33")

# Case 35
setCellRightBorder(0, 0, green, "tbl34")
setCellRightBorder(1, 0, green, "tbl34")
setCellBottomBorder(0, 1, red, "tbl34")

# Case 36
setCellRightBorder(1, 0, green, "tbl35")
setCellBottomBorder(0, 1, green, "tbl35")

# Case 37
setCellRightBorder(1, 0, green, "tbl36")
setCellBottomBorder(0, 1, red, "tbl36")

# Case 38
setCellRightBorder(1, 0, green, "tbl37")
setCellBottomBorder(0, 0, green, "tbl37")

# Case 39
setCellRightBorder(1, 0, green, "tbl38")
setCellBottomBorder(0, 0, red, "tbl38")

# Case 40
setCellRightBorder(0, 0, green, "tbl39")
setCellBottomBorder(0, 1, green, "tbl39")

# Case 41
setCellRightBorder(0, 0, green, "tbl40")
setCellBottomBorder(0, 1, red, "tbl40")

# Case 42
setCellRightBorder(0, 0, green, "tbl41")
setCellBottomBorder(0, 0, green, "tbl41")

# Case 43
setCellRightBorder(0, 0, green, "tbl42")
setCellBottomBorder(0, 0, red, "tbl42")

