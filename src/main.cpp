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

#include <QApplication>
#include <QDebug>
#include <QLabel>
#include "mainwindow.h"

void writeBagrovTable(
    float bag_min = 0.1F,
    float bag_max = 1.0F,
    float bag_step = 0.1F,
    float x_min = 0.1F,
    float x_max = 15.1F,
    float x_step = 0.1F
);

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "--write-bagrov-table") == 0) {
        writeBagrovTable();
        return 0;
    }

    QApplication app(argc, argv);
    MainWindow mainWin(&app);
    mainWin.show();
    return app.exec();
}

QTextStream& qStdOut()
{
    static QTextStream ts( stdout );
    return ts;
}

void writeBagrovTable(float bag_min, float bag_max, float bag_step,
                      float x_min, float x_max, float x_step)
{
    qStdOut() << "# Writing a table of bagrov values to stdout...\n";
    qStdOut() << "bag,x,y\n";

    Bagrov bagrov;

    float y = 0.0;
    float bag = bag_min;

    while(bag <= bag_max) {

        float x = x_min;

        while(x <= x_max) {
            float xtmp = x;
            bagrov.nbagro(&bag, &y, &xtmp);
            qStdOut() << bag << "," << x << "," << y << "\n";
            x += x_step;
        }

        bag += bag_step;
    }
}
