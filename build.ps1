Param
(
    [parameter(Position=0)]
    [String]
    $config = "Debug"
)

clear
$vspath = Get-VSSetupInstance | Select-Object -Last 1 -ExpandProperty InstallationPath
$msbuild = "$vspath\MSBuild\15.0\Bin\MSBuild.exe"
$dotnetpath = $([System.Runtime.InteropServices.RuntimeEnvironment]::GetRuntimeDirectory())
$scriptDir = Split-Path $script:MyInvocation.MyCommand.Path
$solution = "$scriptDir/Installer/Installer.sln"

# Find the latest version of Qt installed
$qtversion = (Get-ChildItem -Path "$Env:SystemDrive\Qt" -Directory -Filter '5.*' | Sort | Select -Last 1).Name
$qtpath = "$Env:SystemDrive\Qt\$qtversion\msvc2017_64\bin"
$Env:Path += ";$qtpath"
$qmake = "$qtpath\qmake.exe"

pushd $scriptDir
If (!(test-path tmp))
{
	New-Item -ItemType Directory -Force -Path tmp
}
pushd tmp

pushd "$vspath\VC\Auxiliary\Build\"
cmd /c "vcvarsall.bat amd64 & set" |
foreach {
  if ($_ -match "=") {
    $v = $_.split("="); set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
  }
}
popd

& $qmake ../
nmake
popd
& $msbuild "$solution" /t:rebuild /p:Configuration=$config
popd
