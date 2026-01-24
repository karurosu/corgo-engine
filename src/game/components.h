//
//  game/components.h
//  List of game ECS components
//  Copyright (c) 2025 Carlos Camacho. All rights reserved.
//

#ifndef CORGO_GAME_COMPONENTS_H
#define CORGO_GAME_COMPONENTS_H

#include "../ecs/core/component.h"

/**
 * How to define a component:
 * 
 * 1. Define the component data struct, this can be any C struct, for example:
 *    
 *      typedef struct CE_MyComponent {
 *            // Component data here
 *      } CE_MyComponent;
 * 
 *  2. Add the component to the component description macro below:
 *  
 *    #define CE_COMPONENT_DESC_GAME(X) \
 *       X(CE_MY_COMPONENT, <UUID>, CE_MyComponent, <Initial Capacity>)
 * 
 *  UUID must be larger than 100 to avoid conflicts with core and engine components.
 *  See docs for details on UUID and Initial Capacity.
 * 
 *  3. Implement the component initialization and cleanup functions in a .c file:
 *  
 *      CE_DEFINE_COMPONENT_INIT(CE_MY_COMPONENT)
 *      {
 *          // Initialization logic here
 *          return CE_OK;
 *      }
 * 
 *      CE_DEFINE_COMPONENT_CLEANUP(CE_MY_COMPONENT)
 *      {
 *          // Cleanup logic here
 *          return CE_OK;
 *      }
 * 
 *  4. Generate the component implementation using the following macro in the c file (one per component):
 *     
 *      CE_GENERATE_COMPONENT_IMP(CE_MY_COMPONENT)
 * 
 *  Tips:
 *  - Don't forget the backslash at the end of the line when adding new components.
 */

#define CE_COMPONENT_DESC_GAME(X) \
/**
    X(CE_MY_COMPONENT, <UUID>, CE_MyComponent, <Initial Capacity>) \

**/


#endif // CORGO_GAME_COMPONENTS_H