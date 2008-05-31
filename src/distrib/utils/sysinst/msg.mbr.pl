/*	$NetBSD: msg.mbr.pl,v 1.10 2003/10/19 20:17:32 dsl Exp $	*/

/*
 * Copyright 1997 Piermont Information Systems Inc.
 * All rights reserved.
 *
 * Written by Philip A. Nelson for Piermont Information Systems Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed for the NetBSD Project by
 *      Piermont Information Systems Inc.
 * 4. The name of Piermont Information Systems Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PIERMONT INFORMATION SYSTEMS INC. ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PIERMONT INFORMATION SYSTEMS INC. BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* MBR Message catalog -- Polish, i386 version */

message fullpart
{Zainstalujemy teraz NetBSD na dysku %s. Mozesz wybrac, czy chcesz
zainstalowac NetBSD na calym dysku, czy tylko na jego czesci.

Instalacja na czesci dysku, tworzy partycje, lub 'plaster', dla NetBSD
w tablicy partycji MBR twojego dysku. Instalacja na calym dysku jest
`zdecydowanie polecana': zabiera ona caly MBR. Spowoduje to calkowita
utrate danych na dysku. Uniemozliwia ona take pozniejsza instalacje kilku
systemow na tym dysku (chyba, ze nadpiszesz NetBSD i przeinstalujesz uzywajac
tylko czesci dysku).

Ktora instalacje chcesz zrobic?
}

message Select_your_choice
{Wybierz}
message Use_only_part_of_the_disk
{Uzyj tylko czesci dysku}
message Use_the_entire_disk
{Uzyj calego dysku}

/* the %s's will expand into three character strings */
message part_header
{   Calkowity rozmiar dysku %d %s.

.if BOOTSEL
    Pocz(%3s)  Rozm(%3s) Flg Rodzaj                  Wpis menu
   ---------- ---------- --- ----------------------- ---------
.else
    Pocz(%3s)  Rozm(%3s) Flg Rodzaj
   ---------- ---------- --- ------
.endif
}

message part_row_used
{%10d %10d %c%c%c}

message noactivepart
{Nie zaznaczyles aktywnej partycji. Moze to spowodowac, ze twoj system nie
uruchomi sie prawidlowo. Czy partycja NetBSD ma zostac zaznaczona jako aktynwa?}

message setbiosgeom	/* XXX translate */
{
Zostaniesz poproszony o podanie geometrii. 
Please enter the number of sectors per track (maximum 63)
and number of heads (maximum 256) that the BIOS uses to access the disk.
The number of cylinders will be calculated from the disk size.

}

message nobiosgeom
{Sysinst nie mogl automatycznie rozpoznac geometrii dysku z BIOS. 
Fizyczna geometria to %d cylindrow %d sektorow %d glowic\n}

message biosguess
{Uzywajac informacji z dysku, najlepsze parametry geometrii dysku z BIOS to
%d cylindrow %d sektorow %d glowic\n}

message realgeom
{praw. geo: %d cyl, %d glowic, %d sek  (tylko dla porownania)\n}

message biosgeom
{BIOS geom: %d cyl, %d glowic, %d sek\n}

message ovrwrite
{Twoj dysk aktualnie posiada partycje nie-NetBSD. Czy napewno chcesz ja
nadpisac z NetBSD?
}

message Partition_OK
{Partycje OK}

message ptn_type	/* XXX translate */
{    rodzaj: %s}
message ptn_start	/* XXX translate */
{  poczatek: %d %s}
message ptn_size	/* XXX translate */
{   rozmair: %d %s}
message ptn_end	/* XXX translate */
{       end: %d %s}
message ptn_active	/* XXX translate */
{   aktywna: %s}
message ptn_install	/* XXX translate */
{   install: %s}
.if BOOTSEL
message bootmenu	/* XXX translate */
{  bootmenu: %s}
message boot_dflt	/* XXX translate */
{   default: %s}
.endif

message get_ptn_size {%ssize (maximum %d %s)}	/* XXX translate */
message Invalid_numeric {Invalid numeric: }	/* XXX translate */
message Too_large {Too large: }	/* XXX translate */
message Space_allocated {Space allocated: }	/* XXX translate */
message ptn_starts {Space at %d..%d %s (size %d %s)\n}	/* XXX translate */
message get_ptn_start {%s%sStart (in %s)}	/* XXX translate */
message get_ptn_id {Partition kind (0..255)}	/* XXX translate */
message No_free_space {No free space}	/* XXX translate */
message Only_one_extended_ptn {There can only be one extended partition}	/* XXX translate */


message editparttable
{Wyedytuj DOSowa tablice partycji. Tablica partycji wyglada tak:

}

message Partition_table_ok
{Partition table OK}

message Delete_partition
{Delete Partition}	/* XXX translate */
message Dont_change
{Nie zmieniaj}
message Other_kind 
{Other, input number}	/* XXX translate */


message reeditpart	/* XXX translate */
{

Czy chcesz tablice partycji MBR przekonfigurowac (or abandon the installation)?
}

message nobsdpart
{Nie ma partycji NetBSD w tablicy partycji MBR.}

message multbsdpart	/* XXX translate */
{W tablicy partycji MBR znajduje sie kilka partycji NetBSD.
You should set the 'install' flag on the one you want to use.}

message dofdisk
{Konfigurowanie DOSowej tablicy partycji ...
}

message wmbrfail
{Nadpisanie MBR nie powiodlo sie. Nie moge kontynuowac.}

.if 0
.if BOOTSEL
message Set_timeout_value
{Ustaw opoznienie}

message bootseltimeout
{Opoznienie bootmenu: %d\n}

.endif
.endif
