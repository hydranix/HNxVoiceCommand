# HNxVoiceCommand
My voice command utility for Windows

Beware, this project had some unfinished bits to it.

It requires Visual Studio 2015 (community or better) and Qt 5.6 or later to build. I may have also used APIs only available in Windows 10 1511 or later.


# How to use
Once built, you should be able to just launch it.

You'll be greeted by a boring dialog with some buttons.

When you add a command a "phrase" is any word, or combination of words, that you will say to trigger an action.
Phrases can be any reasonable length.

Upon detection of the phrase, the program will attempt to execute whatever you set for the [program] and with the [arguments] you set, if any.

Intenally, it just passes the strings off to ShellExecute(), so make sure you understand what can and can't be done with ShellExecute(), and how you can extend the functionality of the commands with the registry.
https://msdn.microsoft.com/en-us/library/windows/desktop/bb776886.aspx

Finally, all commands are saved on exit and reloaded upon launch.
