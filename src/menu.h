#ifndef MENU_H
#define MENU_H

#include "stm32f4xx.h"
#include <stdint.h>

void display_menu();
void process_menu();

typedef enum
{
    MENU_OFF = 0,
    MENU_ALL_ON,
    MENU_ON,
    MENU_RANDOM,
    MENU_EQ,
    MENU_WALKING_ONES,
    MENU_WALKING_ZEROS,
    MENU_NUM_ITEMS,
} Menu_Commands_T;

 
#endif /* MENU_H */


