/*	$NetBSD: toc.h,v 1.1.1.3 2004/07/12 23:26:48 wiz Exp $	*/

/* toc.h -- table of contents handling.
   Id: toc.h,v 1.3 2004/02/13 22:01:39 dirt Exp

   Copyright (C) 1999 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   Written by Karl Heinz Marbaise <kama@hippo.fido.de>.  */

#ifndef TOC_H
#define TOC_H

/* Structure to hold one entry for the toc. */
typedef struct toc_entry_elt {
  char *name;
  char *containing_node; /* Name of node containing this section.  */
  char *html_file;       /* Name of HTML node-file in split-HTML mode */
  int number;            /* counting number from 0...n independent from
                            chapter/section can be used for anchors or
                            references to it.  */
  int level;             /* level: chapter, section, subsection... */
} TOC_ENTRY_ELT;

/* all routines which have relationship with TOC should start with
   toc_ (this is a kind of name-space) */
extern int toc_add_entry (char *tocname, int level,
    char *node_name, char *anchor); /* return the number for the toc-entry */
extern void toc_free (void);
extern char *toc_find_section_of_node (char *node);

extern void cm_contents (int arg);

#endif /* not TOC_H */
