#include "channel.hpp"
#include "client.hpp"
#include "server.hpp"
#include "utility.hpp"

/**
 * Handle a LIST command.
 */
void Client::handleList(int argc, char** argv)
{
	// Check that enough parameters were given.
	if (argc > 1)
		return sendLine("461 ", nick, " LIST :Not enough parameters");

	// The list start reply is always sent.
	sendLine("321 ", nick, " Channel :Users  Name");

	// If no parameters were given, list all channels.
	if (argc == 0) {
		for (auto& [_, channel]: server->allChannels()) {
			send("322 ", nick, " ", channel.name, " ");
			sendLine(channel.getMemberCount(), " :", channel.topic);
		}
	
	// Otherwise, list just the info for the listed channels.
	} else if (argc == 1) {
		char* channelList = argv[0];
		while (*channelList != '\0') {
			char* channelName = nextListItem(channelList);
			Channel* channel = server->findChannelByName(channelName);
			if (channel != nullptr) {
				send("322 ", nick, " ", channel->name, " ");
				sendLine(channel->getMemberCount(), " :", channel->topic);
			}
		}
	}

	// The list end reply is always sent.
	sendLine("323 ", nick, " :End of /LIST");
}
