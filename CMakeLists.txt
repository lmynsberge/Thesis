# Locate Paraview build and then import CMake configuration,
# macros etc. from it.
FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

# Use the ADD_PARAVIEW_PLUGIN macro to build a plugin
ADD_PARAVIEW_PLUGIN(
   MyThesis                            #<--Name of my plugin
  "1.0"                                #<--This is version number 1.0
   SERVER_MANAGER_XML MyThesis.xml     #<--My server manager xml
   SERVER_MANAGER_SOURCES MyThesis.cxx #<--Source file for my plugin
   GUI_RESOURCE_FILES MyThesisGUI.xml  #<--My GUI xml
)
