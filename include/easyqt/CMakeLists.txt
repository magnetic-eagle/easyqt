set(MOC_HEADERS
	application.hxx
	command.hxx
	flowlayout.hxx
	menubar.hxx
	scrollarea.hxx
	settingsdialog.hxx
	uiloader.hxx
)
list(TRANSFORM MOC_HEADERS PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
set(EASYQT_MOC_HEADERS ${MOC_HEADERS} CACHE INTERNAL "")
