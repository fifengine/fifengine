#based on https://www.appveyor.com/docs/api/samples/download-artifacts-ps/
$python_Version = $args[0]
if($python_Version -eq $null)
{
    echo "Usage: dl_extract_boost_python.ps1 python_version"
    exit
}
$apiUrl = 'https://ci.appveyor.com/api'
$accountName = 'beliaar'
$projectSlug = 'boost-appveyor'

# get project with last build details
$project = Invoke-RestMethod -Method Get -Uri "$apiUrl/projects/$accountName/$projectSlug"

# Get the MSVC2015 job id
ForEach ($job in $project.build.jobs) {if ($job.name.Contains("PYTHON_VERSION=$python_Version")) {$jobId = $job.jobid; break}}

# get job artifacts (just to see what we've got)
$artifacts = Invoke-RestMethod -Method Get -Uri "$apiUrl/buildjobs/$jobId/artifacts"

# here we just take the first artifact, but you could specify its file name
$artifactFileName = $artifacts[0].fileName
$localartifactFileName = [System.IO.Path]::GetFileName("$artifactFileName")

# artifact will be downloaded as
$localArtifactPath = "$env:downloadLocation/$localartifactFileName" 

# download artifact
# -OutFile - is local file name where artifact will be downloaded into
Invoke-RestMethod -Method Get -Uri "$apiUrl/buildjobs/$jobId/artifacts/$artifactFileName" -OutFile $localArtifactPath

$folder_name = [System.IO.Path]::GetFileNameWithoutExtension("$artifactFileName")
$extract_dir = "$env:downloadLocation/$folder_name"
7z x $localArtifactPath -o"$extract_dir"

#Thanks to TheMadTechnician (http://stackoverflow.com/questions/28843448/powershell-getting-foldername-like-and-get-the-first-subdirectory)
Function GCI-ToDepth{
Param(
    $Path = $PWD.Path,
    $Filter = "*",
    $Depth = 255
    )

    $Paths = 1..$Depth|ForEach{"{0}{1}" -f $Path.trimend('\'), ("\*" * $_)}
    Get-ChildItem -Path $Paths -Filter $Filter
}
$include_dir = GCI-ToDepth -Path "$env:downloadLocation/$folder_name/include" -Depth 1

$env:BOOST_LIBRARYDIR = "$env:downloadLocation/$folder_name/lib"
$env:BOOST_INCLUDEDIR = "$include_dir"
