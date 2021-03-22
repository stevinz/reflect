//
// Description:     Reflect, C++ 11 Reflection Library
// Author:          Stephens Nunnally and Scidian Software
// License:         Distributed under the MIT License
// Source(s):       https://github.com/stevinz/reflect
//
// Copyright (c) 2021 Stephens Nunnally and Scidian Software
//
//
//####################################################################################
//####################################################################################
//####################################################################################
//######################## INCLUDE ALL REFLECTED CLASSES BELOW #######################
#define REGISTER_REFLECTION        
#include "reflect.h"

// ##### User Structs #####
#include "transform.h"





//####################################################################################
//##    Demo
//####################################################################################
// Demo Includes
#include <iostream>

// Main
int main(int argc, char* argv[]) {


    // ########## Turn on reflection, register classes / member variables
    InitializeReflection();


    // ########## Create class instance
    Transform2D t { };
        t.width =       10;
        t.height =      20;
        t.position =    std::vector<double>({1.0, 2.0, 3.0});
        t.rotation =    std::vector<double>({4.0, 5.0, 6.0});
        t.scale =       std::vector<double>({7.0, 8.0, 9.0});
        t.text =        "hello world!";


    // ########## Store HashID for later
    HashID t_hash_id = CreateHashID<Transform2D>();


    // ########## EXAMPLE: Get class type data by class type / instance / hash id / name
    std::cout << "Class Data by Type     - Name:    " << GetClassData<Transform2D>().name       << std::endl;
    std::cout << "Class Data by Instance - Members: " << GetClassData(t).member_count           << std::endl;
    std::cout << "Class Data by HashID   - Title:   " << GetClassData(t_hash_id).title          << std::endl;
    std::cout << "Class Data by Name     - HashID:  " << GetClassData("Transform2D").hash_code  << std::endl;


    // ########## EXAMPLE: Get member type data by member variable index / name
    std::cout << "By Class Type, Member Index:      " << GetMemberData<Transform2D>(t, 2).name          << std::endl;
    std::cout << "By Class Type, Member Name:       " << GetMemberData<Transform2D>("position").index   << std::endl;
    std::cout << "By Class Instance, Member Index:  " << GetMemberData(t, 2).name                       << std::endl;
    std::cout << "By Class Instance, Member Name:   " << GetMemberData(t, "position").index             << std::endl;
    std::cout << "By Class HashID, Member Index:    " << GetMemberData(t_hash_id, 2).name               << std::endl;
    std::cout << "By Class HashID, Member Name:     " << GetMemberData(t_hash_id, "position").index     << std::endl;
    

    // ########## EXAMPLE: Meta Data
    // Class meta data
    std::string description {};
    description = GetClassMeta(GetClassData<Transform2D>(), META_DATA_DESCRIPTION);
    std::cout << "Class Meta Data -  Description: " << description << std::endl;

    // Member meta data
    description = GetMemberMeta(GetMemberData<Transform2D>("position"), META_DATA_DESCRIPTION);
    std::cout << "Member Meta Data - Description: " << description << std::endl;


    // ########## Get Values
    std::cout << "Transform2D instance 't' member variable values:" << std::endl;

    // EXAMPLE: GetValue by class instance, member variable index
    HashID member_type = GetMemberData(t, 0).hash_code;
    if (member_type == MEMBER_TYPE_INT) {
        int width = GetValue<int>(t, 0);
        std::cout << "  " << GetMemberData(t, 0).title << ": " << width << std::endl;
    }

    // EXAMPLE: GetValue by class instance, member variable name
    member_type = GetMemberData(t, "position").hash_code;
    if (member_type == MEMBER_TYPE_VECTOR_DOUBLE) {
        std::vector<double> position = GetValue<std::vector<double>>(t, "position");
        std::cout << "  " << GetMemberData(t, "position").title << " X: " << position[0] << std::endl;
        std::cout << "  " << GetMemberData(t, "position").title << " Y: " << position[1] << std::endl;
        std::cout << "  " << GetMemberData(t, "position").title << " Z: " << position[2] << std::endl;
    }

    // EXAMPLE: GetValue by void* class, class hash id, and member variable name
    member_type = GetMemberData(t_hash_id, "text").hash_code;
    if (member_type == MEMBER_TYPE_STRING) {
        std::string txt = GetValue<std::string>(&t, t_hash_id, "text");
        std::cout << "  " << GetMemberData(t_hash_id, "text").title << ": " << txt << std::endl;
    }


    // ########## EXAMPLE: Iterating Members
    std::cout << "Iterating Members: " << std::endl;
    for (int p = 0; p < GetClassData("Transform2D").member_count; ++p) {
        std::cout << "  Member Index: " << p << ", Name: " << GetMemberData(t, p).name << ", Value(s): ";
        HashID type = GetMemberData(t, p).hash_code;
        if (type == MEMBER_TYPE_INT) {
            std::cout << GetValue<int>(t, p);                    
        } else 
        if (type == MEMBER_TYPE_VECTOR_DOUBLE) {
            std::vector<double> v = GetValue<std::vector<double>>(t, p);
            for (size_t c = 0; c < v.size(); c++) std::cout << v[c] << ", ";
        } else 
        if (type == MEMBER_TYPE_STRING) {
            std::cout << GetValue<std::string>(t, p);
        }
        std::cout << std::endl;
    }


    // ########## EXAMPLE: SetValue by Name (can also be called by class type / member variable index, etc...)
    member_type = GetMemberData(t, "position").hash_code;
    if (member_type == MEMBER_TYPE_VECTOR_DOUBLE) {
        std::vector<double> position = { 56.0, 58.5, 60.2 };
        SetValue(t, "position", position);
        std::cout << "After calling SetValue on 'position':" << std::endl;
        std::cout << "  " << GetMemberData(t, "position").title << " X: " << position[0] << std::endl;
        std::cout << "  " << GetMemberData(t, "position").title << " Y: " << position[1] << std::endl;
        std::cout << "  " << GetMemberData(t, "position").title << " Z: " << position[2] << std::endl;
    }


    // ########## EXAMPLE: GetValue from unknown class types
    //  
    //  If using with an entity component system, it's possible you may not have access to class type at runtime. Often a
    //  collection of components are stored in a container of void pointers. Somewhere in your code when your class is initialized,
    //  store the component class '.hash_code':                              
    //
            HashID saved_hash = GetClassData(t).hash_code;
            void*  component_ptr = (void*)(&t);
    //  
    //  Later (if your components are stored as void pointers in an array / vector / etc. with other components) you may still
    //  access the member variables of the component back to the original type. This is done by using the saved_hash from earlier:                                                     
    //
    std::cout << "Getting member variable value from unknown class type:" << std::endl;
    member_type = GetMemberData(saved_hash, 3).hash_code;
    if (member_type == MEMBER_TYPE_VECTOR_DOUBLE) {
        std::vector<double> rotation = GetValue<std::vector<double>>(component_ptr, saved_hash, 3);
        std::cout << "  Rotation X: " << rotation[0] << ", Rotation Y: " << rotation[1] << ", Rotation Z: " << rotation[2] << std::endl;
    }


    // ########## END DEMO
}
