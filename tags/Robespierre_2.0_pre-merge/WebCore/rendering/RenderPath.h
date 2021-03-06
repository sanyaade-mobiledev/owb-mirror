/*
    Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
                  2004, 2005 Rob Buis <buis@kde.org>
                  2005 Eric Seidel <eric.seidel@kdemail.net>
                  2006 Apple Computer, Inc

    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef RenderPath_h
#define RenderPath_h

#ifdef SVG_SUPPORT

#include "AffineTransform.h"
#include "FloatRect.h"

#include "RenderObject.h"

namespace WebCore {

class FloatPoint;
class Path;
class RenderSVGContainer;
class SVGStyledElement;

class RenderPath : public RenderObject
{
public:
    RenderPath(RenderStyle* style, SVGStyledElement *node);
    virtual ~RenderPath();

    // Hit-detection seperated for the fill and the stroke
    virtual bool fillContains(const FloatPoint&, bool requiresFill = true) const;
    virtual bool strokeContains(const FloatPoint&, bool requiresStroke = true) const;

    // Returns an unscaled bounding box (not even including localTransform()) for this vector path
    virtual FloatRect relativeBBox(bool includeStroke = true) const;

    const Path& path() const;
    void setPath(const Path& newPath);

    virtual bool isRenderPath() const { return true; }
    virtual const char* renderName() const { return "RenderPath"; }
    
#ifdef __OWB__
    virtual BAL::BTAffineTransform localTransform() const;
    virtual void setLocalTransform(const BAL::BTAffineTransform &matrix);
#else
    virtual AffineTransform localTransform() const;
    virtual void setLocalTransform(const AffineTransform& matrix);
#endif

    virtual void layout();
    virtual IntRect getAbsoluteRepaintRect();
    virtual bool requiresLayer();
    virtual short lineHeight(bool b, bool isRootLineBox = false) const;
    virtual short baselinePosition(bool b, bool isRootLineBox = false) const;
    virtual void paint(PaintInfo&, int parentX, int parentY);

    virtual void absoluteRects(Vector<IntRect>&, int tx, int ty);

    virtual bool nodeAtPoint(const HitTestRequest&, HitTestResult&, int x, int y, int tx, int ty, HitTestAction);

    FloatRect drawMarkersIfNeeded(GraphicsContext*, const FloatRect&, const Path&) const;
    virtual FloatRect strokeBBox() const;

    bool hasRelativeValues() const;
 
private:
    FloatPoint mapAbsolutePointToLocal(const FloatPoint&) const;

    mutable Path m_path;
    mutable FloatRect m_fillBBox;
    mutable FloatRect m_strokeBbox;
    FloatRect m_markerBounds;
#ifdef __OWB__
    BAL::BTAffineTransform m_matrix;
#else
    AffineTransform m_matrix;
#endif
    IntRect m_absoluteBounds;
};

}

#endif // SVG_SUPPORT
#endif

// vim:ts=4:noet
