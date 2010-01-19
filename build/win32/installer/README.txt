To create the installer you must have Inno Setup Compiler >= 5.3 installed.

Then follow this procedure:
1. Check out a clean copy of fife trunk.
2. Build FIFE (ensure that the required DLLs get copied to <FIFE>\trunk\engine\python\fife)
3. From the <FIFE>\trunk directory execute the command 'python setup.py bdist_wininst' to build the python module installer.
4. Compile the FIFE installer (right click fife.iss -> Compile)

You should now have a working installer called FIFE {VERSION}.

Now that you have the working copy set up, all you have to do to create a new installer is:
1. Update the fife directory (svn update)
2. Recompile FIFE
3. Run 'python setup.py bdist_wininst' to create the python module installer
3. Compile the FIFE installer
