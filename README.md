# HNxVoiceCommand
My voice command utility for Windows

This was my first time coding so there are many mistakes.

# How to use
Build it and go.

Does not require an internet connection.

Add phrases with the add button, then say "computer", then the command.

the commandline is executed with ShellExecute when it detects the correct phrase(s).

Requires SAPI 5.4, which can be downloaded from Microsoft if not already present.

Microsoft removed sapi in later versions of windows, you might have to track down the sapi runtime manually.
