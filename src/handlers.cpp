#include "irc.hpp"

/**
 * Handle any type of message. Removes the command from the parameter list, then
 * calls the handler for that command with the remaining parameters.
 */
void Server::handleMessage(Client& client, std::string_view* params, int count)
{
	// Ignore empty messages.
	if (count == 0)
		return;

	// Remove the command from the parameter array.
	std::string_view cmd = params[0];
	params++;
	count--;

	// Send the message to the handler for that command.
	if (matchIgnoreCase(cmd, "USER"))
		return handleUser(client, params, count);
	if (matchIgnoreCase(cmd, "NICK"))
		return handleNick(client, params, count);
	if (matchIgnoreCase(cmd, "PASS"))
		return handlePass(client, params, count);
	if (matchIgnoreCase(cmd, "CAP"))
		return handleCap(client, params, count);
	if (matchIgnoreCase(cmd, "JOIN"))
		return handleJoin(client, params, count);

	// Log any unimplemented commands, so that they can be added eventually.
	logError("Unimplemented command '%.*s'", int(cmd.size()), cmd.data());
}

/**
 * Handle a USER message.
 */
void Server::handleUser(Client& client, std::string_view* params, int count)
{
	(void) client, (void) params, (void) count;
	logWarn("Unimplemented command: JOIN");
}

/**
 * Handle a NICK message.
 */
void Server::handleNick(Client& client, std::string_view* params, int count)
{
	(void) client, (void) params, (void) count;
	logWarn("Unimplemented command: NICK");
}

/**
 * Handle a PASS message.
 */
void Server::handlePass(Client& client, std::string_view* params, int count)
{
	(void) client, (void) params, (void) count;
	logWarn("Unimplemented command: PASS");
}

/**
 * Handle a CAP message.
 */
void Server::handleCap(Client& client, std::string_view* params, int count)
{
	(void) client, (void) params, (void) count;
	logWarn("Unimplemented command: CAP");
}

/**
 * Handle a JOIN message.
 */
void Server::handleJoin(Client& client, std::string_view* params, int count)
{
	(void) client, (void) params, (void) count;
	logWarn("Unimplemented command: JOIN");
}
