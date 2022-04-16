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

#include "pdr.h"
PDR::PDR():
    wIndex(0),
    nFK(0),
    FLW(0),
    NUT(0),
    R(0),
    ROW(0),
    RI(0),
    VER(0),
    ERT(0),
    BER(0),
    P1(0.0F),
    ETP(0),
    KR(0),
    P1S(0.0F),
    ETPS(0)
{}

void PDR::setUsageYieldIrrigation(char usage, int yield, int irrigation)
{
    this->NUT = usage;
    this->ERT = yield;
    this->BER = irrigation;
}
