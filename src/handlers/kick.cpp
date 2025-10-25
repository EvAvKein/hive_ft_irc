#include "client.hpp"
#include "channel.hpp"
#include "utility.hpp"
#include "server.hpp"
#include "irc.hpp"
#include <cstring>

/**
 * Handle a KICK message.
 */
void Client::handleKick(int argc, char** argv)
{
	// Must have at least <channel> and <user>
	if (argc < 2)
		return sendLine("461 ", nick, " KICK :Not enough parameters");

	// Find the target channel.
	char* channelName = argv[0];
	Channel* channel = server->findChannelByName(channelName);
	if (channel == nullptr) {
		log::warn("KICK: No such channel: ", channelName);
		return sendLine("403 ", nick, " ", channelName, " :No such channel");
	}

	// Check that the sender is in the channel.
	if (!channel->isMember(*this)) {
		log::warn("KICK: ", nick, " tried to kick but is not a member of ", channelName);
		return sendLine("442 ", nick, " ", channelName, " :You're not on that channel");
	}

	// Check that the sender has operator privileges on the channel.
	if (!channel->isOperator(*this))
		return sendLine("482 ", nick, " ", channelName, " :You're not channel operator");

	// Use an empty <reason> if none was given.
	const char* reason = argc == 3 ? argv[2] : "";

	// Process the list of target clients to kick.
	char* targetList = argv[1];
	while (*targetList != '\0') {

		// Find the target channel member.
		char* targetName = nextListItem(targetList);
		Client* target = channel->findClientByName(targetName);
		if (target == nullptr) {
			log::warn("KICK: ", nick, " tried to kick ", targetName, " but they are not in ", channelName);
			return sendLine("441 ", nick, " ", targetName, " ", channelName, " :They aren't on that channel");
		}

		// Broadcast kick message.
		for (Client* member: channel->members) {
			member->send(":", fullname, " ");
			member->send("KICK ", channelName, " ", targetName);
			member->sendLine(" :", reason);
		}

		// Remove kicked dude.
		channel->removeMember(*target);
		log::info("KICK: ", nick, " kicked ", targetName, " from ", channelName);
	}
}
