/*
 *  TwoLAME: an optimized MPEG Audio Layer Two encoder
 *
 *  Copyright (C) 2001-2004 Michael Cheng
 *  Copyright (C) 2004-2018 The TwoLAME Project
 *  Copyright (C) 2023 IObundle, Lda
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


//#include <stdio.h>
#include "printf.h"
#include "mem.h"

/*******************************************************************************
*
*  Allocate number of bytes of memory equal to "size".
*
*******************************************************************************/

void *twolame_malloc(size_t size, int line, char *file)
{
    void *ptr = (void *) calloc(size, 1);

    if (ptr == NULL) {
        printf("Unable to allocate %d bytes at line %d of %s\n", (int) size, line, file);
    }

    return (ptr);
}



// vim:ts=4:sw=4:nowrap:
