/*
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */
#ifndef HTMLStyleElement_h
#define HTMLStyleElement_h

#include "CSSStyleSheet.h"
#include "HTMLElement.h"
#include "StyleElement.h"

namespace WebCore {

class HTMLStyleElement : public HTMLElement, public StyleElement
{
public:
    HTMLStyleElement(Document*);

    virtual HTMLTagStatus endTagRequirement() const { return TagStatusRequired; }
    virtual int tagPriority() const { return 1; }
    virtual bool checkDTD(const Node* newChild) { return newChild->isTextNode(); }

    // overload from HTMLElement
    virtual void parseMappedAttribute(MappedAttribute*);
    virtual void insertedIntoDocument();
    virtual void removedFromDocument();
    virtual void childrenChanged();

    virtual bool isLoading() const;
    virtual bool sheetLoaded();

    bool disabled() const;
    void setDisabled(bool);

    const AtomicString& media() const;
    void setMedia(const AtomicString&);

    const AtomicString& type() const;
    void setType(const AtomicString&);

protected:
    String m_media;
};

} //namespace

#endif
