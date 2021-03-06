/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-server-socket.h Server implementation for sockets
 *
 * Copyright (C) 2002, 2006  Red Hat Inc.
 * Portion Copyright � 2008 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 * Licensed under the Academic Free License version 2.1
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef DBUS_SERVER_SOCKET_H
#define DBUS_SERVER_SOCKET_H

#ifdef __SYMBIAN32__
//these are not system headers in build time
#include "dbus-internals.h"
#include "dbus-server-protected.h"
#else
#include <dbus/dbus-internals.h>
#include <dbus/dbus-server-protected.h>
#endif

DBUS_BEGIN_DECLS

DBusServer* _dbus_server_new_for_socket           (int               fd,
                                                   const DBusString *address);
DBusServer* _dbus_server_new_for_tcp_socket       (const char       *host,
                                                   dbus_uint32_t     port,
                                                   DBusError        *error);
DBusServerListenResult _dbus_server_listen_socket (DBusAddressEntry  *entry,
                                                   DBusServer       **server_p,
                                                   DBusError         *error);


void _dbus_server_socket_own_filename (DBusServer *server,
                                       char       *filename);

DBUS_END_DECLS

#endif /* DBUS_SERVER_SOCKET_H */
