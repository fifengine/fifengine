To create the installer you must have Inno Setup Compiler >= 5.3 installed.

Then follow this procedure:
1. Check out a clean copy of fife trunk into the same directory as the installer.
2. Rename the directory from "trunk" to "fife"
3. Install the Win32 SDK
4. Build FIFE
5. Compile the installer (right click fife.iss -> Compile)

You should now have a working installer called FIFE {VERSION}.

Now that you have the working copy set up, all you have to do to create a new installer is:
1. Update the fife directory (svn update)
2. Recompile FIFE
3. Compile the installer
