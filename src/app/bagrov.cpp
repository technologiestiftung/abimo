/***************************************************************************
 *   Copyright (C) 2009 by Meiko Rachimow, Claus Rachimow                  *
 *   This file is part of Abimo 3.2                                        *
 *   Abimo 3.2 is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QDebug>
#include <math.h>

#include "bagrov.h"

#define UPPER_LIMIT_Y0 0.99999F

/*
 =======================================================================================================================
    Translated by FOR_C, v2.3.2, on 10/16/94 at 18:28:43 ;
    FOR_C Options SET: none ;
    include <stdio.h> ;
    #include <f_rt.h> ;
    NBAGRO & BAGROV - Module zur Lesung der BAGROV-Gleichung AUFRUF: CALL NBAGRO(BAG,y,x) CALL BAGROV(BAG,x,y)
    PROGRAMMTYP: SUBROUTINE, SPRACHE: FORTRAN80 EINGABEPARAMETER: BAG- Bagrov-Parameter mit n=BAG x - x=P/ETP mit
    P=PMD*niedKorrFaktor [mm/a] ETP::Mittlere potentielle Verdunstung [mm/a] AUSGABEPARAMETER: y - ETR/ETP ETR::
    Mittlere reale Verdunstung [mm/a]
 =======================================================================================================================
 */

Bagrov::Bagrov()
{
}

void Bagrov::nbagro(float *bage, float *y, float *x)
{
    int _do0, i, i_, ia, ie, j;
    float bag, bag_plus_one;
    float a, a0, a1, a2, b, c, epa, eyn, h, h13, h23, reciprocal_bag_plus_one, s1, s2, w, one_third, two_thirds, y0;
    static float aa[16] =
    {
        0.9946811499F,
        1.213648255F,
        -1.350801214F,
        11.80883489F,
        -21.53832235F,
        19.3775197F,
        0.862954876F,
        9.184851852F,
        -147.2049991F,
        1291.164889F,
        -6357.554955F,
        19022.42165F,
        -35235.40521F,
        39509.02815F,
        -24573.23867F,
        6515.556685F
    };

    y0 = 0.0F;

    // Set local bag value to input value bage, but to 20.0 at maximum
    bag = ((*bage > 20.0) ? 20.0 : *bage);

    // If input value x is already below a threshold, return
    if (*x < 0.0005F) goto FINISH;

    // Set input value x to 15.0 at maximum
    if (*x > 15.0F) *x = 15.0F;

    // Calculate expressions that are used more than once
    bag_plus_one = bag + 1.0F;
    reciprocal_bag_plus_one = (float) (1.0 / bag_plus_one);
    one_third = 1.0F / 3.0F;
    two_thirds = 2.0F / 3.0F;

    h13 = (float) exp(-bag_plus_one * 1.09861);
    h23 = (float) exp(-bag_plus_one * 0.405465);

    // KOEFFIZIENTEN DER BEDINGUNGSGLEICHUNG
    a2 = -13.5F * reciprocal_bag_plus_one * (1.0F + 3.0F * (h13 - h23));
    a1 = 9.0F * reciprocal_bag_plus_one * (h13 + h13 - h23) - two_thirds * a2;
    a0 = 1.0F / (1.0F - reciprocal_bag_plus_one - 0.5F * a1 - one_third * a2);

    // Multiply each of a1, a2 with a0
    a1 *= a0;
    a2 *= a0;

    // KOEFFIZIENTEN DES LOESUNSANSATZES
    b = (bag >= 0.49999F) ?
        (- (float) sqrt(0.25 * a1 * a1 - a2) + 0.5F * a1) :
        (- (float) sqrt(0.5F * a1 * a1 - a2));

    c = a1 - b;
    a = a0 / (b - c);

    epa = (float) exp(*x / a);
    w = b - c * epa;

    // NULLTE NAEHERUNGSLOESUNG (1. Naeherungsloesung)
    y0 = (epa - 1.0F) / w;

    // Limit y0 to its maximum allowed value
    if (y0 > UPPER_LIMIT_Y0) y0 = UPPER_LIMIT_Y0;

    // If bag is between a certain range we have finished
    if (bag >= 0.7F && bag <= 3.8F) goto FINISH;

    j = 0;
    if (bag >= 3.8F) goto L_12;

    /* NUMERISCHE INTEGRATION FUER BAG<0.7 (2.Naeherungsloesung) */
    for (j = 1; j <= 30; j++)
    {
        eyn = (float) exp(bag * log(y0));
        if (eyn > 0.9F) goto FINISH;
        if (eyn >= 0.7F && bag > 4.0F) goto FINISH;
        ia = 2;
        ie = 6;
        if (eyn > 0.7F) ia = 8;
        if (eyn > 0.7F) ie = 16;
        s1 = 0.0F;
        s2 = 0.0F;
        h = 1.0F;
        for (i = ia, _do0 = ie; i <= _do0; i++)
        {
            i_ = i - 1;
            h = h * eyn;
            w = aa[i_] * h;
            j = i - ia + 1; /* cls J=I-IA+1 */
            s2 = s2 + w / (j * (float) bag + 1.0F);
            s1 = s1 + w;
        }

        ia = ia - 1;
        h = aa[ia - 1];
        h = (*x - y0 * s2 - y0 * h) / (h + s1);
        y0 = y0 + h;
        if (fabs(h) / y0 < 0.007F) goto L_15;
    }

L_15:
    ;
    if (y0 > 0.9) {
        bagrov(&bag, x, &y0);
    }
    else {
      //qDebug() << "y0 <= 0.9 -> not calling bagrov()";
    }
    goto FINISH;

    /* NUMERISCHE INTEGRATION FUER BAG>3.8 (3. Naeherungsloesung) */
L_12:
    ;
    j = j + 1;
    if (j > 15) goto FINISH;
    if (y0 > 0.999) y0 = 0.999F;
    epa = (float) exp(bag * log(y0));
    h = 1.0f - epa;
    if (h < 1.0e-07) h = 1.0e-07F;
    if (h > 0.99999) h = 0.99999F;
    s1 = h - bag * epa / (float) log(h);
    h = h * (y0 + epa * y0 / s1 -*x);
    y0 = y0 - h;
    if (fabs(h) > 0.001) goto L_12;
FINISH:
    // Set result value y to y0 or 1.0 at maximum
    *y = ((y0 > 1.0) ? 1.0F : y0);
}

