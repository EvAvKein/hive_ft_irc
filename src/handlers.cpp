#include "channel.hpp"
#include "client.hpp"
#include "irc.hpp"
#include "server.hpp"
#include "utility.hpp"

/**
 * Handle any type of message. Removes the command from the parameter list, then
 * calls the handler for that command with the remaining parameters.
 */
void Server::handleMessage(Client& client, int argc, char** argv)
{
	// Ignore empty messages.
	if (argc == 0)
		return;

	// Array of message handlers.
	using Handler = void (Server::*)(Client&, int, char**);
	static const std::pair<const char*, Handler> handlers[] = {
		{"USER", &Server::handleUser},
		{"NICK", &Server::handleNick},
		{"PASS", &Server::handlePass},
		{"CAP",  &Server::handleCap},
		{"JOIN", &Server::handleJoin},
	};

	// Send the message to the handler for that command.
	for (const auto& [command, handler]: handlers) {
		if (matchIgnoreCase(command, argv[0])) {
			return (this->*handler)(client, argc - 1, argv + 1);
		}
	}

	// Log any unimplemented commands, so that they can be added eventually.
	logError("Unimplemented command '%s'", argv[0]);
}

/**
 * Handle a USER message.
 */
void Server::handleUser(Client& client, int argc, char** argv)
{
	(void) client, (void) argc, (void) argv;
	logWarn("Unimplemented command: USER");
}

/**
 * Handle a NICK message.
 */
void Server::handleNick(Client& client, int argc, char** argv)
{
	(void) client, (void) argc, (void) argv;
	logWarn("Unimplemented command: NICK");
}

/**
 * Handle a PASS message.
 */
void Server::handlePass(Client& client, int argc, char** argv)
{
	(void) client, (void) argc, (void) argv;
	logWarn("Unimplemented command: PASS");
}

/**
 * Handle a CAP message.
 */
void Server::handleCap(Client& client, int argc, char** argv)
{
	(void) client, (void) argc, (void) argv;
	logWarn("Unimplemented command: CAP");
}

/**
 * Handle a JOIN message.
 */
void Server::handleJoin(Client& client, int argc, char** argv)
{
	// Check that enough parameters were provided.
	if (argc < 1 || argc > 2)
		return client.sendLine("461 ", client.nick, " JOIN :Not enough parameters");

	// Check that the client is registered.
	if (!client.isRegistered)
		return client.sendLine("451 ", client.nick, " :You have not registered");

	// Join a list of channels.
	char noKeys[] = ""; // Empty list used if no keys were given.
	char* names = argv[0]; // List of channel names ("#abc,#def")
	char* keys = argc == 2 ? argv[1] : noKeys; // List of keys ("key1,key2")
	while (*names != '\0') {

		// Get the next channel name and key in the comma-separated list.
		char* name = nextListItem(names);
		char* key = nextListItem(keys);

		// Check that a valid channel name was given.
		if (!Channel::isValidName(name)) {
			client.sendLine("403 ", client.nick, " ", name, " :No such channel");
			continue;
		}

		// If there's no channel by that name, create it.
		Channel* channel = findChannelByName(name);
		if (channel == nullptr) {
			logInfo("Creating new channel %s", name);
			channel = &_channels[name]; // Create a new empty channel.
			channel->name = name;
		}

		// Skip if the client is already in the channel.
		if (channel->members.find(client.nick) != channel->members.end())
			continue;

		// Issue an error message if the key doesn't match.
		if (channel->key != key) {
			client.sendLine(client.nick, " ", name, " :Cannot join channel (+k)");
			continue;
		}

		// Join the channel.
		logInfo("%s joined channel %s", client.nick.c_str(), name);
		channel->members[client.nick] = &client;

		// Send a join message, the topic, and a list of channel members.
		client.sendLine(":", client.nick, " JOIN ", name);
		client.sendLine("332 ", client.nick, " ", name, " :", channel->topic);
		client.send("353 ", client.nick, " ", channel->symbol, " ", name, " :");
		for (auto& [_, member]: channel->members)
			client.send(member->prefix, member->nick, " ");
		client.sendLine(); // Line break at the end of the member list.
		client.sendLine("366 ", client.nick, " ", name, " :End of /NAMES list");
	}
}
