# Linux Shell
## Prerequisite

The GNU Readline library must be installed in the System to compile the Shell Code. It can be installed using the following command:

```sudo apt-get install libreadline6 libreadline6-dev```

After installation, shell can be compiled as follow:

```gcc shell.c –lreadline```

## Supported Features

### History:
This shell supports following history feature:
1.	‘history’ command to view history.
2.	Up and Down arrow to navigate through history.
3.	‘!!’ command to execute last command.
4.	‘!n’ command to execute nth command in the history from the beginning.
5.	‘!-n’ command to execute nth command in the history from the present.
### Autocomplete:
Tab key can be used to autocomplete a filename.
### Process Handling and Job Control:
Any ongoing foreground process can be sent to background or killed and any background or stopped process can be started and brought to foreground. 
Any foreground process can be killed using ‘ctrl+c’. Also, any foreground process can be stopped using the ‘ctrl+z’. A process thus stopped is added to job queue.
Background Process: Any process can be made to run in background by passing the last argument as ‘&’ to the process.
Jobs: Jobs command can be used to see the list of all the jobs that are either stopped or running in the background.
fg: fg command can be used to bring to foreground any stopped or running process in job queue. A job number can be also provided as an optional argument. It bring the process associated with the job number to the foreground. If no argument is provided, then the latest job is fetched to front.
bg: bg command can be used to start any of the stopped process in the background. A job number can be also provided as an optional argument. It starts the process associated with the job number in the background. If no argument is provided, then the latest stopped job is started.
