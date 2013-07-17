
set(SOURCES BuiltinNameToTimeTranslator.cxx INameToTimeTranslator.cxx DeprecatedRecord.cxx CompleteRecord.cxx Record.cxx MasterModel.cxx)

set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Qt5Core REQUIRED)
find_package(Boost 1.52.0 REQUIRED date_time)
include_directories(${Boost_INCLUDE_DIRS})

add_library(core ${SOURCES})

qt5_use_modules(core Core Widgets)
target_link_libraries(core ${Boost_LIBRARIES})
