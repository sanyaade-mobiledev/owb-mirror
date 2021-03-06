/*
 * Copyright (C) 2008, 2009 Julien Chaffraix <julien.chaffraix@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CookieManager_h
#define CookieManager_h

#include "CookieMap.h"
#include "PlatformString.h"
#include <wtf/HashMap.h>

namespace WebCore {

    class Cookie;
    class KURL;
    class String;

    enum BackingStoreRemoval {
        RemoveFromBackingStore,
        DoNotRemoveFromBackingStore
    };

    enum HttpOnlyCookieFiltering {
        NoHttpOnlyCookie,
        WithHttpOnlyCookies
    };

    class CookieManager {
    public:

        void setCookies(const KURL& url, const String& value);

        String getCookie(const KURL& url, HttpOnlyCookieFiltering);

        void removeAllCookies(BackingStoreRemoval);

        unsigned short cookiesCount() { return m_count; }

        void setCookieJar(const char *);
        const String& cookieJar() { return m_cookieJarFileName; }

        // Count update method
        void removedCookie() { ASSERT(m_count > 0); m_count--; }

        // Constants getter.
        static unsigned int maxCookieLength() { return s_maxCookieLength; }

    private:
        friend CookieManager& cookieManager();

        CookieManager();
        ~CookieManager();

        void checkAndTreatCookie(Cookie* cookie);

        bool shouldRejectForSecurityReason(const Cookie*, const KURL&);

        void addCookieToMap(CookieMap* map, Cookie* cookie);
        void update(CookieMap* map, Cookie* prevCookie, Cookie* newCookie);

        HashMap<String, CookieMap*> m_managerMap;

        // Count all cookies, cookies are limited by max_count
        unsigned short m_count;

        String m_cookieJarFileName;

        // FIXME: This method should be removed.
        void getBackingStoreCookies();    

        // Max count constants.
        static const unsigned int s_globalMaxCookieCount = 300;
        static const unsigned int s_maxCookieCountPerHost = 20;
        // Cookie size limit of 4kB as advised per RFC2109
        static const unsigned int s_maxCookieLength = 4096;
    };

    // Get the global instance.
    CookieManager& cookieManager();

} // namespace WebCore

#endif // CookieManager_h
