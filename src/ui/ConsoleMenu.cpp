#include "ConsoleMenu.h"

#include "ConsoleScreen.h"

#include <iostream>

void ConsoleMenu::render(const std::vector<std::string>& items)
{
	ConsoleScreen::header();
	for (size_t index = 0; index < items.size(); ++index)
		std::cout << '[' << index + 1 << "] " << items[index] << std::endl;
}

void ConsoleMenu::render(const std::string& location, const UserRecord& user, const std::vector<std::string>& items)
{
	ConsoleScreen::header(location, &user);
	for (size_t index = 0; index < items.size(); ++index)
		std::cout << '[' << index + 1 << "] " << items[index] << std::endl;
}
