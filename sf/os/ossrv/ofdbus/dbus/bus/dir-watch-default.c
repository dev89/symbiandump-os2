/* -*- mode: C; c-file-style: "gnu" -*- */
/* dir-watch-default.c  OS specific directory change notification for message bus
 *
 * Copyright (C) 2003 Red Hat, Inc.
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

#ifndef __SYMBIAN32__
#include <config.h>
#include <dbus/dbus-internals.h>
#else
#include "config.h"
#include "dbus-internals.h"
#endif //__SYMBIAN32__

#include "dir-watch.h"


/* NoOp */

void 
bus_drop_all_directory_watches (void)
{
}

void
bus_watch_directory (const char *dir, BusContext *context)
{
}
