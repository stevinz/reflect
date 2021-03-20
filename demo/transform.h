//
// Description:     Reflect, C++ 11 Reflection Library
// Author:          Stephens Nunnally and Scidian Software
// License:         Distributed under the MIT License
// Source(s):       https://github.com/stevinz/reflect
//
// Copyright (c) 2021 Stephens Nunnally and Scidian Software
//
//
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../include/reflect.h"


//####################################################################################
//##    Component: Transform
//##        Sample component used to descibe a location of an object
//############################
struct Transform {
	int width;
	int	height;
	std::vector<double> position;
	std::vector<double>	rotation;
	std::vector<double>	scale;
	std::string	text;

	REFLECT();
};


//####################################################################################
//##    Register Reflection / Meta Data
//############################
#ifdef REGISTER_REFLECTION
	REFLECT_STRUCT(Transform)
		STRUCT_META_DESCRIPTION("Describes the location and positioning of an object.")
	REFLECT_MEMBER(width)
		MEMBER_META_TYPE(PROPERTY_TYPE_INT)
	REFLECT_MEMBER(height)
		MEMBER_META_TYPE(PROPERTY_TYPE_INT)
	REFLECT_MEMBER(position)
		MEMBER_META_TITLE("Object Position")
		MEMBER_META_DESCRIPTION("Location of an object in space.")
		MEMBER_META_TYPE(PROPERTY_TYPE_VECTOR_DOUBLE)
	REFLECT_MEMBER(rotation)
		MEMBER_META_TYPE(PROPERTY_TYPE_VECTOR_DOUBLE)
	REFLECT_MEMBER(scale)
		MEMBER_META_TYPE(PROPERTY_TYPE_VECTOR_DOUBLE)
	REFLECT_MEMBER(text)
		MEMBER_META_TYPE(PROPERTY_TYPE_STRING)
	REFLECT_END(Transform)
#endif

#endif	// TRANSFORM_H

