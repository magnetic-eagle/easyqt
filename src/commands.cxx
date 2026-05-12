#include <map>
#include <string>

#include "application.hxx"
#include "commands.hxx"
#include "logging.hxx"

namespace easyqt {
	EASYQTCOMMAND_GEN_IMPL(CommandQuit, "quit", Application::instance()->exit())
	
	static std::map<std::string, std::shared_ptr<Command> > _commandMap = {
		{"quit", CommandQuit::instance()},
	};

	std::shared_ptr<Command> getCommand(std::string name) {
		if (!_commandMap.contains(name)) {
			LOG(WARNING, "Failed getting command: no command with name " << name);
			return nullptr;
		}
		return _commandMap[name];
	}
	
	void addCommand(std::shared_ptr<Command> command) {
		if (_commandMap.contains(command->name())) {
			LOG(WARNING, "Failed adding command: a command with name " << command->name() << " was already previously added");
			return;
		}
		_commandMap[command->name()] = command;
	}
	
	void removeCommand(std::string name) {
		if (!_commandMap.contains(name)) {
			LOG(WARNING, "Failed removing command: no command with name " << name);
		}
		_commandMap.erase(name);
	}
};

