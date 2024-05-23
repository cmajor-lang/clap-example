//
//     ,ad888ba,                              88
//    d8"'    "8b
//   d8            88,dba,,adba,   ,aPP8A.A8  88     The Cmajor Toolkit
//   Y8,           88    88    88  88     88  88
//    Y8a.   .a8P  88    88    88  88,   ,88  88     (C)2024 Cmajor Software Ltd
//     '"Y888Y"'   88    88    88  '"8bbP"Y8  88     https://cmajor.dev
//                                           ,88
//                                        888P"
//
//  The Cmajor project is subject to commercial or open-source licensing.
//  You may use it under the terms of the GPLv3 (see www.gnu.org/licenses), or
//  visit https://cmajor.dev to learn about our commercial licence options.
//
//  CMAJOR IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
//  EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
//  DISCLAIMED.

#pragma once

#include "../COM/cmaj_Library.h"
#include "cmaj_DiagnosticMessages.h"
#include "cmaj_BuildSettings.h"

namespace cmaj
{

/**
    This class acts as a wrapper around a ProgramInterface object, replacing
    the clunky COM-style methods with nicer, idiomatic C++ methods.

    This is essentially a smart-pointer to a ProgramInterface object, so bear in
    mind that copying a Program is just copying a (ref-counted) pointer - it won't
    make a copy of the actual program itself.

    When you've created a Program and parsed one or more source files with it, you
    can pass it over to cmaj::Engine::load() to start actually compiling it.
*/
struct Program
{
    Program();
    ~Program();

    /// Resets this program to an empty state.
    void reset();

    /// Attempts to parse some Cmajor code and add it to the current program.
    /// Note that this won't load the file for you - the caller must do that, and
    /// provide the filename and content. (The filename is needed so that the compiler
    /// can use it in error message locations, but you can pass an empty string if the
    /// code isn't from a file).
    bool parse (DiagnosticMessageList& messages,
                const std::string& filename,
                const std::string& fileContent);

    /// Returns a JSON version of the current syntax tree.
    std::string getSyntaxTree (const SyntaxTreeOptions&) const;

    //==============================================================================
    /// The underlying program object.
    ProgramPtr program;

private:
    Library::SharedLibraryPtr library;
};


//==============================================================================
//        _        _           _  _
//     __| |  ___ | |_   __ _ (_)| | ___
//    / _` | / _ \| __| / _` || || |/ __|
//   | (_| ||  __/| |_ | (_| || || |\__ \ _  _  _
//    \__,_| \___| \__| \__,_||_||_||___/(_)(_)(_)
//
//   Code beyond this point is implementation detail...
//
//==============================================================================

inline Program::Program() = default;
inline Program::~Program()  { reset(); }

inline void Program::reset()
{
    program = {};  // explicitly release the program before the library
    library = {};
}

inline bool Program::parse (DiagnosticMessageList& messages,
                            const std::string& filename,
                            const std::string& fileContent)
{
    if (program == nullptr)
    {
        program = Library::createProgram();
        library = Library::getSharedLibraryPtr();
    }

    if (auto result = choc::com::StringPtr (program->parse (filename.c_str(), fileContent.data(), fileContent.length())))
        return messages.addFromJSONString (result);

    return true;
}

inline std::string Program::getSyntaxTree (const SyntaxTreeOptions& options) const
{
    if (program == nullptr)
        return {};

    return choc::com::StringPtr (program->getSyntaxTree (options));
}

} // namespace cmaj
