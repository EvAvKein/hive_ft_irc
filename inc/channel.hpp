#pragma once

#include <map>
#include <string>

class Client;

class Channel
{
public:

	char symbol = '=';						// The channel symbol (one of '=', '@', '*')
	std::string name;						// The name of the channel
	std::string topic;						// The current topic
	std::string key;						// Key needed to join the channel
	std::map<std::string, Client*> members;	// Clients joined to this channel

	static bool isValidName(const char* name);
};
