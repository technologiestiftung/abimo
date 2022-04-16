#ifndef CONFIG_H
#define CONFIG_H

#include "pdr.h"

class Config
{
public:
    Config();
    static float getTWS(int ert, char nutz);
    static void set_NUT_ERT_BER(PDR &pdr, char nut, int ert = 0, int ber = 0);
};

#endif // CONFIG_H
