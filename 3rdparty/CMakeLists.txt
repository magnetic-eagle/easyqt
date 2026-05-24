set(SYSTEM_PUGIXML OFF)

if (PkgConfig_FOUND)
	pkg_check_modules(PugiXML IMPORTED_TARGET pugixml)

	if(PugiXML_FOUND)
		set(SYSTEM_PUGIXML ON)
		add_library(pugixml::pugixml INTERFACE IMPORTED GLOBAL)
		target_link_libraries(pugixml::pugixml INTERFACE PkgConfig::PugiXML)
	endif()
endif()

set(ENABLE_TESTING OFF)
if (NOT TARGET pugixml::pugixml AND NOT PugiXML_FOUND)
	add_subdirectory(pugixml)
endif()
unset(ENABLE_TESTING)
