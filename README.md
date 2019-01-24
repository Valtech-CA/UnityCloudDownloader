# Unity Cloud Downloader

Unity Cloud Downloader is a utility that helps you keep your cloud builds synchronized on disk. Through a simple interface, navigate through your projects and different build configurations. Set the configurations that interest you to synchronize automatically and manage the number of builds to keep on disk as well as the number of days to keep them. If you need an older build for testing or don't want automatic syncrhonization, just find the builds you want and select them for manual download.

On platforms that support it, notifications messages are provided when a new build has been downloaded. Clicking on them will instantly navigate to the build's location.

The windows installer will setup the application to start on login. If you don't want this behaviour, simply uncheck the option during installation.

# Getting Started

Start by creating a new profile. Give it a name, choose where you want builds to download and provide your [Unity Cloud API Key](https://developer.cloud.unity3d.com/login/me/) . The field will become green if the authentification is successful or red if there is an error. Once your profile is created, you can now see all the projects that are associated with your user. Projects are displayed with an icon (if available) the name of the project and an optional marker which is shown for projects on which at least one configuration has automatic synchronization.

Each project can have a number of build configuration, weither it be UAT build, a production build, or an iOS and an Android build. Each configuration can be configured independenlty. Start by enabling synchronization on a configuration to see a set of options appear under that configuration. These options let you decide how many builds to keep on disk.
* Max specifies the maximum number of builds to keep on disk. No more than that number of builds will be syncrhonized on disk at the same time.
* Days specified how many days to keep a build, after which it will potentially be removed from disk.
* Min specifies the minimum number of builds to keep when builds go out of date. This way, you can keep a desired minimum number of builds on disk, even if they are older than the number of days you had specified to keep them.

Note that builds can also be manually downloaded. Simply navigate into the list of builds and click on the download icon to queu the build for downloads. Such manually downlaoded builds are not deleted by the synchronizations system. This means that they will stay on disk whether they are out of date or not and will also not count towards your specified maximum and minimum number of synchronized builds.

The list of builds is not just for selecting manual downloads. It also provides information on current and past builds. Builds are displayed with their build number, the name of the build configuration, an action button (if available) a status icon, the date and time the build was submitted, and the build size (if the build was successful). The action button on downloaded builds will open their folder location. The build status icon will display all possible build status from queued, to failed. It can be used to see which new build have gone from queued to in-progress.

Finally, to not consume system resources, Unity Cloud Downloader spends most of its time sleeping. It only checks for changes once every two minutes. If you would ever want to force the application to check for changes, use the circling arrows at the bottom of the window or the *Refresh* option in the system tray.

# Development Information

This application was developped with Qt 5.10+ with a windows installer developped with WiX Toolset. A Mac installer is created through QMake.

The application is divided into two projects, one for the business logic and the other for the views. The core library contains database manipulation classes, view models and the syncrhonization worker. The desktop application provides all the views to display and interact with the core's models as well as the tray icon components and a **Single Instance Guard**. This guard not only prevents multiple runs of the same application, it turns attemps to run other instances into a signal that is used in this case to show the main window of the application.

Dependency injection is done with a **Service Locator**. It provides access to a pure virtual database provider and an abstract synchronizer. There is also a helper function that gets the default database from the provider, however, note that this operation is reserved to the UI thread.

# Scripts

 -  **setup-buildenv.ps1** is a powershell script that will install Chocolatey, Visual Studio, the WiX Toolset, and some other build components. Qt still needs to be installed manually afterwards.
- **build.ps1** is a powershell script that will build the application and the installer. You can pass it an argument to specify the build configuration (e.g. *Release* or *Degbug* which is the default).
- **append-git-revision.ps1** is a simple powershell script that will rename the msi installer to append the short git revision.

**Note:** Before you can run any powershell script, you need to set your execution policy to *Unrestricted* by running `Set-ExecutionPolicy Unrestricted` from an elevated PowerShell.