import sys, os , pythoncom

def main (from_path, to_path):
# check if the output-folder exists
	if os.path.exists(to_path) == False:
		os.mkdir(to_path);
      
# traverse the unit_test-files
	files = os.listdir(from_path)
	guidlist=[]
	filelist=[]
   
	for file in files:
		if (file[-4:] == ".cpp"):
			guidlist.append(writeProject(file[0:-4], to_path))
			filelist.append(file[0:-4].replace("test_", "testprog_"))

	writeSolution(to_path, filelist, guidlist)

def writeProject(file, to_path):
	infile = open("unittest_template.xml", "r")
	outfile = open(to_path + "/" + file.replace("test_", "testprog_") + ".vcproj", "w")
	text = infile.read()
	guid = pythoncom.CreateGuid()
	text = text.replace("__INSERT_NAME_HERE__", file.replace("test_", "testprog_"))
	text = text.replace("__INSERT_FILENAME_HERE__", file)
	text = text.replace("__INSERT_GUID_HERE__", str(guid))
	outfile.write(text)
	return str(guid)

def writeSolution(to_path, namelist, guidlist):
	solution = open(to_path + "/unit_tests.sln", "w")
	
	solution.write("\xef\xbb\xbf\n") # Specify MSVS 2005
	solution.write("Microsoft Visual Studio Solution File, Format Version 9.00\n")
	solution.write("# Visual Studio 2005\n")
	  
	# add the libfife-project
	solution.write("Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"libfife\", \"..\\fife.vcproj\", \"{96025707-5759-400D-80E5-A1E94C8A79A2}\"\n")
	solution.write("EndProject\n")
  
	# Add the test projects:
	for i in range(len(namelist)):
		solution.write("Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"" + namelist[i] + "\", \"" + namelist[i] + ".vcproj\", \"" + guidlist[i] + "\"\n")
		solution.write("	ProjectSection(ProjectDependencies) = postProject\n")
		solution.write("		{96025707-5759-400D-80E5-A1E94C8A79A2} = {96025707-5759-400D-80E5-A1E94C8A79A2}\n")
		solution.write("	EndProjectSection\n")
		solution.write("EndProject\n")

	solution.write("Global\n")
	solution.write("	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n")
	solution.write("		Debug_static|Win32 = Debug_static|Win32\n")
	solution.write("		Release_static|Win32 = Release_static|Win32\n")
	solution.write("	EndGlobalSection\n")
	solution.write("	GlobalSection(ProjectConfigurationPlatforms) = postSolution\n")

	solution.write("		{96025707-5759-400D-80E5-A1E94C8A79A2}.Debug_static|Win32.ActiveCfg = Debug_static|Win32\n")
	solution.write("		{96025707-5759-400D-80E5-A1E94C8A79A2}.Debug_static|Win32.Build.0 = Debug_static|Win32\n")
	solution.write("		{96025707-5759-400D-80E5-A1E94C8A79A2}.Release_static|Win32.ActiveCfg = Release_static|Win32\n")
	solution.write("		{96025707-5759-400D-80E5-A1E94C8A79A2}.Release_static|Win32.Build.0 = Release_static|Win32\n")
  
	for guid in guidlist:
		solution.write("		" + guid + ".Debug|Win32.ActiveCfg = Debug_static|Win32\n")
		solution.write("		" + guid + ".Debug|Win32.Build.0 = Debug_static|Win32\n")
		solution.write("		" + guid + ".Release|Win32.ActiveCfg = Release_static|Win32\n")
		solution.write("		" + guid + ".Release|Win32.Build.0 = Release_static|Win32\n")

	solution.write("	EndGlobalSection\n")
	solution.write("	GlobalSection(SolutionProperties) = preSolution\n")
	solution.write("		HideSolutionNode = FALSE\n")
	solution.write("	EndGlobalSection\n")
	solution.write("EndGlobal\n")

# call main function
main ("../../../../tests/core_tests", "./unit_tests")
