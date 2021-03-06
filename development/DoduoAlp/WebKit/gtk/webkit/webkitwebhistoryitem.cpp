/*
 * Copyright (C) 2008 Jan Michael C. Alonzo
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
 */


#include "config.h"

#include "webkitwebhistoryitem.h"
#include "webkitprivate.h"

#include <glib.h>

#include "CString.h"
#include "HistoryItem.h"
#include "PlatformString.h"

using namespace WebKit;

extern "C" {

struct _WebKitWebHistoryItemPrivate {
    WTF::RefPtr<WebCore::HistoryItem> historyItem;

    WebCore::CString title;
    WebCore::CString alternateTitle;
    WebCore::CString uri;
    WebCore::CString originalUri;
};

#define WEBKIT_WEB_HISTORY_ITEM_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE((obj), WEBKIT_TYPE_WEB_HISTORY_ITEM, WebKitWebHistoryItemPrivate))

enum {
    PROP_0,

    PROP_TITLE,
    PROP_ALTERNATE_TITLE,
    PROP_URI,
    PROP_ORIGINAL_URI,
    PROP_LAST_VISITED_TIME
};

G_DEFINE_TYPE(WebKitWebHistoryItem, webkit_web_history_item, G_TYPE_OBJECT);

static void webkit_web_history_item_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec);

static void webkit_web_history_item_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec);

static GHashTable* webkit_history_items()
{
    static GHashTable* historyItems = g_hash_table_new(g_direct_hash, g_direct_equal);
    return historyItems;
}

static void webkit_history_item_add(WebKitWebHistoryItem* webHistoryItem, WebCore::HistoryItem* historyItem)
{
    g_return_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem));

    GHashTable* table = webkit_history_items();

    g_hash_table_insert(table, historyItem, g_object_ref(webHistoryItem));
}

static void webkit_history_item_remove(WebCore::HistoryItem* historyItem)
{
    GHashTable* table = webkit_history_items();
    WebKitWebHistoryItem* webHistoryItem = (WebKitWebHistoryItem*) g_hash_table_lookup(table, historyItem);

    g_return_if_fail(webHistoryItem != NULL);

    g_hash_table_remove(table, historyItem);
    g_object_unref(webHistoryItem);
}

static void webkit_web_history_item_dispose(GObject* object)
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(object);

    webkit_history_item_remove(core(webHistoryItem));

    /* destroy table if empty */
    GHashTable* table = webkit_history_items();
    if (!g_hash_table_size(table))
        g_hash_table_destroy(table);

    G_OBJECT_CLASS(webkit_web_history_item_parent_class)->dispose(object);
}

static void webkit_web_history_item_finalize(GObject* object)
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(object);
    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;

    priv->title = WebCore::CString();
    priv->alternateTitle = WebCore::CString();
    priv->uri = WebCore::CString();
    priv->originalUri = WebCore::CString();

    G_OBJECT_CLASS(webkit_web_history_item_parent_class)->finalize(object);
}

static void webkit_web_history_item_class_init(WebKitWebHistoryItemClass* klass)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->dispose = webkit_web_history_item_dispose;
    gobject_class->finalize = webkit_web_history_item_finalize;
    gobject_class->set_property = webkit_web_history_item_set_property;
    gobject_class->get_property = webkit_web_history_item_get_property;

    /**
    * WebKitWebHistoryItem:title:
    *
    * The title of the history item.
    *
    * Since: 1.0.2
    */
    g_object_class_install_property(gobject_class,
                                    PROP_TITLE,
                                    g_param_spec_string(
                                    "title",
                                    "Title",
                                    "The title of the history item",
                                    NULL,
                                    WEBKIT_PARAM_READABLE));

    /**
    * WebKitWebHistoryItem:alternate-title:
    *
    * The alternate title of the history item.
    *
    * Since: 1.0.2
    */
    g_object_class_install_property(gobject_class,
                                    PROP_ALTERNATE_TITLE,
                                    g_param_spec_string(
                                    "alternate-title",
                                    "Alternate Title",
                                    "The alternate title of the history item",
                                    NULL,
                                    WEBKIT_PARAM_READWRITE));

    /**
    * WebKitWebHistoryItem:uri:
    *
    * The URI of the history item.
    *
    * Since: 1.0.2
    */
    g_object_class_install_property(gobject_class,
                                    PROP_URI,
                                    g_param_spec_string(
                                    "uri",
                                    "URI",
                                    "The URI of the history item",
                                    NULL,
                                    WEBKIT_PARAM_READABLE));

    /**
    * WebKitWebHistoryItem:original-uri:
    *
    * The original URI of the history item.
    *
    * Since: 1.0.2
    */
    g_object_class_install_property(gobject_class,
                                    PROP_ORIGINAL_URI,
                                    g_param_spec_string(
                                    "original-uri",
                                    "Original URI",
                                    "The original URI of the history item",
                                    NULL,
                                    WEBKIT_PARAM_READABLE));

   /**
    * WebKitWebHistoryItem:last-visited-time:
    *
    * The time at which the history item was last visited.
    *
    * Since: 1.0.2
    */
    g_object_class_install_property(gobject_class,
                                    PROP_LAST_VISITED_TIME,
                                    g_param_spec_double(
                                    "last-visited-time",
                                    "Last visited Time",
                                    "The time at which the history item was last visited",
                                    0, G_MAXDOUBLE, 0,
                                    WEBKIT_PARAM_READABLE));

    g_type_class_add_private(gobject_class, sizeof(WebKitWebHistoryItemPrivate));
}

static void webkit_web_history_item_init(WebKitWebHistoryItem* webHistoryItem)
{
    webHistoryItem->priv = WEBKIT_WEB_HISTORY_ITEM_GET_PRIVATE(webHistoryItem);
}

static void webkit_web_history_item_set_property(GObject* object, guint prop_id, const GValue* value, GParamSpec* pspec)
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(object);

    switch(prop_id) {
    case PROP_ALTERNATE_TITLE:
        webkit_web_history_item_set_alternate_title(webHistoryItem, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void webkit_web_history_item_get_property(GObject* object, guint prop_id, GValue* value, GParamSpec* pspec)
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(object);

    switch (prop_id) {
    case PROP_TITLE:
        g_value_set_string(value, webkit_web_history_item_get_title(webHistoryItem));
        break;
    case PROP_ALTERNATE_TITLE:
        g_value_set_string(value, webkit_web_history_item_get_alternate_title(webHistoryItem));
        break;
    case PROP_URI:
        g_value_set_string(value, webkit_web_history_item_get_uri(webHistoryItem));
        break;
    case PROP_ORIGINAL_URI:
        g_value_set_string(value, webkit_web_history_item_get_original_uri(webHistoryItem));
        break;
    case PROP_LAST_VISITED_TIME:
        g_value_set_double(value, webkit_web_history_item_get_last_visited_time(webHistoryItem));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/* Helper function to create a new WebHistoryItem instance when needed */
WebKitWebHistoryItem* webkit_web_history_item_new_with_core_item(WebCore::HistoryItem* item)
{
    WebKitWebHistoryItem* webHistoryItem = kit(item);

    if (webHistoryItem)
        g_object_ref(webHistoryItem);
    else {
        webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(g_object_new(WEBKIT_TYPE_WEB_HISTORY_ITEM, NULL));
        WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;

        priv->historyItem = item;
        webkit_history_item_add(webHistoryItem, priv->historyItem.get());
    }

    return webHistoryItem;
}


/**
 * webkit_web_history_item_new:
 *
 * Creates a new #WebKitWebHistoryItem instance
 *
 * Return value: the new #WebKitWebHistoryItem
 */
WebKitWebHistoryItem* webkit_web_history_item_new()
{
    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(g_object_new(WEBKIT_TYPE_WEB_HISTORY_ITEM, NULL));
    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;

    priv->historyItem = WebCore::HistoryItem::create();
    webkit_history_item_add(webHistoryItem, priv->historyItem.get());

    return webHistoryItem;
}

/**
 * webkit_web_history_item_new_with_data:
 * @uri: the uri of the page
 * @title: the title of the page
 *
 * Creates a new #WebKitWebHistoryItem with the given URI and title
 *
 * Return value: the new #WebKitWebHistoryItem
 */
WebKitWebHistoryItem* webkit_web_history_item_new_with_data(const gchar* uri, const gchar* title)
{
    WebCore::KURL historyUri(uri);
    WebCore::String historyTitle = WebCore::String::fromUTF8(title);

    WebKitWebHistoryItem* webHistoryItem = WEBKIT_WEB_HISTORY_ITEM(g_object_new(WEBKIT_TYPE_WEB_HISTORY_ITEM, NULL));
    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;

    priv->historyItem = WebCore::HistoryItem::create(historyUri, historyTitle, 0);
    webkit_history_item_add(webHistoryItem, priv->historyItem.get());

    return webHistoryItem;
}

/**
 * webkit_web_history_item_get_title:
 * @webHistoryItem: a #WebKitWebHistoryItem
 *
 * Returns the page title of @webHistoryItem
 */
G_CONST_RETURN gchar* webkit_web_history_item_get_title(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebCore::HistoryItem* item = core(webHistoryItem);

    g_return_val_if_fail(item != NULL, NULL);

    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;
    priv->title = item->title().utf8();

    return priv->title.data();
}

/**
 * webkit_web_history_item_get_alternate_title:
 * @webHistoryItem: a #WebKitWebHistoryItem
 *
 * Returns the alternate title of @webHistoryItem
 *
 * Return value: the alternate title of @webHistoryItem
 */
G_CONST_RETURN gchar* webkit_web_history_item_get_alternate_title(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebCore::HistoryItem* item = core(webHistoryItem);

    g_return_val_if_fail(item != NULL, NULL);

    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;
    priv->alternateTitle = item->alternateTitle().utf8();

    return priv->alternateTitle.data();
}

/**
 * webkit_web_history_item_set_alternate_title:
 * @webHistoryItem: a #WebKitWebHistoryItem
 * @title: the alternate title for @this history item
 *
 * Sets an alternate title for @webHistoryItem
 */
void webkit_web_history_item_set_alternate_title(WebKitWebHistoryItem* webHistoryItem, const gchar* title)
{
    g_return_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem));
    g_return_if_fail(title);

    WebCore::HistoryItem* item = core(webHistoryItem);

    item->setAlternateTitle(WebCore::String::fromUTF8(title));
    g_object_notify(G_OBJECT(webHistoryItem), "alternate-title");
}

/**
 * webkit_web_history_item_get_uri:
 * @webHistoryItem: a #WebKitWebHistoryItem
 *
 * Returns the URI of @this
 *
 * Return value: the URI of @webHistoryItem
 */
G_CONST_RETURN gchar* webkit_web_history_item_get_uri(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebCore::HistoryItem* item = core(WEBKIT_WEB_HISTORY_ITEM(webHistoryItem));

    g_return_val_if_fail(item != NULL, NULL);

    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;
    priv->uri = item->urlString().utf8();

    return priv->uri.data();
}

/**
 * webkit_web_history_item_get_original_uri:
 * @webHistoryItem: a #WebKitWebHistoryItem
 *
 * Returns the original URI of @webHistoryItem.
 *
 * Return value: the original URI of @webHistoryITem
 */
G_CONST_RETURN gchar* webkit_web_history_item_get_original_uri(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebCore::HistoryItem* item = core(WEBKIT_WEB_HISTORY_ITEM(webHistoryItem));

    g_return_val_if_fail(item != NULL, NULL);

    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;
    priv->originalUri = item->originalURLString().utf8();

    return webHistoryItem->priv->originalUri.data();
}

/**
 * webkit_web_history_item_get_last_visisted_time :
 * @webHistoryItem: a #WebKitWebHistoryItem
 *
 * Returns the last time @webHistoryItem was visited
 *
 * Return value: the time in seconds this @webHistoryItem was last visited
 */
gdouble webkit_web_history_item_get_last_visited_time(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), 0);

    WebCore::HistoryItem* item = core(WEBKIT_WEB_HISTORY_ITEM(webHistoryItem));

    g_return_val_if_fail(item != NULL, 0);

    return item->lastVisitedTime();
}

} /* end extern "C" */

WebCore::HistoryItem* WebKit::core(WebKitWebHistoryItem* webHistoryItem)
{
    g_return_val_if_fail(WEBKIT_IS_WEB_HISTORY_ITEM(webHistoryItem), NULL);

    WebKitWebHistoryItemPrivate* priv = webHistoryItem->priv;
    WTF::RefPtr<WebCore::HistoryItem> historyItem = priv->historyItem;

    return historyItem ? historyItem.get() : 0;
}

WebKitWebHistoryItem* WebKit::kit(WebCore::HistoryItem* historyItem)
{
    g_return_val_if_fail(historyItem != NULL, NULL);

    WebKitWebHistoryItem* webHistoryItem;
    GHashTable* table = webkit_history_items();

    webHistoryItem = (WebKitWebHistoryItem*) g_hash_table_lookup(table, historyItem);
    return webHistoryItem;
}

