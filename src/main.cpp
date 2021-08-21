//Based on the code from: https://www.geeksforgeeks.org/making-linux-shell-c/ (created by Suprotik Dey)

//This code is created by Ola Kr. Hoff
//g++ main.cpp -std=c++17 -o main -ledit -pthreads

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <readline/history.h>

#include "directComP2P/p2p.cpp"

#define MAX_NO_INPUT_CHAR 1000 //Maximum number of characters in a single input
#define MAX_NO_ARGS 10 //Maximum number of command arguments in a single input

void init_shell();
void printDir();
u_int8_t getInput(char* str);
void processCommand(char* input, char** commands);
void removeSpace(char* inputString, char** output);
int commandHandler(char** commandList);
void openHelp();
void executeArgs(char** args);

int main()
{
    char inputString[MAX_NO_INPUT_CHAR], *parsedArgs[MAX_NO_ARGS];

    init_shell();
  
    while (true)
    {    
        //Print the new line indicator
        printDir();
        
        //Get input from user
        if (getInput(inputString))
            continue; //If there is no input continue loop
        
        processCommand(inputString, parsedArgs);
    }
    return 0;
}

//Shows the "Welcome"-message to the
//user upon starting the program.
void init_shell()
{
    const char* line = "\n\n\n------------------------------------------";
    printf("%s", line);
    printf("\n\n\n\t-----  ShellLink  -----");
    printf("%s", line);
    char* username = getenv("USER");
    printf("\n\n\nHello, %s\n", username);
}

//This method printds the current working directory
void printDir()
{
    char currentDirectory[1024];
    getcwd(currentDirectory, sizeof(currentDirectory)); //Gets current working directory
    printf("\n%s", currentDirectory); //Prints the cwd
}

//Returns 1 if there is no input, else 0
u_int8_t getInput(char* str)
{
    char* buffer = readline("\x1b[34m$-> \x1b[0m"); //Changing colour on the text: https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
    
    if (strlen(buffer) != 0) //Checking that there is something tín the buffer
    {
        add_history(buffer); //Adds the string to the history, so user can use the arrow keys to go back to previous commands faster
        strcpy(str, buffer); //Copies the command into the inputString vairable in main
        return 0;
    }
    return 1; //Buffer is empty
}

//The method processes the input command from the user.
//It returns 0 if the command was internal to the system and was executed, else 1 + piped (1 if there was a pipe, else 0)
void processCommand(char* input, char** commands)
{   
    removeSpace(input, commands); //Parse the input for spaces
    commandHandler(commands); //Handles which command to execute
}

//Removes the spaces and adds the seperate commands to the list of commands.
void removeSpace(char* inputString, char** commands)
{
    for (u_int8_t i = 0; i < MAX_NO_ARGS; i++)
    {
        commands[i] = strsep(&inputString, " ");
        
        if (commands[i] == NULL)
            break; //End of string reached
        if (strlen(commands[i]) == 0)
            i--; //Reapeting spaces in the command, not a valid command
    }
}

//Handles what to do with the given command, returns 0 if the command is invalid, else 1.
int commandHandler(char** commandList)
{
    const int NO_OF_COMMANDS {6};
    int switchCommand {-1};
    char* commands[NO_OF_COMMANDS];
  
    //------- List of own commands start -------

    commands[0] = (char*)"exit";
    commands[1] = (char*)"cd";
    commands[2] = (char*)"help";
    commands[3] = (char*)"test";
    commands[4] = (char*)"ls";
    commands[5] = (char*)"talk";


    //-------- List of own commands end --------
  
    for (u_int8_t i = 0; i < NO_OF_COMMANDS; i++)
    {
        if (strcmp(commandList[0], commands[i]) == 0) //The strings are equal
        {
            switchCommand = i; //Sets which command is used
            break;
        }
    }
  
    switch (switchCommand)
    {
        case 0: //exit
            printf("\nGoodbye\n");
            exit(0);
        case 1: //cd
            chdir(commandList[1]);
            return 1;
        case 2: //help
            openHelp();
            return 1;
        case 3: //test
        {
            printf("\nHello %s.\nPlay around and have "
                "fun, but do so at your own risk."
                "\nUse 'help' to get some badly documented info about the program. :)\n",
                getenv("USER"));
            return 1;
        }
        case 4: //ls
            executeArgs(commandList);
            return 1;
        case 5: //talk
        {
            P2P talk = P2P();

            talk.start();

            return 1;
        }
        default:
            printf("\nThe command was not recognized, use 'help' to see which commands are supported.\n");
            return 0;
    }
}

//Shows the "help" menu
void openHelp()
{
    printf("\nShellLink - HELP"
        "\nList of Commands supported:"
        "\n>cd"
        "\n>exit"
        "\n>test"
        "\n>ls"
        "\n>talk");
}

//Method creates a child process which executes the command
//arguments pass in, parent waits for child to finnish
void executeArgs(char** args)
{
    pid_t pid = fork(); //Creating child process
  
    if (pid == -1) //Creating child failed
    {
        printf("\nSomething went wrong");
    }
    else if (pid == 0) //This is the child process
    {
        if (execvp(args[0], args) < 0)  //Child executes the work
        {
            printf("\nThe command could not be executed"); //Child failed to execute work
        }
        exit(0); //When the child is finnished it exits.
    }
    else //This is the parent process
    {
        wait(&pid); //The parent waits for the child to finnish
    }
}