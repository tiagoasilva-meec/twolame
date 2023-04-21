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
#include <string.h>
#include <math.h>

#include "twolame.h"
#include "common.h"
#include "mem.h"
#include "bitbuffer.h"
#include "enwindow.h"
#include "subband.h"

static double tabcos_create_dct_matrix[] = {1000000000.000000, 998795456.205172, 995184726.672197, 989176509.964781, 980785280.403231, 970031253.194544, 956940335.732209, 941544065.183021, 923879532.511287, 903989293.123444, 881921264.348355, 857728610.000272, 831469612.302546, 803207531.480645, 773010453.362737, 740951125.354960, 707106781.186548, 671558954.847019, 634393284.163646, 595699304.492434, 555570233.019603, 514102744.193223, 471396736.825999, 427555093.430283, 382683432.365091, 336889853.392221, 290284677.254464, 242980179.903265, 195090322.016130, 146730474.455363, 98017140.329562, 49067674.327420, 1000000000.000000, 989176509.964781, 956940335.732209, 903989293.123444, 831469612.302546, 740951125.354960, 634393284.163646, 514102744.193223, 382683432.365091, 242980179.903265, 98017140.329562, -49067674.327416, -195090322.016126, -336889853.392218, -471396736.825996, -595699304.492432, -707106781.186546, -803207531.480644, -881921264.348354, -941544065.183020, -980785280.403230, -998795456.205172, -995184726.672197, -970031253.194545, -923879532.511288, -857728610.000274, -773010453.362739, -671558954.847021, -555570233.019606, -427555093.430286, -290284677.254467, -146730474.455367, 1000000000.000000, 970031253.194544, 881921264.348355, 740951125.354960, 555570233.019603, 336889853.392221, 98017140.329562, -146730474.455360, -382683432.365088, -595699304.492432, -773010453.362735, -903989293.123442, -980785280.403230, -998795456.205173, -956940335.732210, -857728610.000274, -707106781.186550, -514102744.193226, -290284677.254467, -49067674.327422, 195090322.016123, 427555093.430277, 634393284.163641, 803207531.480641, 923879532.511284, 989176509.964780, 995184726.672198, 941544065.183023, 831469612.302549, 671558954.847024, 471396736.826004, 242980179.903271, 1000000000.000000, 941544065.183021, 773010453.362737, 514102744.193223, 195090322.016130, -146730474.455360, -471396736.825996, -740951125.354957, -923879532.511286, -998795456.205172, -956940335.732210, -803207531.480647, -555570233.019606, -242980179.903268, 98017140.329556, 427555093.430277, 707106781.186544, 903989293.123441, 995184726.672196, 970031253.194546, 831469612.302549, 595699304.492439, 290284677.254470, -49067674.327410, -382683432.365082, -671558954.847012, -881921264.348351, -989176509.964780, -980785280.403232, -857728610.000278, -634393284.163654, -336889853.392231, 1000000000.000000, 903989293.123444, 634393284.163646, 242980179.903265, -195090322.016126, -595699304.492432, -881921264.348354, -998795456.205172, -923879532.511288, -671558954.847021, -290284677.254467, 146730474.455357, 555570233.019597, 857728610.000269, 995184726.672196, 941544065.183023, 707106781.186553, 336889853.392228, -98017140.329553, -514102744.193214, -831469612.302540, -989176509.964780, -956940335.732212, -740951125.354966, -382683432.365100, 49067674.327407, 471396736.825988, 803207531.480638, 980785280.403228, 970031253.194547, 773010453.362746, 427555093.430294, 1000000000.000000, 857728610.000272, 471396736.825999, -49067674.327416, -555570233.019601, -903989293.123442, -995184726.672197, -803207531.480647, -382683432.365094, 146730474.455357, 634393284.163641, 941544065.183019, 980785280.403232, 740951125.354964, 290284677.254470, -242980179.903256, -707106781.186542, -970031253.194542, -956940335.732212, -671558954.847026, -195090322.016138, 336889853.392209, 773010453.362729, 989176509.964779, 923879532.511292, 595699304.492445, 98017140.329575, -427555093.430269, -831469612.302536, -998795456.205172, -881921264.348363, -514102744.193236, 1000000000.000000, 803207531.480645, 290284677.254464, -336889853.392218, -831469612.302544, -998795456.205173, -773010453.362739, -242980179.903268, 382683432.365085, 857728610.000269, 995184726.672198, 740951125.354964, 195090322.016136, -427555093.430274, -881921264.348351, -989176509.964782, -707106781.186555, -146730474.455372, 471396736.825988, 903989293.123438, 980785280.403233, 671558954.847029, 98017140.329575, -514102744.193209, -923879532.511281, -970031253.194548, -634393284.163659, -49067674.327436, 555570233.019587, 941544065.183014, 956940335.732214, 595699304.492449, 1000000000.000000, 740951125.354960, 98017140.329562, -595699304.492432, -980785280.403230, -857728610.000274, -290284677.254467, 427555093.430277, 923879532.511284, 941544065.183023, 471396736.826004, -242980179.903256, -831469612.302540, -989176509.964782, -634393284.163654, 49067674.327407, 707106781.186539, 998795456.205172, 773010453.362746, 146730474.455376, -555570233.019590, -970031253.194540, -881921264.348363, -336889853.392236, 382683432.365073, 903989293.123435, 956940335.732214, 514102744.193239, -195090322.016108, -803207531.480632, -995184726.672199, -671558954.847036, 1000000000.000000, 671558954.847019, -98017140.329559, -803207531.480644, -980785280.403231, -514102744.193226, 290284677.254458, 903989293.123441, 923879532.511289, 336889853.392228, -471396736.825990, -970031253.194542, -831469612.302551, -146730474.455372, 634393284.163636, 998795456.205172, 707106781.186557, -49067674.327403, -773010453.362727, -989176509.964783, -555570233.019617, 242980179.903245, 881921264.348345, 941544065.183028, 382683432.365107, -427555093.430264, -956940335.732203, -857728610.000284, -195090322.016151, 595699304.492414, 995184726.672194, 740951125.354977, 1000000000.000000, 595699304.492434, -290284677.254461, -941544065.183020, -831469612.302547, -49067674.327422, 773010453.362733, 970031253.194546, 382683432.365097, -514102744.193214, -995184726.672196, -671558954.847026, 195090322.016117, 903989293.123438, 881921264.348361, 146730474.455376, -707106781.186536, -989176509.964783, -471396736.826014, 427555093.430264, 980785280.403227, 740951125.354972, -98017140.329540, -857728610.000261, -923879532.511296, -242980179.903288, 634393284.163625, 998795456.205174, 555570233.019624, -336889853.392195, -956940335.732201, -803207531.480663, 1000000000.000000, 514102744.193223, -471396736.825996, -998795456.205172, -555570233.019606, 427555093.430277, 995184726.672196, 595699304.492439, -382683432.365082, -989176509.964780, -634393284.163654, 336889853.392209, 980785280.403228, 671558954.847029, -290284677.254449, -970031253.194540, -707106781.186560, 242980179.903245, 956940335.732204, 740951125.354972, -195090322.016108, -941544065.183013, -773010453.362753, 146730474.455339, 923879532.511277, 803207531.480661, -98017140.329533, -903989293.123431, -831469612.302561, 49067674.327388, 881921264.348340, 857728610.000289, 1000000000.000000, 427555093.430283, -634393284.163644, -970031253.194545, -195090322.016133, 803207531.480641, 881921264.348359, -49067674.327410, -923879532.511283, -740951125.354966, 290284677.254451, 989176509.964779, 555570233.019614, -514102744.193209, -995184726.672199, -336889853.392236, 707106781.186534, 941544065.183028, 98017140.329581, -857728610.000261, -831469612.302559, 146730474.455339, 956940335.732202, 671558954.847038, -382683432.365063, -998795456.205171, -471396736.826024, 595699304.492408, 980785280.403237, 242980179.903295, -773010453.362715, -903989293.123459, 1000000000.000000, 336889853.392221, -773010453.362735, -857728610.000274, 195090322.016123, 989176509.964780, 471396736.826004, -671558954.847012, -923879532.511291, 49067674.327407, 956940335.732205, 595699304.492445, -555570233.019590, -970031253.194548, -98017140.329577, 903989293.123435, 707106781.186563, -427555093.430264, -995184726.672199, -242980179.903288, 831469612.302532, 803207531.480661, -290284677.254435, -998795456.205171, -382683432.365117, 740951125.354937, 881921264.348370, -146730474.455328, -980785280.403224, -514102744.193255, 634393284.163617, 941544065.183033, 1000000000.000000, 242980179.903265, -881921264.348354, -671558954.847021, 555570233.019597, 941544065.183023, -98017140.329553, -989176509.964780, -382683432.365100, 803207531.480638, 773010453.362746, -427555093.430269, -980785280.403233, -49067674.327436, 956940335.732204, 514102744.193239, -707106781.186531, -857728610.000284, 290284677.254438, 998795456.205174, 195090322.016155, -903989293.123431, -634393284.163668, 595699304.492408, 923879532.511299, -146730474.455328, -995184726.672193, -336889853.392257, 831469612.302526, 740951125.354984, -471396736.825962, -970031253.194555, 1000000000.000000, 146730474.455363, -956940335.732208, -427555093.430286, 831469612.302542, 671558954.847024, -634393284.163639, -857728610.000278, 382683432.365078, 970031253.194547, -98017140.329546, -998795456.205172, -195090322.016144, 941544065.183014, 471396736.826017, -803207531.480632, -707106781.186565, 595699304.492414, 881921264.348368, -336889853.392195, -980785280.403236, 49067674.327388, 995184726.672194, 242980179.903295, -923879532.511275, -514102744.193255, 773010453.362713, 740951125.354984, -555570233.019565, -903989293.123462, 290284677.254421, 989176509.964787, 1000000000.000000, 49067674.327420, -995184726.672197, -146730474.455367, 980785280.403229, 242980179.903271, -956940335.732206, -336889853.392231, 923879532.511282, 427555093.430294, -881921264.348348, -514102744.193236, 831469612.302534, 595699304.492449, -773010453.362722, -671558954.847036, 707106781.186529, 740951125.354977, -634393284.163625, -803207531.480663, 555570233.019577, 857728610.000289, -471396736.825968, -903989293.123459, 382683432.365053, 941544065.183033, -290284677.254424, -970031253.194555, 195090322.016082, 989176509.964787, -98017140.329514, -998795456.205175
};


