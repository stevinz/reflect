//
// Description:     Reflect, C++ 11 Reflection Library
// Author:          Stephens Nunnally and Scidian Software
// License:         Distributed under the MIT License
// Source(s):       https://github.com/stevinz/reflect
//
// Copyright (c) 2021 Stephens Nunnally and Scidian Software
//
//
#ifndef TRANSFORM2D_H
#define TRANSFORM2D_H

#include "../include/reflect.h"


//####################################################################################
//##    Component: Transform2D
//##        Sample component used to descibe a location of an object
//############################
struct Transform2D {
	int width;
	int height;
	std::vector<double> position;
	std::vector<double> rotation;
	std::vector<double> scale;
	std::string text;

	REFLECT();
};


//####################################################################################
//##    Register Reflection / Meta Data
//############################
#ifdef REGISTER_REFLECTION
	REFLECT_CLASS(Transform2D)
		CLASS_META_DATA(META_DATA_DESCRIPTION, "Describes the location and positioning of an object.")
	REFLECT_MEMBER(width)
	REFLECT_MEMBER(height)
	REFLECT_MEMBER(position)
		MEMBER_META_TITLE("Object Position")
		MEMBER_META_DATA(META_DATA_DESCRIPTION, "Location of an object in space.")
	REFLECT_MEMBER(rotation)
	REFLECT_MEMBER(scale)
	REFLECT_MEMBER(text)
	REFLECT_END(Transform2D)
#endif

#endif	// TRANSFORM2D_H

