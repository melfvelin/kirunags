/*  tmtc.h
*
*
*/

#ifndef TMTC_H
#define TMTC_H

#include <iostream>
#include <stdint.h>
#include "tables.h"

namespace tmtc
{
    int encapsulate(const char *data);
    int decapsulate(const char *frame);

}





#endif /* TABLES_H */
