# Description
**main.c :**
Main code for the shell which calls appropriate functions for printing prompts and executing commands.

**shellprofile**
File that stores details related to the shell program (similar to environment variables)run like home path, the shell process ID, last warp'd directory, and the last run foreground command (if it takes more than 2 sec).

**shellhistory**
File that stores the number and commands associated with pastevents. Similar to (bash_history or zsh_history).

**shellprocesses**
File used to store the pid and name of background running processes (spawned by the shell only).

**setup.c**
Code to initialize all files `shellprofile`, `shellhistory` and `shellprocesses` when the shell is started.

**execute.c**
Conatains functions for parsing the input, executing the commands, saving background processes and handling errors.

**dircontrol.c**
Contains functions pertaining to tasks related to absolute and relative path management used by commands. It also contains functions to save and retrive the last warp'd path.

**prompt.c**
Code to print the shell prompt. Prints the username, systemname, pwd and the last process (if it took > 2 sec for exection)

**warp.c** 
Contains code for the command `warp` (analogous to `cd`), and contains functions to change path both absolutely and relatively.

**peek.c** 
Contains code for the command `peek` (analogous to `ls`), which is used to print details about the files and folders in the input directory.

**seek.c** 
Contains code for the command `seek` which is used to search for files and directoried in the given folder.

**pastevents.c** 
Contains code for the command `pastevents` (analogous to `history`), and contains functions to add or retrive inputs or remove all inputs from the history file.

**proclore.c**
Contains code for the command `proclore` which prints detials about the given pid or the shell process.

**colors.h**
Contains definitions of various colors used in formatting the output in cases like error and warning messages.

**headers.h** (along with all other header .h files) contians codes which include other standard libraries as well as contain function definitions for the all the functions used in the project.

# Assumptions
1. All path lengths, text, any string or file size in general has been assumed to be **maximum 4096** characters.
2. `home` is the only global variable declared which stores the absolute path of the shell's home directory.
3. The last process printed in the prompt is considered to be the last foreground exectuing command in the input (eg. C in input "A ; B & C ;"), given it takes > 2 sec for execution.
4. History file contians the formatted and modified input. An input is not added if it is similar to the previous one. Invalid commands are also added. Commands containing pastevents are translated to their corresponding commands and added. Pastevents execute out of range number return an empty string.
5. Block size for `peek -l` is 4096 bytes. 
6. All details for proclore are read from the `/proc/pid/status` file and the executatble link is taken from the `/proc/pid/exe` symlink. A process is assumed to be foreground if the group id is same as the standard terminal group id.
7. For `seek`, all items which contain the searchString as the prefix are considered. Not giving any flags searches for both files and direcotries but giving `-fd` is considered to be invalid
8. Space Delimiters - ` \n\t\r\b\f\0` and whitespace
9. Input Delimiters - `;&|><\n`. Only `&` runs the process in the background and all others run it in the foreground (for now in Part A)
10. Foreground process prints time taken by the last command in the sequence in the next prompt only if is greater than 2 sec. eg. `A & B; C` prints C as the command in the next prompt only if C takes greater than 2 sec.
11. Background process which write to stdout give unexpected behaviour as they can print the output before the input for the next prompt is given. (To be handled with pipes in part B)
12. Number of background processes function does not replicate what bash does. It only calculates the number of background processes whose exit message is not yet displayed.
