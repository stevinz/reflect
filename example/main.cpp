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


    // ########## EXAMPLE: Get class TypeData by class type / instance / hash id / name
    std::cout << "Class Data by Type     - Name:    " << ClassData<Transform2D>().name      << std::endl;
    std::cout << "Class Data by Instance - Members: " << ClassData(t).member_count          << std::endl;
    std::cout << "Class Data by HashID   - Title:   " << ClassData(t_hash_id).title         << std::endl;
    std::cout << "Class Data by Name     - HashID:  " << ClassData("Transform2D").hash_code << std::endl;


    // ########## EXAMPLE: Get member TypeData by member variable index / name
    std::cout << "By Class Type, Member Index:      " << MemberData<Transform2D>(t, 2).name          << std::endl;
    std::cout << "By Class Type, Member Name:       " << MemberData<Transform2D>("position").index   << std::endl;
    std::cout << "By Class Instance, Member Index:  " << MemberData(t, 2).name                       << std::endl;
    std::cout << "By Class Instance, Member Name:   " << MemberData(t, "position").index             << std::endl;
    std::cout << "By Class HashID, Member Index:    " << MemberData(t_hash_id, 2).name               << std::endl;
    std::cout << "By Class HashID, Member Name:     " << MemberData(t_hash_id, "position").index     << std::endl;
    

    // ########## EXAMPLE: Meta Data
    // Class meta data
    std::string description {};
    description = GetMetaData(ClassData<Transform2D>(), META_DATA_DESCRIPTION);
    std::cout << "Class Meta Data -  Description: " << description << std::endl;

    // Member meta data
    description = GetMetaData(MemberData<Transform2D>("position"), META_DATA_DESCRIPTION);
    std::cout << "Member Meta Data - Description: " << description << std::endl;


    // ########## Get Values
    std::cout << "Transform2D instance 't' member variable values:" << std::endl;

    // EXAMPLE: Return member variable by class instance, member variable index
    TypeData& member = MemberData(t, 0);
    if (member.hash_code == MEMBER_TYPE_INT) {
        int& width = ClassMember<int>(&t, member);
        std::cout << "  " << member.title << ": " << width << std::endl;
    }

    // EXAMPLE: Return member variable by class instance, member variable name
    member = MemberData(t, "position");
    if (member.hash_code == MEMBER_TYPE_VECTOR_DOUBLE) {
        std::vector<double>& position = ClassMember<std::vector<double>>(&t, member);
        std::cout << "  " << MemberData(t, "position").title << " X: " << position[0] << std::endl;
        std::cout << "  " << MemberData(t, "position").title << " Y: " << position[1] << std::endl;
        std::cout << "  " << MemberData(t, "position").title << " Z: " << position[2] << std::endl;
    }

    // EXAMPLE: Return member variable by void* class, class hash id, and member variable name
    member = MemberData(t_hash_id, "text");
    if (member.hash_code == MEMBER_TYPE_STRING) {
        std::string& txt = ClassMember<std::string>(&t, member);
        std::cout << "  " << MemberData(t_hash_id, "text").title << ": " << txt << std::endl;
    }


    // ########## EXAMPLE: Iterating Members
    std::cout << "Iterating Members: " << std::endl;
    for (int p = 0; p < ClassData("Transform2D").member_count; ++p) {
        std::cout << "  Member Index: " << p << ", Name: " << MemberData(t, p).name << ", Value(s): ";
        member = MemberData(t, p);
        if (member.hash_code == MEMBER_TYPE_INT) {
            std::cout << ClassMember<int>(&t, member);                    
        } else 
        if (member.hash_code == MEMBER_TYPE_VECTOR_DOUBLE) {
            std::vector<double> v = ClassMember<std::vector<double>>(&t, member);
            for (size_t c = 0; c < v.size(); c++) std::cout << v[c] << ", ";
        } else 
        if (member.hash_code == MEMBER_TYPE_STRING) {
            std::cout << ClassMember<std::string>(&t, member);
        }
        std::cout << std::endl;
    }


    // ########## EXAMPLE: SetValue by Name (can also be called by class type / member variable index, etc...)
    member = MemberData(t, "position");
    if (member.hash_code == MEMBER_TYPE_VECTOR_DOUBLE) {
        ClassMember<std::vector<double>>(&t, member) = { 56.0, 58.5, 60.2 };
        std::cout << "After calling SetValue on 'position':" << std::endl;
        std::cout << "  " << MemberData(t, "position").title << " X: " << ClassMember<std::vector<double>>(&t, member)[0] << std::endl;
        std::cout << "  " << MemberData(t, "position").title << " Y: " << ClassMember<std::vector<double>>(&t, member)[1] << std::endl;
        std::cout << "  " << MemberData(t, "position").title << " Z: " << ClassMember<std::vector<double>>(&t, member)[2] << std::endl;
    }


    // ########## EXAMPLE: GetValue from unknown class types
    //  
    //  If using with an entity component system, it's possible you may not have access to class type at runtime. Often a
    //  collection of components are stored in a container of void pointers. Somewhere in your code when your class is initialized,
    //  store the component class '.hash_code':                              
    //
            HashID saved_hash = ClassData(t).hash_code;
            void*  component_ptr = (void*)(&t);
    //  
    //  Later (if your components are stored as void pointers in an array / vector / etc. with other components) you may still
    //  access the member variables of the component back to the original type. This is done by using the saved_hash from earlier:                                                     
    //
    std::cout << "Getting member variable value from unknown class type:" << std::endl;
    member = MemberData(saved_hash, 3);
    if (member.hash_code == MEMBER_TYPE_VECTOR_DOUBLE) {
        std::vector<double>& rotation = ClassMember<std::vector<double>>(component_ptr, member);
        std::cout << "  Rotation X: " << rotation[0] << ", Rotation Y: " << rotation[1] << ", Rotation Z: " << rotation[2] << std::endl;
    }


    // ########## END DEMO
}
