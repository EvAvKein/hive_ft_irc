#include "irc.hpp"

void Server::handleMessage(char** parts, int partCount)
{
	// Remove the command from the parameter array.
	char* command = parts[0];
	partCount--;
	parts++;

	// Send the message to the handler for that command.
	if (matchIgnoreCase(command, "USER"))
		return handleUser(parts, partCount);
	if (matchIgnoreCase(command, "NICK"))
		return handleNick(parts, partCount);
	if (matchIgnoreCase(command, "PASS"))
		return handlePass(parts, partCount);
	if (matchIgnoreCase(command, "CAP"))
		return handleCap(parts, partCount);
	if (matchIgnoreCase(command, "JOIN"))
		return handleJoin(parts, partCount);
	logError("Unimplemented command '%s'", command);
}


void Server::handleUser(char** params, int paramCount)
{
	(void) params, (void) paramCount;
	logWarn("Unimplemented command: JOIN");
}

void Server::handleNick(char** params, int paramCount)
{
	(void) params, (void) paramCount;
	logWarn("Unimplemented command: NICK");
}

void Server::handlePass(char** params, int paramCount)
{
	(void) params, (void) paramCount;
	logWarn("Unimplemented command: PASS");
}

void Server::handleCap(char** params, int paramCount)
{
	(void) params, (void) paramCount;
	logWarn("Unimplemented command: CAP");
}

void Server::handleJoin(char** params, int paramCount)
{
	(void) params, (void) paramCount;
	logWarn("Unimplemented command: JOIN");
}
