/*
 * Copyright (C) 2009 Pleyo.  All rights reserved.
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

#ifndef DumpRenderTreeBal_h
#define DumpRenderTreeBal_h

#include <JavaScriptCore/JSBase.h>
#include <WebKit.h>
#include <string>
#include <vector>
#include <time.h>

#if ENABLE(DAE)
#include "WebApplicationManager.h"
#define EVENT_SINK          (&(webAppMgr()))
#else
#define EVENT_SINK          getWebView()
#endif

class PolicyDelegate;
#if PLATFORM(GTK)|| PLATFORM(QT)
extern unsigned int waitToDumpWatchdog;
#else
extern timer_t waitToDumpWatchdog;
#endif
extern WebFrame* topLoadingFrame;
extern SharedPtr<PolicyDelegate> policyDelegate;
extern std::vector<std::string> disallowedURLs;

char* JSStringCopyUTF8CString(JSStringRef jsString);

void addTimetoDump(int timeoutSeconds);
void removeTimer();

WebView *getWebView();
bool getDone();
void setDone(bool);

extern std::string dumpPath(DOMNode* node);

void setPersistentUserStyleSheetLocation(const char*);

void getBalPoint(int x, int y, BalPoint* point);
int getX(BalPoint pos);
int getY(BalPoint pos);
#endif // DumpRenderTreeGtk_h
