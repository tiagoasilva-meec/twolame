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

#include "twolame.h"
#include "common.h"
#include "availbits.h"


/* function returns the number of available bits */
int twolame_available_bits(twolame_options * glopts)
{
    frame_header *header = &glopts->header;
    FLOAT average;
    FLOAT frac;
    int whole;
    int adb;

    average = (1152.0 / ((FLOAT) glopts->samplerate_out / 1000.0))
              * ((FLOAT) glopts->bitrate / 8.0);

    // printf("availbits says: sampling freq is %i. version %i. bitrateindex %i slots
    // %f\n",header->sampling_frequency, header->version, header->bitrate_index, average);

    whole = (int) average;
    frac = average - (FLOAT) whole;

    /* never allow padding for a VBR frame. Don't ask me why, I've forgotten why I set this */
    if (frac != 0 && glopts->padding && glopts->vbr == FALSE) {
        if (glopts->slots_lag > (frac - 1.0)) {   /* no padding for this frame */
            glopts->slots_lag -= frac;
            header->padding = 0;
        } else {                /* padding */
            header->padding = 1;
            glopts->slots_lag += (1 - frac);
        }
    }

    adb = whole * 8;

    return adb;
}


// vim:ts=4:sw=4:nowrap:
