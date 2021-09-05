param($p1)

$env:qmodName = "SearchFixes"
$env:module_id = "SearchFixes"

$zip = $env:qmodName + ".zip"
$qmod = $env:qmodName + ".qmod"

$mod = "./mod.json"
$modJson = Get-Content $mod -Raw | ConvertFrom-Json

$allLibs = Get-ChildItem ./libs/arm64-v8a/*.so | Select -Expand Name
$libs = @()

$fileList = @($mod)

$bannedLibList = @("modloader", "codegen")

$modlib = @()

foreach ($lib in $allLibs) 
{
    # ignore modloader
    $contains = 0
    foreach ($ban in $bannedLibList)
    {
        if ($lib.Contains($ban))
        {
            $contains = 1
            break
        }
    }

    if ($contains)
    {
        continue
    }
    # if its the actual mod lib
    else
    {
        if ($lib.Contains($env:module_id))
        {
            $path = "./libs/arm64-v8a/" + $lib;
            $fileList += ,$path;
            $modlib += ,$lib;
            $msg = "Adding " + $lib + " as the mod file"
            echo $msg
            continue
        }
    }
    $path = "./libs/arm64-v8a/" + $lib
    $fileList += ,$path
    $libs += ,$lib
    $msg = "Adding " + $lib + " as a lib file"
    echo $msg
}

# update version from qpm json
$qpm = "./qpm.json"
$qpmJson = Get-Content $qpm | ConvertFrom-Json 
$modJson.version = $qpmJson.info.version

# add the thing to the libs list because we don't need it as a mod file
$modJson.modFiles = $modlib
$modJson.libraryFiles = $libs
$modText = $modJson | ConvertTo-Json -Depth 50 -EscapeHandling EscapeNonAscii

Set-Content $mod $modText

# if the qmod exists, rename it to zip to update it, we'll rename it back later
if (Test-Path $qmod) 
{
    if ($p1 -and $p1.Contains("y"))
    {
        echo "param y passed, deleting previous existent qmod"
        remove-item $qmod
    }
    else
    {
        move-item -Force $qmod $zip
    }
}
$msg = "Creating qmod for module " + $env:module_id + " with name " + $qmod
echo $msg
Compress-Archive -Path $fileList -DestinationPath $zip -Update

& move-item -Force $zip $qmod