static void create_dct_matrix(FLOAT filter[16][32])
{
    register int i, k;
    register int aux = 0;

#ifdef FLOAT_DOUBLE
    for (i = 0; i < 16; i++)
        for (k = 0; k < 32; k++) {
            //if ((filter[i][k] = 1e9 * cos((FLOAT) ((2 * i + 1) * k * PI64))) >= 0)
            if ((filter[i][k] = tabcos_create_dct_matrix[aux++]) >= 0)
                modf(filter[i][k] + 0.5, &filter[i][k]);
            else
                modf(filter[i][k] - 0.5, &filter[i][k]);
            filter[i][k] *= 1e-9;
        }
#else
    for (i = 0; i < 16; i++)
        for (k = 0; k < 32; k++) {
            //if ((filter[i][k] = 1e9 * cos((FLOAT) ((2 * i + 1) * k * PI64))) >= 0)
            if ((filter[i][k] = tabcos_create_dct_matrix[aux++]) >= 0)
                modff(filter[i][k] + 0.5, &filter[i][k]);
            else
                modff(filter[i][k] - 0.5, &filter[i][k]);
            filter[i][k] *= 1e-9;
        }
#endif
}


int twolame_init_subband(subband_mem * smem)
{
    memset(smem, 0, sizeof(subband_mem));
    create_dct_matrix(smem->m);

    return 0;
}


