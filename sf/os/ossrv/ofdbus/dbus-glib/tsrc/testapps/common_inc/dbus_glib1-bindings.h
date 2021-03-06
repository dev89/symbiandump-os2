/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/* Generated by dbus-binding-tool; do not edit! */

#include <glib/gtypes.h>
#include <glib/gerror.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

#ifndef DBUS_GLIB_CLIENT_WRAPPERS_com_example_DBusGlibObject1
#define DBUS_GLIB_CLIENT_WRAPPERS_com_example_DBusGlibObject1

static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
com_example_DBusGlibObject1_variable_args_method (DBusGProxy *proxy, const gboolean IN_b, const gint IN_n, const gint IN_i, const gint64 IN_x, const gint IN_q, const guint IN_u, const guint64 IN_t, const gdouble IN_d, const char * IN_s, const GArray* IN_a, gint* OUT_r, GError **error)

{
  return dbus_g_proxy_call (proxy, "Variable_args_method", error, G_TYPE_BOOLEAN, IN_b, G_TYPE_INT, IN_n, G_TYPE_INT, IN_i, G_TYPE_INT64, IN_x, G_TYPE_INT, IN_q, G_TYPE_UINT, IN_u, G_TYPE_UINT64, IN_t, G_TYPE_DOUBLE, IN_d, G_TYPE_STRING, IN_s, dbus_g_type_get_collection ("GArray", G_TYPE_INT), IN_a, G_TYPE_INVALID, G_TYPE_INT, OUT_r, G_TYPE_INVALID);
}

typedef void (*com_example_DBusGlibObject1_variable_args_method_reply) (DBusGProxy *proxy, gint OUT_r, GError *error, gpointer userdata);

static void
com_example_DBusGlibObject1_variable_args_method_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*) user_data;
  GError *error = NULL;
  gint OUT_r;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_INT, &OUT_r, G_TYPE_INVALID);
  (*(com_example_DBusGlibObject1_variable_args_method_reply)data->cb) (proxy, OUT_r, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
com_example_DBusGlibObject1_variable_args_method_async (DBusGProxy *proxy, const gboolean IN_b, const gint IN_n, const gint IN_i, const gint64 IN_x, const gint IN_q, const guint IN_u, const guint64 IN_t, const gdouble IN_d, const char * IN_s, const GArray* IN_a, com_example_DBusGlibObject1_variable_args_method_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "Variable_args_method", com_example_DBusGlibObject1_variable_args_method_async_callback, stuff, g_free, G_TYPE_BOOLEAN, IN_b, G_TYPE_INT, IN_n, G_TYPE_INT, IN_i, G_TYPE_INT64, IN_x, G_TYPE_INT, IN_q, G_TYPE_UINT, IN_u, G_TYPE_UINT64, IN_t, G_TYPE_DOUBLE, IN_d, G_TYPE_STRING, IN_s, dbus_g_type_get_collection ("GArray", G_TYPE_INT), IN_a, G_TYPE_INVALID);
}
static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
com_example_DBusGlibObject1_simple_method (DBusGProxy *proxy, const gint IN_in_num, gint* OUT_out_num, GError **error)

{
  return dbus_g_proxy_call (proxy, "Simple_method", error, G_TYPE_INT, IN_in_num, G_TYPE_INVALID, G_TYPE_INT, OUT_out_num, G_TYPE_INVALID);
}

typedef void (*com_example_DBusGlibObject1_simple_method_reply) (DBusGProxy *proxy, gint OUT_out_num, GError *error, gpointer userdata);

static void
com_example_DBusGlibObject1_simple_method_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*) user_data;
  GError *error = NULL;
  gint OUT_out_num;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_INT, &OUT_out_num, G_TYPE_INVALID);
  (*(com_example_DBusGlibObject1_simple_method_reply)data->cb) (proxy, OUT_out_num, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
com_example_DBusGlibObject1_simple_method_async (DBusGProxy *proxy, const gint IN_in_num, com_example_DBusGlibObject1_simple_method_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "Simple_method", com_example_DBusGlibObject1_simple_method_async_callback, stuff, g_free, G_TYPE_INT, IN_in_num, G_TYPE_INVALID);
}
static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
com_example_DBusGlibObject1_struct_example_method (DBusGProxy *proxy, const GValueArray* IN_st, gint* OUT_ret, GError **error)

