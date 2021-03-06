/* -*- mode: C; c-file-style: "gnu" -*- */
/* dbus-uuidgen.h  The guts of the dbus-uuidgen binary live in libdbus, in this file.
 *
 * Copyright (C) 2006  Red Hat, Inc.
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
#ifdef DBUS_INSIDE_DBUS_H
#error "You can't include dbus-uuidgen.h in the public header dbus.h"
#endif

#ifndef DBUS_UUIDGEN_H
#define DBUS_UUIDGEN_H

#include <dbus/dbus-types.h>
#include <dbus/dbus-errors.h>

DBUS_BEGIN_DECLS

#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t dbus_internal_do_not_use_get_uuid    (const char *filename,
                                                  char      **uuid_p,
                                                  dbus_bool_t create_if_not_found,
                                                  DBusError  *error);
dbus_bool_t dbus_internal_do_not_use_ensure_uuid (const char *filename,
                                                  char      **uuid_p,
                                                  DBusError  *error);
#ifdef __SYMBIAN32__
IMPORT_C
#endif
dbus_bool_t dbus_internal_do_not_use_create_uuid (char      **uuid_p);


DBUS_END_DECLS

#endif /* DBUS_UUIDGEN_H */
