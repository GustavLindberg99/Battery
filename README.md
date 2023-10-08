# Battery
This is a Windows program that warns you when your battery is almost full.

## Setup

The easiest way to install the program is to download and run [this setup program](https://raw.githubusercontent.com/GustavLindberg99/Battery/main/Battery-install.exe). This only installs the program for the current user. Alternatively, if you prefer to download the files manually, you can download and extract [this ZIP folder](https://raw.githubusercontent.com/GustavLindberg99/Battery/main/Battery-portable.zip) with the executable and the necessary DLLs.

## Usage

When you start this program, it runs in the background, so no window will open. You can see that it's running by seeing its icon in the taskbar:

<img width="302" src="https://user-images.githubusercontent.com/95423695/208722917-54dc00e6-0569-41d0-8665-8b84dd0ef20d.png">

By default, this program will show a notification when your battery is charged at 80% or higher (you can change this in the settings, see below).

### Settings

To open the settings window, right click on the icon in the taskbar shown in the screenshot above, and choose Settings. There you can choose if you want the program to warn you when your battery is almost more charged than a certain level, or less charged than a certain level, or both.

To increase your battery lifetime, you can set the program to warn you when your battery is charged at 80% (the default setting) so that you can unplug it then.

### Run this program at startup

If you installed this program using the installation program, it should start automatically whenever you start Windows. If you *don't* want this for some reason, you can open the task manager (by pressing <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>Esc</kbd>), go to the Startup tab, right click Battery and choose Disable.

If Battery isn't starting whenever you start Windows (which could be the case if you installed it from the ZIP folder), create a shortcut to it in `%appdata%\Microsoft\Windows\Start Menu\Programs\Startup`. If you've done this and it's still not starting on startup, open the task manager as described in the previous paragraph and make sure it's enabled.
