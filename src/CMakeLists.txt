add_library(easyqt
	${EASYQT_MOC_HEADERS}
	application.cxx
	commands.cxx
	flowlayout.cxx
	logging.cxx
	menubar.cxx
	scrollarea.cxx
	uiloader.cxx
	utils.cxx
)
add_library(easyqt::easyqt ALIAS easyqt)
target_link_libraries(
	easyqt
	PUBLIC
	Qt5::Widgets
	Qt5::UiTools
	pugixml::pugixml
	singleton++
)

target_include_directories(
	easyqt
	PUBLIC
		${PROJECT_SOURCE_DIR}/include/easyqt/
		"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>"
)
set_target_properties(easyqt PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION 1
)

install(
	TARGETS easyqt
	ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

