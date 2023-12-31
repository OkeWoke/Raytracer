cmake_minimum_required(VERSION 3.20)
render_module(GObjects thirdparty-CMarkup thirdparty-pngpp Utility Stats)
#keep in mind everything is linked privately now with this macro.
#we use to have a public link to Utility/Stats and other private links like so below
#target_link_libraries(${PROJECT_NAME} PRIVATE thirdparty-CMarkup thirdparty-pngpp)
#target_link_libraries(${PROJECT_NAME} PUBLIC Utility Stats)
#
