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

#ifndef WebHistoryItem_H
#define WebHistoryItem_H

#include <string>
#include <vector>

/**
 *  @file  WebHistoryItem.h
 *  WebHistoryItem description
 *  Repository informations :
 * - $URL$
 * - $Rev$
 * - $Date$
 */

class WebHistoryItemPrivate;

class WebHistoryItem
{
public:

    /**
     *  createInstance create an Instance of a WebHistoryItem
     * @param[in]: 
     * @param[out]: WebHistoryItem
     */
    static WebHistoryItem* createInstance();

    /**
     *  createInstance create an Instance of a WebHistoryItem
     * @param[in]: HistoryItem from WebCore
     * @param[out]: WebHistoryItem
     */
    static WebHistoryItem* createInstance(WebHistoryItemPrivate*);
protected:

    /**
     *  WebHistoryItem constructor
     * @param[in]: WebCore History Item
     * @param[out]: 
     */
    WebHistoryItem(WebHistoryItemPrivate*);

public:

    /**
     *  ~WebHistoryItem destructor
     */
    virtual ~WebHistoryItem();

    /**
        @method initWithURLString:title:lastVisitedTimeInterval:
        @param URLString The URL string for the item.
        @param title The title to use for the item.  This is normally the <title> of a page.
        @param time The time used to indicate when the item was used.
        @abstract Initialize a new WebHistoryItem
        @discussion WebHistoryItems are normally created for you by the WebKit.
        You may use this method to prepopulate a WebBackForwardList, or create
        'artificial' items to add to a WebBackForwardList.  When first initialized
        the URLString and originalURLString will be the same.
     */
    virtual void initWithURLString(const char* urlString, const char* title, double lastVisited);

    /**
        @method originalURLString
        @abstract The string representation of the originial URL of this item.
        This value is normally set by the WebKit.
        @result The string corresponding to the initial URL of this item.

     */
    virtual const char* originalURLString();

    /**
        @method URLString
        @abstract The string representation of the URL represented by this item.
        @discussion The URLString may be different than the originalURLString if the page
        redirected to a new location.  This value is normally set by the WebKit.
        @result The string corresponding to the final URL of this item.

     */
    virtual const char* URLString();

    /**
        @method title
        @abstract The title of the page represented by this item.
        @discussion This title cannot be changed by the client.  This value
        is normally set by the WebKit when a page title for the item is received.
        @result The title of this item.

     */
    virtual const char* title();

    /**
        @method lastVisitedTimeInterval
        @abstract The last time the page represented by this item was visited. The interval
        is since the reference date as determined by NSDate.  This value is normally set by
        the WebKit.
        @result The last time this item was visited.

     */
    virtual double lastVisitedTimeInterval();

    /**
      @method setAlternateTitle:
        @param alternateTitle The new display title for this item.
        @abstract A title that may be used by the client to display this item.

     */
    virtual void setAlternateTitle(const char* title);

    /**
        @method title
        @abstract A title that may be used by the client to display this item.
        @result The alternate title for this item.

     */
    virtual const char* alternateTitle();

    /**
        @method icon
        @abstract The favorite icon of the page represented by this item.
        @discussion This icon returned will be determined by the WebKit.
        @result The icon associated with this item's URL.

     */
//    virtual WebCore::Image* icon();


    /**
        @method returns a counter of visits
     */
    virtual int visitCount();

    /**
     * @method setVisitCount sets visit count
     * @param[in]: count figure
     */
    virtual void setVisitCount(int count);

    /**
     *  hasURLString return whether there is a name or not
     * @param[in]: 
     * @param[out]: boolean
     * @code
     * @endcode
     */
    virtual bool hasURLString();

    /**
     *  mergeAutoCompleteHints add visit count
     * @param[in]: item to increment
     */
    virtual void mergeAutoCompleteHints(WebHistoryItem* otherItem);

    /**
       @method sets last visit time
     */
    virtual void setLastVisitedTimeInterval(double time);

    /**
     *  setTitle description
     * @param[in]: time
     * @param[out]: 
     */
    virtual void setTitle(const char* title);

    /**
     *  RSSFeedReferrer  returns the Feed Referred
     * @param[in]: 
     * @param[out]: 
     */
    virtual const char* RSSFeedReferrer();

    /**
     *  setRSSFeedReferrer sets the feed referrer
     * @param[in]: url 
     * @param[out]: 
     */
    virtual void setRSSFeedReferrer(const char* url);

    /**
     *  hasPageCache determines if a page exists in cache
     * @param[in]: 
     * @param[out]: boolean
     */
    virtual bool hasPageCache();

    /**
     *  setHasPageCache sets existence of page cahche
     * @param[in]: boolean
     * @param[out]: 
     */
    virtual void setHasPageCache(bool hasCache);

    /**
     *  target returns string name of the last target
     * @param[in]: 
     * @param[out]: name of the target
     */
    virtual const char* target();

    /**
     *  isTargetItem tells if the item is a target
     * @param[in]: 
     * @param[out]: boolean
     */
    virtual bool isTargetItem();

    /**
     *  children list of history items
     * @param[in]: 
     * @param[out]: 
     */
    virtual std::vector<WebHistoryItem*> children();

    /**
     * lastVisitWasFailure
     */
    virtual bool lastVisitWasFailure();

    /*
     * setLastVisitWasFailure
     */
    virtual void setLastVisitWasFailure(bool wasFailure);

    /**
     * lastVisitWasHTTPNonGet
     */
    virtual bool lastVisitWasHTTPNonGet() const;

    /**
     * setLastVisitWasHTTPNonGet
     */
    virtual void setLastVisitWasHTTPNonGet(bool wasHttpNonGet);

    WebHistoryItemPrivate *getPrivateItem() { return d; }

protected:
    WebHistoryItemPrivate *d;
    std::string m_alternateTitle;
};

#endif
