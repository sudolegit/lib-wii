####################################################################################################
# @file
# 
# @brief            Relative script used to invoke a standalone version of the same namesake.
# 
# @details          The core logic used to launch the standalone version of this script lies in a 
#                   file within the same directory of this script [or, minimally in the Python 
#                   include path].
#                                                                                                   \n\n
#                   The script invoke will handle the heavy lifting and can infer the name of this 
#                   script. The main item required is the path [be it relative or absolute] to 
#                   the directory containing [all of] the standalone scripts.
# 
# @attention        Please see the standalone version of this script for all notes, warnings, and 
#                   notes on values returned.
####################################################################################################

# Import required module(s).
import os, sys                                                                  # Manipulate system-level functions.

# Import file containing core function we need to invoke to execute the wrapper functionality.
# >> NOTE:  This will pasively provide all arguments provided to this script and the name of this 
#           script to the script invoked.
mod = __import__('launch_standalone_script')
rc  = mod.launcher("../../../doxygen/scripts/standalone")

# Exit with return code provided form 'launcher()' method.
sys.exit(rc)


