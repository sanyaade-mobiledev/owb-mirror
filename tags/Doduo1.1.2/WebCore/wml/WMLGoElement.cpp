/**
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved.
 *               http://www.torchmobile.com/
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
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "config.h"

#if ENABLE(WML)
#include "WMLGoElement.h"

#include "WMLDocument.h"
#include "WMLPageState.h"

namespace WebCore {

WMLGoElement::WMLGoElement(const QualifiedName& tagName, Document* doc)
    : WMLTaskElement(tagName, doc)
{
}

WMLGoElement::~WMLGoElement()
{
}

void WMLGoElement::parseMappedAttribute(MappedAttribute* attr)
{
    WMLTaskElement::parseMappedAttribute(attr);
}

void WMLGoElement::executeTask(Event*)
{
    WMLPageState* pageState = wmlPageStateForDocument(document());
    if (!pageState)
        return;

    WMLCardElement* card = pageState->activeCard();
    if (!card)
        return;

    storeVariableState(pageState);
    // FIXME: Implement <go> functionality.
}

}

#endif
