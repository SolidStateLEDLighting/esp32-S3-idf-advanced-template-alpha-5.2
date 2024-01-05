## Remembering how to add a submodule

Submodules are particularly interesting for containing our components.   When components are modules, they are held in their own separate repository and can
be used in multiple projects.   Any update in one project is shared from that submodule to all projects.

1) Make sure the main project is already in Git under control. (the project we are adding a submodule to)
2) Explore the target directory (eg. components directory) and right mouse click "Git Bash here" to start a terminal.
3) Apply command $ git submodule add  https: // github.com / (account name) / (repository name).git