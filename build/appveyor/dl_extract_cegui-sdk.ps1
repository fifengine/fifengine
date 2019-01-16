#based on https://www.appveyor.com/docs/api/samples/download-artifacts-ps/

$apiUrl = 'https://ci.appveyor.com/api'
$downloadLocation = 'C:\projects\fifengine-dependencies\downloads'
$extractLocation = 'C:\projects\fifengine-dependencies\extracted'
$installLocation = 'C:\projects\fifengine-dependencies\includes'
$accountName = 'cegui-ci'
$projectSlug = 'cegui'
$branch = "v0-8"

$cegui_dlls = ("CEGUIBase-0.dll", "freetype.dll", "pcre.dll", "minizip.dll", "CEGUIOpenGLRenderer-0.dll", "glew.dll", `
"CEGUICommonDialogs-0.dll", "CEGUICoreWindowRendererSet.dll", "CEGUISillyImageCodec.dll", "silly.dll", "libpng.dll", "jpeg.dll", "CEGUIExpatParser.dll", "libexpat.dll")
$global:dll_excludes = $global:dll_excludes + ("zlib.dll", "CEGUICommonDialogs-0.dll", "CEGUICoreWindowRendererSet.dll", "CEGUISillyImageCodec.dll", "silly.dll",`
"jpeg.dll", "CEGUIExpatParser.dll", "libexpat.dll")
# get project with last build details
$project = Invoke-RestMethod -Method Get -Uri "$apiUrl/projects/$accountName/$projectSlug/branch/$branch"

# Get the MSVC2015 job id
ForEach ($job in $project.build.jobs) {if ($job.name.Contains("TOOLCHAIN=msvc2015")) {$jobId = $job.jobid; break}}

# get job artifacts (just to see what we've got)
$artifacts = Invoke-RestMethod -Method Get -Uri "$apiUrl/buildjobs/$jobId/artifacts"

# here we just take the first artifact, but you could specify its file name
$artifactFileName = $artifacts[0].fileName
$localartifactFileName = [System.IO.Path]::GetFileName("$artifactFileName")

# artifact will be downloaded as
$localArtifactPath = "$downloadLocation\$localartifactFileName" 

# download artifact
# -OutFile - is local file name where artifact will be downloaded into
# the Headers in this call should only contain the bearer token, and no Content-type, otherwise it will fail!
Invoke-RestMethod -Method Get -Uri "$apiUrl/buildjobs/$jobId/artifacts/$artifactFileName" `
-OutFile $localArtifactPath

$folder_name = [System.IO.Path]::GetFileNameWithoutExtension("$artifactFileName")
$extract_dir = "$extractLocation"
7z x $localArtifactPath -o"$extract_dir" -y
copy-item $extractLocation/$folder_name/bin/* $installLocation\bin -force -recurse -Confirm:$False -Include $cegui_dlls
copy-item $extractLocation/$folder_name/lib/* $installLocation\lib -force -recurse -Confirm:$False -Exclude "tinyxml*.*"
copy-item $extractLocation/$folder_name/include/* $installLocation\include -force -recurse -Confirm:$False -Exclude ("tinyxml.h", "tinystr.h")
