#include "channel.hpp"

/**
 * Determine if a string contains a valid channel name. A channel must start
 * with either a '#' or '&' sign, and can't contain spaces, commas or the bell
 * character (of all things).
 */
bool Channel::isValidName(const char* name)
{
	if (*name == '\0' || (*name != '#' && *name != '&'))
		return false;
	for (; *name != '\0'; name++)
		if (*name == ' ' || *name == ',' || *name == '\a')
			return false;
	return true;
}
