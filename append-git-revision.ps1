$git = "C:/Program Files/Git/bin/git.exe"
$revision = & $git rev-parse --short HEAD
Get-ChildItem -Recurse *.msi | Rename-Item -NewName { $_.name -Replace '\.msi$', "_$revision.msi" }

echo "added git revision, $revision, to msi file"
