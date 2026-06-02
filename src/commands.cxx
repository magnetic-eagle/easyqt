#include <map>
#include <string>

#include "application.hxx"
#include "commands.hxx"
#include "logging.hxx"
#include "objectregistry.hxx"
#include "settingsdialog.hxx"

namespace easyqt {
	EASYQTCOMMAND_GEN_IMPL(CommandShowSettingsDialog, "show-settings-dialog", ObjectRegistry::get<SettingsDialog>()->show())
	EASYQTCOMMAND_GEN_IMPL(CommandQuit, "quit", Application::instance()->exit())
	
	static std::map<std::string, CommandPtr > _commandMap = {
		{"show-settings-dialog", new CommandShowSettingsDialog()},
		{"quit", new CommandQuit()},
	};

	CommandPtr getCommand(std::string name) {
		if (!_commandMap.contains(name)) {
			LOG(WARNING, "Failed getting command: no command with name " << name);
			return nullptr;
		}
		return _commandMap[name];
	}
	
	void addCommand(CommandPtr command) {
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

