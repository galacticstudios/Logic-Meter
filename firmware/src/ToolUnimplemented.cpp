/* 
 * File:   ToolUnimplemented.cpp
 * Author: Bob
 * 
 * Created on January 10, 2021, 11:00 AM
 */

#include "ToolUnimplemented.h"
#include "Help.h"
#include "Menu.h"
#include "UnimplementedPane.h"

static const Help help(NULL, NULL, NULL, 
        "Unimplemented. What would be a good feature to add here?");

static const MenuItem menuItems[5] = {
    MenuItem(),
    MenuItem(),
    MenuItem(),
    MenuItem(),
    MenuItem()};

const Menu menu(menuItems);

ToolUnimplemented::ToolUnimplemented() :
    Tool("Unimplemented", new UnimplementedPane(), menu, help)
{
}

ToolUnimplemented::~ToolUnimplemented() 
{
}

