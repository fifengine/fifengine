function createTargetFolder(folder) {
  var fso = new ActiveXObject("Scripting.FileSystemObject");
  if (!fso.FolderExists(folder))
    fso.CreateFolder(folder);
}

function makeGuid() {
  return (new ActiveXObject("Scriptlet.TypeLib")).Guid.substr(0, 38);
}

function writeProject(name) {
  var fso, guid, template, project, text;
  
  fso = new ActiveXObject("Scripting.FileSystemObject");
  guid = makeGuid();
  
  template = fso.OpenTextFile("test_vcproj_template.xml", 1);
  project = fso.CreateTextFile("unit_tests/" + name + ".vcproj");
  
  text = template.ReadAll();
  text = text.replace(/__INSERT_NAME_HERE__/g, name);
  text = text.replace(/__INSERT_GUID_HERE__/g, guid);
  
  project.WriteLine(text);
  
  return guid;
}

function writeSolution(nameList, guidList) {
  var fso, solution;
  
  fso = new ActiveXObject("Scripting.FileSystemObject");
  solution = fso.CreateTextFile("unit_tests/unit_tests.sln");
  
  solution.WriteLine("\xef\xbb\xbf"); // Specify MSVS 2005
  solution.WriteLine("Microsoft Visual Studio Solution File, Format Version 9.00");
  solution.WriteLine("# Visual Studio 2005");
  
  for (var i in nameList) {
    solution.WriteLine("Project(\"" + guidList[i] + "\") = \"" + nameList[i] + "\", \"" + nameList[i] + ".vcproj\", \"{B6321146-2FA6-4295-B1F8-41045197C770}\"");
    solution.WriteLine("EndProject");
  }
  
  solution.WriteLine("Global");
  solution.WriteLine("	GlobalSection(SolutionConfigurationPlatforms) = preSolution");
  solution.WriteLine("		Debug|Win32 = Debug|Win32");
  solution.WriteLine("		Release|Win32 = Release|Win32");
  solution.WriteLine("	EndGlobalSection");
  solution.WriteLine("	GlobalSection(ProjectConfigurationPlatforms) = postSolution");
  solution.WriteLine("		{B6321146-2FA6-4295-B1F8-41045197C770}.Debug|Win32.ActiveCfg = Debug|Win32");
  solution.WriteLine("		{B6321146-2FA6-4295-B1F8-41045197C770}.Debug|Win32.Build.0 = Debug|Win32");
  solution.WriteLine("		{B6321146-2FA6-4295-B1F8-41045197C770}.Release|Win32.ActiveCfg = Release|Win32");
  solution.WriteLine("		{B6321146-2FA6-4295-B1F8-41045197C770}.Release|Win32.Build.0 = Release|Win32");
  solution.WriteLine("	EndGlobalSection");
  solution.WriteLine("	GlobalSection(SolutionProperties) = preSolution");
  solution.WriteLine("		HideSolutionNode = FALSE");
  solution.WriteLine("	EndGlobalSection");
  solution.WriteLine("EndGlobal");
}

function nameFromPath(fileName) {
  return fileName.split(".")[0];
}

function makeNameList(folder) {
  var fso, e, file;
  var result = new Array();
  fso = new ActiveXObject("Scripting.FileSystemObject");
  e = new Enumerator(fso.GetFolder(folder).files);
  for (e.moveFirst(); ! e.atEnd(); e.moveNext()) {
    file = e.item();
    if (fso.GetExtensionName(file.Path) == "cpp")
      result.push(nameFromPath(file.Name));
  }
  return result;
}

function mainFunction(from, to) {
  var nameList = makeNameList(from);
  var guidList = new Array();
  
  createTargetFolder(to);
  for (var i in nameList)
    guidList.push(writeProject(nameList[i]));
  writeSolution(nameList, guidList);
}

mainFunction("../../../tests/unit_tests", "unit_tests");

