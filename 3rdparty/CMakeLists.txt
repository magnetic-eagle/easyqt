set(SYSTEM_PUGIXML OFF)

if (NOT TARGET pugixml::pugixml)
	if (PkgConfig_FOUND)
		pkg_check_modules(PugiXML IMPORTED_TARGET pugixml)

		if(PugiXML_FOUND)
			set(SYSTEM_PUGIXML ON)
			add_library(pugixml::pugixml INTERFACE IMPORTED GLOBAL)
			target_link_libraries(pugixml::pugixml INTERFACE PkgConfig::PugiXML)
		endif()
	endif()
endif()

if (NOT TARGET pugixml::pugixml AND NOT PugiXML_FOUND)
	set(ENABLE_TESTING OFF)
	add_subdirectory(pugixml)
	unset(ENABLE_TESTING)
endif()
