ADD_LIBRARY(Extensions_HUD
  Display/HUD.h
  Display/HUD.cpp
)

TARGET_LINK_LIBRARIES(Extensions_HUD
  OpenEngine_Core
  OpenEngine_Resources
  OpenEngine_Renderers
)
