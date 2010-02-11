ADD_LIBRARY(Extensions_HUD
  Display/HUD.h
  Display/HUD.cpp
)

TARGET_LINK_LIBRARIES(Extensions_HUD
  # core libraries
  OpenEngine_Core
  OpenEngine_Resources
  OpenEngine_Renderers
  OpenEngine_Display
  # extenstions libraries
  Extensions_CairoResource
)
