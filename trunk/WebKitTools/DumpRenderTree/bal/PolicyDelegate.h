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

#ifndef PolicyDelegate_h
#define PolicyDelegate_h

#include <WebKit.h>

class LayoutTestController;

class PolicyDelegate : public WebPolicyDelegate {
public:
    static TransferSharedPtr<PolicyDelegate> createInstance();

    // WebPolicyDelegate
    virtual void decidePolicyForNavigationAction(
        /* [in] */ WebView *webView,
        /* [in] */ WebNavigationAction *actionInformation,
        /* [in] */ WebMutableURLRequest *request,
        /* [in] */ WebFrame *frame,
        /* [in] */ WebFramePolicyListener *listener); 
    
    virtual void decidePolicyForNewWindowAction(
        /* [in] */ WebView *webView,
        /* [in] */ WebNavigationAction *actionInformation,
        /* [in] */ WebMutableURLRequest *request,
        /* [in] */ const char* frameName,
        /* [in] */ WebFramePolicyListener *listener) {}
    
    virtual bool decidePolicyForMIMEType(
        /* [in] */ WebView *webView,
        /* [in] */ const char* type,
        /* [in] */ WebMutableURLRequest *request,
        /* [in] */ WebFrame *frame,
        /* [in] */ WebFramePolicyListener *listener);
    
    virtual void unableToImplementPolicyWithError(
        /* [in] */ WebView *webView,
        /* [in] */ WebError *error,
        /* [in] */ WebFrame *frame);

    // PolicyDelegate
    void setPermissive(bool permissive) { m_permissiveDelegate = permissive; }
    void setControllerToNotifyDone(LayoutTestController* controller) { m_controllerToNotifyDone = controller; }

private:
    PolicyDelegate();
    bool m_permissiveDelegate;
    LayoutTestController* m_controllerToNotifyDone;
};

#endif // PolicyDelegate_h
