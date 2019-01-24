# this script will setup a computer with Choco, Msbuild, and Wix
# To run this script from powershell, you will first need to `Set-ExecutionPolicy Unrestricted`
# The default powershell settings limit executions to interactive shells only

# Get the ID and security principal of the current user account
$myWindowsID=[System.Security.Principal.WindowsIdentity]::GetCurrent()
$myWindowsPrincipal=new-object System.Security.Principal.WindowsPrincipal($myWindowsID)
 
# Get the security principal for the Administrator role
$adminRole=[System.Security.Principal.WindowsBuiltInRole]::Administrator
 
# Check to see if we are currently running "as Administrator"
if ($myWindowsPrincipal.IsInRole($adminRole))
{
   # We are running "as Administrator" - so change the title and background color to indicate this
   $Host.UI.RawUI.WindowTitle = $myInvocation.MyCommand.Definition + "(Elevated)"
   $Host.UI.RawUI.BackgroundColor = "DarkBlue"
   clear-host
}
else
{
    # We are not running "as Administrator" - so relaunch as administrator
   
    # Create a new process object that starts PowerShell
    $newProcess = new-object System.Diagnostics.ProcessStartInfo "PowerShell";
   
    # Specify the current script path and name as a parameter
    $newProcess.Arguments = $myInvocation.MyCommand.Definition;
   
    # Indicate that the process should be elevated
    $newProcess.Verb = "runas";
   
    # Start the new process
    [System.Diagnostics.Process]::Start($newProcess);
   
    # Exit from the current, unelevated, process
    exit
}

Set-ExecutionPolicy Unrestricted
# Install dot net 3.5
DISM /Online /Enable-Feature /FeatureName:NetFx3 /All /NoRestart /Quiet
# Install chocolatey
iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
RefreshEnv.cmd
# Install the microsoft build tools
choco install microsoft-build-tools -y
RefreshEnv.cmd
# Install the wix toolset (Windows Installer XML)
choco install wixtoolset -y
RefreshEnv.cmd
# Install the command line interface for NuGet
choco install NuGet.CommandLine -y
RefreshEnv.cmd
# Install the NuGet package provider
Install-PackageProvider NuGet -Force
# Install visual studio
Install-Module VSSetup -Scope AllUsers -Force
RefreshEnv.cmd
# Get info on visual studio
$vspath = Get-VSSetupInstance -All | Select-VSSetupInstance -Product 'Microsoft.VisualStudio.Product.BuildTools' -Latest | Select-Object -ExpandProperty InstallationPath
$vsinstaller = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vs_installer.exe"
# Install missing visual studio components (https://docs.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-community)
& $vsinstaller modify --installPath "$vspath" --quiet --add Microsoft.VisualStudio.Workload.NativeDesktop --add Microsoft.VisualStudio.Component.Nuget.BuildTools --add Microsoft.Net.Component.4.6.1.SDK --add Microsoft.Net.Component.4.6.1.TargetingPack --norestart --force
