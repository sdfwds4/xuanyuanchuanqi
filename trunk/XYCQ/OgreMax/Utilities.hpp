/*
 * OgreMaxViewer - An Ogre 3D-based viewer for .scene and .mesh files
 * Copyright 2008 Derek Nedelman
 *
 * This code is available under the OgreMax Free License:
 *   -You may use this code for any purpose, commercial or non-commercial.
 *   -If distributing derived works (that use this source code) in binary or source code form, 
 *    you must give the following credit in your work's end-user documentation: 
 *        "Portions of this work provided by OgreMax (www.ogremax.com)"
 *
 * Derek Nedelman assumes no responsibility for any harm caused by using this code.
 * 
 * OgreMaxViewer was written by Derek Nedelman and released at www.ogremax.com 
 */

#ifndef OgreMax_Utilities_INCLUDED
#define OgreMax_Utilities_INCLUDED


//Includes---------------------------------------------------------------------
#include <OgreString.h>


//Classes----------------------------------------------------------------------
namespace OgreMax
{
    /** 
     * Utility functions that would be useful in any Ogre-based application
     */
    class Utilities
    {
    public:
    #if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
        static Ogre::String GetMacBundlePath();
    #endif

        static Ogre::String GetApplicationResourcesPath();
    
        /**
         * Performs a case insensitive comparison to determine if string ends with a another string
         * @param text [in] - The string to check
         * @param endsWith [in] - The string to check for at the end of the input text
         * @return Returns true if the specified text ends with the other string, false otherwise.
         */
        static bool EndsWithNoCase(const Ogre::String& text, const Ogre::String& endsWith);

        /**
         * Determines if the specifiec image codec can perform coding
         * @param name [in] - The image codec name
         */
        static bool ImageCodecCanCode(const Ogre::String& name);
    };

}

#endif