void twolame_window_filter_subband(subband_mem * smem, short *pBuffer, int ch, FLOAT s[SBLIMIT])
{
    register int i, j;
    int pa, pb, pc, pd, pe, pf, pg, ph;
    FLOAT t;
    FLOAT *dp, *dp2;
    const FLOAT *pEnw;
    FLOAT y[64];
    FLOAT yprime[32];

    dp = smem->x[ch] + smem->off[ch] + smem->half[ch] * 256;

    /* replace 32 oldest samples with 32 new samples */
    for (i = 0; i < 32; i++)
        dp[(31 - i) * 8] = (FLOAT) pBuffer[i] / SCALE;

    // looks like "school example" but does faster ...
    dp = (smem->x[ch] + smem->half[ch] * 256);
    pa = smem->off[ch];
    pb = (pa + 1) % 8;
    pc = (pa + 2) % 8;
    pd = (pa + 3) % 8;
    pe = (pa + 4) % 8;
    pf = (pa + 5) % 8;
    pg = (pa + 6) % 8;
    ph = (pa + 7) % 8;

    for (i = 0; i < 32; i++) {
        dp2 = dp + i * 8;
        pEnw = enwindow + i;
        t = dp2[pa] * pEnw[0];
        t += dp2[pb] * pEnw[64];
        t += dp2[pc] * pEnw[128];
        t += dp2[pd] * pEnw[192];
        t += dp2[pe] * pEnw[256];
        t += dp2[pf] * pEnw[320];
        t += dp2[pg] * pEnw[384];
        t += dp2[ph] * pEnw[448];
        y[i] = t;
    }

    yprime[0] = y[16];          // Michael Chen's dct filter

    dp = smem->half[ch] ? smem->x[ch] : (smem->x[ch] + 256);
    pa = smem->half[ch] ? (smem->off[ch] + 1) & 7 : smem->off[ch];
    pb = (pa + 1) % 8;
    pc = (pa + 2) % 8;
    pd = (pa + 3) % 8;
    pe = (pa + 4) % 8;
    pf = (pa + 5) % 8;
    pg = (pa + 6) % 8;
    ph = (pa + 7) % 8;

    for (i = 0; i < 32; i++) {
        dp2 = dp + i * 8;
        pEnw = enwindow + i + 32;
        t = dp2[pa] * pEnw[0];
        t += dp2[pb] * pEnw[64];
        t += dp2[pc] * pEnw[128];
        t += dp2[pd] * pEnw[192];
        t += dp2[pe] * pEnw[256];
        t += dp2[pf] * pEnw[320];
        t += dp2[pg] * pEnw[384];
        t += dp2[ph] * pEnw[448];
        y[i + 32] = t;
        // 1st pass on Michael Chen's dct filter
        if (i > 0 && i < 17)
            yprime[i] = y[i + 16] + y[16 - i];
    }

    // 2nd pass on Michael Chen's dct filter
    for (i = 17; i < 32; i++)
        yprime[i] = y[i + 16] - y[80 - i];

    for (i = 15; i >= 0; i--) {
        register FLOAT s0 = 0.0, s1 = 0.0;
        register FLOAT *mp = smem->m[i];
        register FLOAT *xinp = yprime;
        for (j = 0; j < 8; j++) {
            s0 += *mp++ * *xinp++;
            s1 += *mp++ * *xinp++;
            s0 += *mp++ * *xinp++;
            s1 += *mp++ * *xinp++;
        }
        s[i] = s0 + s1;
        s[31 - i] = s0 - s1;
    }

    smem->half[ch] = (smem->half[ch] + 1) & 1;

    if (smem->half[ch] == 1)
        smem->off[ch] = (smem->off[ch] + 7) & 7;
}


// vim:ts=4:sw=4:nowrap:
