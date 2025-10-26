#include "client.hpp"
#include "channel.hpp"
#include "utility.hpp"
#include "server.hpp"
#include "irc.hpp"

/**
 * Handle a PASS message.
 */
void Client::handlePass(int argc, char** argv)
{
	// Check parameter count.
	if (!checkParams("PASS", false, argc, 1, 1))
		return;

	// Check that the client is not already registered.
	if (isRegistered) {
		log::warn(nick, " PASS: Already registered user");
		return sendNumeric("462", ":You may not reregister");
	}

	// Check that the password matches.
	if (!server->correctPassword(argv[0])) {
		isPassValid = false;
		sendNumeric("464", ":Password incorrect");
		log::warn(nick, " PASS: Password is incorrect");
		return server->disconnectClient(*this, "Incorrect password");
	}

	// Mark the client as authenticated, and complete registration, if
	// applicable.
	bool passAlreadyValid = isPassValid;
	isPassValid = true;
	if (!passAlreadyValid)
		handleRegistrationComplete();
}
