#include "ConsoleMenu.h"

#include "ConsoleScreen.h"

#include <iostream>

void ConsoleMenu::render(const std::vector<std::string>& items)
{
	ConsoleScreen::header();
	for (size_t index = 0; index < items.size(); ++index)
		std::cout << '[' << index + 1 << "] " << items[index] << std::endl;
}
