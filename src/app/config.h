#ifndef CONFIG_H
#define CONFIG_H

#include "pdr.h"

class Config
{
public:
    Config();
    static float getTWS(int ert, char nutz);    
};

#endif // CONFIG_H
