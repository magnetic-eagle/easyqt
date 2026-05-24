#ifndef EASYQT_COMMANDS_HXX
#define EASYQT_COMMANDS_HXX

#include "command.hxx"

namespace easyqt {
	CommandPtr getCommand(std::string name);
	void addCommand(CommandPtr command);
	inline void addCommands(CommandPtrVector commands) {
		for (CommandPtr command: commands) {
			addCommand(command);
		}
	}
	void removeCommand(std::string name);
	inline void removeCommand(CommandPtr command) { removeCommand(command->name()); };

	EASYQTCOMMAND_GEN_DEF(CommandShowSettingsDialog)
	EASYQTCOMMAND_GEN_DEF(CommandQuit)
}
#endif

