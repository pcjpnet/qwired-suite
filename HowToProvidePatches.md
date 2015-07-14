# Introduction, or, "Why?" #

Making changes to existing source code is often a lot of work that is easily lost when a medium like the Internet to send the results to another person or developer. Sometimes the original files can be huge and nobody is sure of what changed in which way afterwards.

To ensure the best results when making changes to Qwired source code, your changes to the source files must be recorded into a so called _patch file_ or _diff file_. These _patch_ files contain just the information about what has been changed since the last time.

Wikipedia contains some more detailed information:
http://en.wikipedia.org/wiki/Patch_(computing)


# Sounds like more work than needed, why not just send the whole file? #

Technically this works, unfortunately there is often a situation where the target files changed independently, and just replacing the original file would result in loss of changes from other people.

Since the patch files contain detailed information about what, where and how it changed, those changes can be reversed and applied next to other patch files.


# Alright, alright, how do create a patch? #

Creating a patch is relatively easy. I assume that you have checked out the Qwired source code from SVN. SVN automatically tracks series of changes to files and directories (that's why it's called a version management system), and naturally allows you to generate patch files that can be sent to other developers.

If you use a SVN GUI (like SmartSVN) there normally is a "Create Patch..." command. Just be sure that you select the _trunk_ directory before executing that command!

If you have no GUI for SVN installed, you can easily do this by using Terminal:
  * Open a new Terminal
  * `cd /path/to/qwired-suite-read-only/`  _(navigate to the trunk directory)_
  * `svn status`  _(optional - this will show you which files changed)_
  * `svn diff > ~/Desktop/name_of_my_fix.diff`  _(write the patch to a file on the Desktop)_

As you can see, this is relatively easy. Please give the patch a useful name (for instance include the revision number of the source code - which you can find out with the `svnversion` command - and possibly a issue-ID.

Of course you can write the patch to another location - just be sure that the file has the _.diff_ suffix.

If you are new to creating patches, it might be interesting to take a quick look into the patch file and see how it contains only the changes and hints of your effort.

After sending the patch, or uploading it to a Google Code issue, you might want to reverse the changes to your source code and return it to a "factory-fresh state". Since SVN knows what was changed, you can simply call the following command:

```
svn revert -R .
```