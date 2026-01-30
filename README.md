> [!NOTE]
> This program requires runtimes that Microsoft retired after Windows 7.
> As such, this program will not build on modern version of Windows. It will be kept as an example of my first try at C++ many years ago. There are many mistakes.


# HNxVoiceCommand
My voice command utility for Windows

# How to use
Requires Windows 7 SDK and ATL to build.

Does not require an internet connection.

Add phrases with the add button, then say "computer", then the command.
the commandline is executed with ShellExecute when it detects the correct phrase(s).

Only runs on Windows 7.

~~Microsoft removed sapi in later versions of windows, you might have to track down the sapi runtime manually.~~
