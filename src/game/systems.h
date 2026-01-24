//
//  game/systems.h
//  List of game ECS systems
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_GAME_SYSTEMS_H
#define CORGO_GAME_SYSTEMS_H

#include "ecs/core/system.h"

/**
 * How to define a system:
 * 
 * 1. Declare dependencies using a macro like this (Don't forget the backslash at the end):
 *      #define CE_MY_SYSTEM_NAME_DEPENDENCIES \
 *          REQUIRE_COMPONENT(CE_TEXT_LABEL_COMPONENT, textLabelComponent)\
 *          REQUIRE_RELATIONSHIP(CE_RELATIONSHIP_PARENT, parentEntity)
 * 
 * 2. Add the system to the system description macro below:
 *    
 *      #define CE_SYSTEM_DESC_GAME(X) \
 *          X(CE_MY_SYSTEM_NAME, <Run Order>, <Run Phase>, <Run Frequency>, CE_MY_SYSTEM_NAME_DEPENDENCIES)
 * 
 *    See docs for details on Run Order, Phase, and Frequency.
 * 
 * 3. Implement the system in a .c file using the CE_START_SYSTEM_IMPLEMENTATION and CE_END_SYSTEM_IMPLEMENTATION macros:
 * 
 *      CE_START_SYSTEM_IMPLEMENTATION(CE_MY_SYSTEM_NAME, CE_MY_SYSTEM_NAME_DEPENDENCIES)
 *      {
 *         // System logic here
 *      }
 *      CE_END_SYSTEM_IMPLEMENTATION
 */

#define CE_SYSTEM_DESC_GAME(X) \
    // X(CE_MY_SYSTEM_NAME, <Run Order>, <Run Phase>, <Run Frequency>, CE_MY_SYSTEM_NAME_DEPENDENCIES) \



#endif // CORGO_GAME_SYSTEMS_H
