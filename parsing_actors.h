#ifndef CRT_PARSING_ACTORS_H
#define CRT_PARSING_ACTORS_H

// IMPORTANT: this file can't be used alone, it's only to be included in scnparser.h which
// contains the good includes and typedefs!

#include <boost/array.hpp>

//---------------------------------------------------------------------------------------------------------
// Scene actors
member_function_ptr<void, Scene, const std::string&>	scn_output_file		= bind(&Scene::setOutputFile);
member_function_ptr<void, Scene, int>					scn_set_width		= bind(&Scene::setWidth);
member_function_ptr<void, Scene, int>					scn_set_height		= bind(&Scene::setHeight);
member_function_ptr<void, Scene, bool>					scn_store_z			= bind(&Scene::storeZValues);
member_function_ptr<void, Scene, const Color4&>			scn_set_background	= bind(&Scene::setBackground);
//----------------------------------------------------------------------------------------------------------

#endif
