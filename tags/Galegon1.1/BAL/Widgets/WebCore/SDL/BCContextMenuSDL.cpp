/*
 * Copyright (C) 2008 Pleyo.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Pleyo nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PLEYO AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PLEYO OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "config.h"
#include "ContextMenu.h"

#include "ContextMenuController.h"

#include <cstdio>

namespace WebCore {

// TODO: ref-counting correctness checking.
// See http://bugs.webkit.org/show_bug.cgi?id=16115

ContextMenu::ContextMenu(const HitTestResult& result)
    : m_hitTestResult(result)
{
    printf("ContextMenu::ContextMenu\n");
}

ContextMenu::~ContextMenu()
{
    printf("ContextMenu::~ContextMenu\n");
}

void ContextMenu::appendItem(ContextMenuItem& item)
{
    printf("ContextMenu::appendItem\n");
}

void ContextMenu::setPlatformDescription(PlatformMenuDescription menu)
{
    printf("ContextMenu::setPlatformDescription\n");
}

PlatformMenuDescription ContextMenu::platformDescription() const
{
    printf("ContextMenu::platformDescription\n");
    return m_platformDescription;
}

PlatformMenuDescription ContextMenu::releasePlatformDescription()
{
    printf("ContextMenu::releasePlatformDescriptio\n");
    PlatformMenuDescription description = m_platformDescription;
    m_platformDescription = 0;

    return description;
}

}
