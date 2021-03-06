# Copyright (C) 2009 Google Inc. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#    * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#    * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import unittest
import diff_parser


class DiffParserTest(unittest.TestCase):

    _PATCH = '''diff --git a/WebCore/rendering/style/StyleFlexibleBoxData.h b/WebCore/rendering/style/StyleFlexibleBoxData.h
index f5d5e74..3b6aa92 100644
--- WebKitTools/a/WebCore/rendering/style/StyleFlexibleBoxData.h
+++ WebKitTools/b/WebCore/rendering/style/StyleFlexibleBoxData.h
@@ -47,7 +47,6 @@ public:
 
     unsigned align : 3; // EBoxAlignment
     unsigned pack: 3; // EBoxAlignment
-    unsigned orient: 1; // EBoxOrient
     unsigned lines : 1; // EBoxLines
 
 private:
diff --git a/WebCore/rendering/style/StyleRareInheritedData.cpp b/WebCore/rendering/style/StyleRareInheritedData.cpp
index ce21720..324929e 100644
--- WebKitTools/a/WebCore/rendering/style/StyleRareInheritedData.cpp
+++ WebKitTools/b/WebCore/rendering/style/StyleRareInheritedData.cpp
@@ -39,6 +39,7 @@ StyleRareInheritedData::StyleRareInheritedData()
     , textSizeAdjust(RenderStyle::initialTextSizeAdjust())
     , resize(RenderStyle::initialResize())
     , userSelect(RenderStyle::initialUserSelect())
+    , boxOrient(RenderStyle::initialBoxOrient())
 {
 }
 
@@ -58,6 +59,7 @@ StyleRareInheritedData::StyleRareInheritedData(const StyleRareInheritedData& o)
     , textSizeAdjust(o.textSizeAdjust)
     , resize(o.resize)
     , userSelect(o.userSelect)
+    , boxOrient(o.boxOrient)
 {
 }
 
@@ -81,7 +83,8 @@ bool StyleRareInheritedData::operator==(const StyleRareInheritedData& o) const
         && khtmlLineBreak == o.khtmlLineBreak
         && textSizeAdjust == o.textSizeAdjust
         && resize == o.resize
-        && userSelect == o.userSelect;
+        && userSelect == o.userSelect
+        && boxOrient == o.boxOrient;
 }
 
 bool StyleRareInheritedData::shadowDataEquivalent(const StyleRareInheritedData& o) const
'''.splitlines()


    def test_diff_parser(self):
        parser = diff_parser.DiffParser(self._PATCH)
        self.assertEquals(2, len(parser.files))

        self.assertTrue('WebCore/rendering/style/StyleFlexibleBoxData.h' in parser.files)
        diff = parser.files['WebCore/rendering/style/StyleFlexibleBoxData.h']
        self.assertEquals(7, len(diff.lines))
        # The first two unchaged lines.
        self.assertEquals((47, 47), diff.lines[0][0:2])
        self.assertEquals('', diff.lines[0][2])
        self.assertEquals((48, 48), diff.lines[1][0:2])
        self.assertEquals('    unsigned align : 3; // EBoxAlignment', diff.lines[1][2])
        # The deleted line
        self.assertEquals((50, 0), diff.lines[3][0:2])
        self.assertEquals('    unsigned orient: 1; // EBoxOrient', diff.lines[3][2])

        # The first file looks OK. Let's check the next, more complicated file.
        self.assertTrue('WebCore/rendering/style/StyleRareInheritedData.cpp' in parser.files)
        diff = parser.files['WebCore/rendering/style/StyleRareInheritedData.cpp']
        # There are 3 chunks.
        self.assertEquals(7 + 7 + 9, len(diff.lines))
        # Around an added line.
        self.assertEquals((60, 61), diff.lines[9][0:2])
        self.assertEquals((0, 62), diff.lines[10][0:2])
        self.assertEquals((61, 63), diff.lines[11][0:2])
        # Look through the last chunk, which contains both add's and delete's.
        self.assertEquals((81, 83), diff.lines[14][0:2])
        self.assertEquals((82, 84), diff.lines[15][0:2])
        self.assertEquals((83, 85), diff.lines[16][0:2])
        self.assertEquals((84, 0), diff.lines[17][0:2])
        self.assertEquals((0, 86), diff.lines[18][0:2])
        self.assertEquals((0, 87), diff.lines[19][0:2])
        self.assertEquals((85, 88), diff.lines[20][0:2])
        self.assertEquals((86, 89), diff.lines[21][0:2])
        self.assertEquals((87, 90), diff.lines[22][0:2])


if __name__ == '__main__':
    unittest.main()