/*
 =======================================================================================================================
    FIXME:
 =======================================================================================================================
 */
void Bagrov::bagrov(float *bagf, float *x0, float *y0)
{
    bool doloop; /* LOGICAL16 */
    int _do0, i, ii, j;

    qDebug() << "In bagrov()...";

    /* meiko : initialisiere i (einzige Aenderung) */
    i = 0;

    float	delta, du, h, s, s1, sg, si, su, u, x;

    if (*x0 == 0.0) goto L_10;
    *y0 = 0.99F;
    goto L_20;
L_10:
    *y0 = 0.0F;
    return;
L_20:
    doloop = false;

    /* NUMERISCHE INTEGRATION DER BAGROVBEZIEHUNG */
L_21:
    j = 1;
    du = 2.0F **y0;
    h = 1.0F + 1.0F / (1.0F - (float) exp(*bagf * log(*y0)));
    si = h * du / 4.0F;
    sg = 0.0F;
    su = 0.0F;
L_1:
    s = si;
    j = j * 2;
    du = du / 2.0F;
    u = du / 2.0F;
    sg = sg + su;
    su = 0.0F;

    for (ii = 1, _do0 = j; ii <= _do0; ii += 2)
    {
        su = su + 1.0F / (1.0F - (float) exp(*bagf * log(u)));
        u = u + du;
    }

    si = (2.0F * sg + 4.0F * su + h) * du / 6.0F;
    s1 = 0.001F * s;
    if (fabs(s - si) > s1) goto L_1;
    x = si;

    /* ENDE DER NUMERISCHEN INTEGRATION */
    if (doloop) goto L_42;
    if (*x0 > x) goto L_30;
    *y0 = 0.5F;
    goto L_40;
L_30:
    *y0 = 1.0F;
    return;
L_40:
    i = 1;

    /* SCHLEIFE I=1(1)10 ZUR BERECHNUNG VON DELTA */
L_41:
    doloop = true;
    goto L_21;
L_42:
    delta = (*x0 - x) * (1.0F - (float) exp(*bagf * (float) log(*y0)));
    *y0 = *y0 + delta;
    if (*y0 >= 1.0) goto L_50;
    if (*y0 <= 0.0) goto L_60;
    goto L_70;
L_50:
    *y0 = 0.99F;
    goto L_90;
L_60:
    *y0 = 0.01F;
    goto L_90;
L_70:
    if (fabs(delta) < 0.01F) goto L_80;
    goto L_90;
L_80:
    ;
    return;
L_90:
    if (i < 10) goto L_91;
    goto L_92;
L_91:
    i = i + 1;
    goto L_41;
L_92:
    ;
    return;
}	/* end of function */