{
  return dbus_g_proxy_call (proxy, "Struct_example_method", error, dbus_g_type_get_struct ("GValueArray", G_TYPE_INT, G_TYPE_STRING, G_TYPE_INVALID), IN_st, G_TYPE_INVALID, G_TYPE_INT, OUT_ret, G_TYPE_INVALID);
}

typedef void (*com_example_DBusGlibObject1_struct_example_method_reply) (DBusGProxy *proxy, gint OUT_ret, GError *error, gpointer userdata);

static void
com_example_DBusGlibObject1_struct_example_method_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*) user_data;
  GError *error = NULL;
  gint OUT_ret;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_INT, &OUT_ret, G_TYPE_INVALID);
  (*(com_example_DBusGlibObject1_struct_example_method_reply)data->cb) (proxy, OUT_ret, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
com_example_DBusGlibObject1_struct_example_method_async (DBusGProxy *proxy, const GValueArray* IN_st, com_example_DBusGlibObject1_struct_example_method_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "Struct_example_method", com_example_DBusGlibObject1_struct_example_method_async_callback, stuff, g_free, dbus_g_type_get_struct ("GValueArray", G_TYPE_INT, G_TYPE_STRING, G_TYPE_INVALID), IN_st, G_TYPE_INVALID);
}
static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
com_example_DBusGlibObject1_variant_send_method (DBusGProxy *proxy, const GValue* IN_in_var, GValue* OUT_out_var, GError **error)

{
  return dbus_g_proxy_call (proxy, "Variant_send_method", error, G_TYPE_VALUE, IN_in_var, G_TYPE_INVALID, G_TYPE_VALUE, OUT_out_var, G_TYPE_INVALID);
}

typedef void (*com_example_DBusGlibObject1_variant_send_method_reply) (DBusGProxy *proxy, GValue OUT_out_var, GError *error, gpointer userdata);

static void
com_example_DBusGlibObject1_variant_send_method_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*) user_data;
  GError *error = NULL;
  GValue OUT_out_var;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_VALUE, &OUT_out_var, G_TYPE_INVALID);
  (*(com_example_DBusGlibObject1_variant_send_method_reply)data->cb) (proxy, OUT_out_var, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
com_example_DBusGlibObject1_variant_send_method_async (DBusGProxy *proxy, const GValue* IN_in_var, com_example_DBusGlibObject1_variant_send_method_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "Variant_send_method", com_example_DBusGlibObject1_variant_send_method_async_callback, stuff, g_free, G_TYPE_VALUE, IN_in_var, G_TYPE_INVALID);
}
static
#ifdef G_HAVE_INLINE
inline
#endif
gboolean
com_example_DBusGlibObject1_dict_send_method (DBusGProxy *proxy, const GHashTable* IN_in_dict, gint* OUT_out_int, GError **error)

{
  return dbus_g_proxy_call (proxy, "Dict_send_method", error, dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_STRING), IN_in_dict, G_TYPE_INVALID, G_TYPE_INT, OUT_out_int, G_TYPE_INVALID);
}

typedef void (*com_example_DBusGlibObject1_dict_send_method_reply) (DBusGProxy *proxy, gint OUT_out_int, GError *error, gpointer userdata);

static void
com_example_DBusGlibObject1_dict_send_method_async_callback (DBusGProxy *proxy, DBusGProxyCall *call, void *user_data)
{
  DBusGAsyncData *data = (DBusGAsyncData*) user_data;
  GError *error = NULL;
  gint OUT_out_int;
  dbus_g_proxy_end_call (proxy, call, &error, G_TYPE_INT, &OUT_out_int, G_TYPE_INVALID);
  (*(com_example_DBusGlibObject1_dict_send_method_reply)data->cb) (proxy, OUT_out_int, error, data->userdata);
  return;
}

static
#ifdef G_HAVE_INLINE
inline
#endif
DBusGProxyCall*
com_example_DBusGlibObject1_dict_send_method_async (DBusGProxy *proxy, const GHashTable* IN_in_dict, com_example_DBusGlibObject1_dict_send_method_reply callback, gpointer userdata)

{
  DBusGAsyncData *stuff;
  stuff = g_new (DBusGAsyncData, 1);
  stuff->cb = G_CALLBACK (callback);
  stuff->userdata = userdata;
  return dbus_g_proxy_begin_call (proxy, "Dict_send_method", com_example_DBusGlibObject1_dict_send_method_async_callback, stuff, g_free, dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_STRING), IN_in_dict, G_TYPE_INVALID);
}
#endif /* defined DBUS_GLIB_CLIENT_WRAPPERS_com_example_DBusGlibObject1 */

G_END_DECLS
