####################################################################################################
# @file
# 
# @brief            File contains the 'launcher()' method used to launch a standalone version of a 
#                   documentation update script. This script is not intended to be invoked directly 
#                   from user space. Rather, it should be imported and the 'launcher()' method 
#                   should be invoke appropriately.
# 
# @retval           100                     Returned if script is invoked directly from the command 
#                                           line or Python launcher utility.
# @retval           101                     Unable to exectue 'standalone_file_name.main()' without 
#                                           an error.
# 
# @attention        DEPENDENCIES:
#                       - Python {2.7.9++; 3.4.2++}
#                       - Any dependencies required by the standalone script(s) being invoked.
# 
# @returns          Aside from the '100' and '101' error codes, no values returned directory from 
#                   script. Messages and error codes may be returned by the scripts invoked via the 
#                   'launcher()' method.
####################################################################################################


####################################################################################################
# IMPORT MODULES
####################################################################################################
from __future__ import print_function                                           # Add support for update print function.
import os, sys                                                                  # Manipulate system-level functions.




####################################################################################################
# @brief            Function handles invoking the standalone update script of the same namesake as 
#                   the script invoking this function.
# 
# @details          Determines the location of the standalone scripts directory to use from the 
#                   provided 'path' information. Once determined, removes the current directory 
#                   from the Python path and append the directory defined in 'path'.
#                                                                                                   \n\n
#                   Once the system [Python] path has been set, a simple 'import' method can be used 
#                   to obtain an instance and then invoke the 'main()' function presumed to be in 
#                   the script.
# 
# @note             To aid the standalone script(s) in finding the correct files and folders, the 
#                   optional '-d|--directory' field will be added if not already present in the 
#                   script arguments list. The value provided will be equal to absolute path to 
#                   the directory containing this script.
# 
# @param[in]        path                    Path to the 'standalone' scripts directory to be 
#                                           utilized:
#                                               - If an absolute path is provided, not modifications 
#                                                 are done to the script.
#                                               - If a relative path is provided, the path is 
#                                                 updated to be the concatenation of the directory 
#                                                 containing this script and the value provided.
# 
# @returns          No values provided directory from function, but messages and error codes may 
#                   be returned by the scripts invoked via the 'import()' method.
####################################################################################################
def launcher(path):
    # Determine if '-s|--slient' mode requested. Need to do so before invoking any scripts as 
    # 'sys.argv' is likely modified by a standalone script.
    if '-s' in sys.argv or '--silent' in sys.argv:
        flag_silent_mode = True
    else:
        flag_silent_mode = False
    
    # Define paths needed to execute standalone script.
    # >> NOTE:  Using 'argv[0]' so as to grab location of parent script invoking this function.
    dir_script      = os.path.dirname(os.path.realpath(sys.argv[0]))
    if os.path.isabs(path):
        dir_standalone  = path
    else:
        dir_standalone  = os.path.realpath(os.path.join(dir_script, path))
    
    # Remove current directory and append standalone path to system path if not already a component.
    if dir_script in sys.path:
        sys.path.remove(dir_script)
    if not os.path.realpath(dir_standalone) in sys.path:
        sys.path.append(dir_standalone)
    
    # Determine the name of the script to invoke.
    standalone_script_name, exten = os.path.splitext(os.path.basename(sys.argv[0]))
    
    try:
        # Import file containing core function we need to invoke to execute the wrapper functionality.
        print("Importing the '<%s>/%s%s' script." % (dir_standalone, standalone_script_name, exten))
        module = __import__(standalone_script_name)
        
        # Add optional '-d|--directory' information with location of current file if no directory 
        # information already in the arguments provided to the script.
        if not '-d' in sys.argv:
            sys.argv.extend(['-d', dir_script])
            print("Adding the '-d|--directory' flag '<%s>'." % dir_script)
        else:
            print("Value for '-d|--directory' flag provided to function.")
        module.main(sys.argv[1:])
        
        # Set return code value to indicate success.
        rc = 0
        
    except:
        # Set error return code value and print notification message to user.
        rc = 101
        print("[FATAL_ERROR]> Unable to execute '%s.main()'. Are you sure the standalone file '<%s>/%s%s' exists and contains the required 'main()' method?" % (standalone_script_name, dir_standalone, standalone_script_name, exten) )
        
    
    # Pause for user to observe the error message.
    if flag_silent_mode != True:
        if sys.version[0] >= "3":
            input("Press any key to continue ...")
        else:
            raw_input("Press any key to continue ...")
    
    # Exit with the determined return code.
    return rc




####################################################################################################
# TREAT SCRIPT AS STANDALONE IF FILE WAS NOT IMPORTED.
####################################################################################################
if __name__ == "__main__":
    # Notify user that this script cannot be run directly.
    print("[FATAL_ERROR]> This script cannot be called directly from user space. It is intended to:  (1) be imported; (2) have its 'launcher(<path>)' method invoked.")
    
    # Pause for user to observe the error message.
    if not '-s' in sys.argv and not '--silent' in sys.argv:
        if sys.version[0] >= "3":
            input("Press any key to continue ...")
        else:
            raw_input("Press any key to continue ...")
    
    # Exit with exit code from main function.
    sys.exit(100)




