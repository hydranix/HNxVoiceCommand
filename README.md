

# HNxVoiceCommand
My voice command utility for Windows

Beware, this project had some unfinished bits to it.

It requires Visual Studio 2015 (community or better) and Qt 5.6 or later to build. I may have also used APIs only available in Windows 10 1511 or later.


# How to use
Once built, you should be able to just launch it.

You'll be greeted by a boring dialog with some buttons.

When you add a command a "phrase" is any word, or combination of words, that you will say to trigger an action.
Phrases can be any reasonable length.

First you say a "hotword" similar to 'OK-Google', at this time the default is 'computer'. When it detects the hotword, it will then begin listening for commands. If no commonds are deected within 5 seconds, it goes back to only listening for the hotword.

Upon detection of the command, the program will attempt to execute whatever you set for the [program] and with the [arguments] you set, if any.

Intenrally, it just passes the strings off to ShellExecute(), so make sure you understand what can and can't be done with ShellExecute(), and how you can extend the functionality of the commands with the registry.
https://msdn.microsoft.com/en-us/library/windows/desktop/bb776886.aspx

Finally, all commands are saved on exit and reloaded upon launch.

-Feb 2017-
I'm working on an overhaul of this project and will update both the code and this poorly written readme when finished
