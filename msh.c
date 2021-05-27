// The MIT License (MIT)
//
// Copyright (c) 2016, 2017, 2021 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// 7f704d5f-9811-4b91-a918-57c1bb646b70
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/*

Name: Bryan Rodriguez
ID: 1001647248

*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

// We want to split our command line up into tokens
// so we need to define what delimits our tokens.
// In this case  white space
// will separate the tokens on our command line
#define WHITESPACE " \t\n"

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5 // Mav shell only supports five arguments

//simple function to display the pids
void showpids(int pids[], int numOfPids)
{
  int counter = 0;
  for (int i = numOfPids - 1; i >= 0; i--)
  {
    printf("%d: %d\n", counter, pids[i]);
    counter++;
  }
}

//simple function to display cmd history
void showHistory(char *history[], int counter)
{
  for (int i = 0; i < counter; i++)
  {
    printf("%d: %s\n", i, history[i]);
  }
}

//execvp function, this function is going to excute cmds like ls, mkdir, etc.
//all within the child process and returns the pid of the child when it is done
//executing the code
int execvpFun(char *token[], int tokenCount)
{

  //creating child process
  pid_t pid = fork();
  //entering child process
  if (pid == 0)
  {
    //storing the return value of execvp so that if it happens to fail, it will return
    //a -1 for error handling
    int ret = execvp(token[0], &token[0]);

    //failed to execute execvp, this code displays that the cmd cannot be found
    if (ret == -1)
    {
      for (int i = 0; i < tokenCount - 1; i++)
      {
        printf("%s ", token[i]);
      }
      printf(": Command not found\n");
    }
    fflush(NULL);
    exit(EXIT_SUCCESS);
  }
  else
  {
    int status;
    wait(&status);
  }
  //returns the process id of the child for pid array (pids)
  return pid;
}

int main()
{

  char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);

  //creating pid array and getting the parent pid
  int pids[15];
  int pidCounter = 1;
  pids[0] = getpid();

  //creating the history array and making space
  char *history[15];
  int historyCounter = 0;
  for (int i = 0; i < 15; i++)
  {
    history[i] = malloc(sizeof(char) * MAX_COMMAND_SIZE);
  }

  while (1)
  {

    // Print out the msh prompt
    printf("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin));

    //exits program if cmd_str is equal to exit or quit
    if ((strcmp(cmd_str, "exit\n") == 0) || (strcmp(cmd_str, "quit\n") == 0))
    {
      exit(0);
    }

    /*this block of code checks for the char '!' and and removes it using
      memmove, next atoi to convert it to an interger and then referneces the history
      array to overwrite the cmd_str. all making sure it is within the bounds
      of the history counter. Allows to run past cmd with the short cut !##*/
    if (cmd_str[0] == '!')
    {
      memmove(cmd_str, cmd_str + 1, strlen(cmd_str));
      int inputNum = atoi(cmd_str);

      if (inputNum < historyCounter && inputNum >= 0)
      {
        strcpy(cmd_str, history[inputNum]);
      }
      else
      {
        printf("Command not in history.\n");
        fflush(NULL);
      }
    }

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;

    char *working_str = strdup(cmd_str);

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input strings with whitespace used as the delimiter
    while (((argument_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
           (token_count < MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
      if (strlen(token[token_count]) == 0)
      {
        token[token_count] = NULL;
      }
      token_count++;
    }

    //avoiding seg fault by checking if the first token is null, if not then
    //continue running the code
    if (token[0] != NULL)
    {

      /*this if/else statement controls how the history is stored
        first we check if the historycounter is at max size,
        if not then we continue to create a copy of cmd_str 
        (cmdStrCpy) and remove "\n" with strcspn and finally
        copy it to the history array. 
        if the history array is at max capacity then we will
        shift every element to the left to make space for the
        new cmd_str and then finally copy the new cmd and the end
        of the history array
        The intent of this code was to avoid a seg fault from passing
        15 history commands*/
      char *cmdStrCpy = (char *)malloc(MAX_COMMAND_SIZE);
      if (historyCounter == 15)
      {
        for (int i = 0; i < 14; i++)
        {
          strcpy(history[i], history[i + 1]);
        }
        strcpy(cmdStrCpy, cmd_str);
        cmdStrCpy[strcspn(cmdStrCpy, "\n")] = 0;
        strcpy(history[14], cmdStrCpy);
      }
      else
      {

        strcpy(cmdStrCpy, cmd_str);
        cmdStrCpy[strcspn(cmdStrCpy, "\n")] = 0;
        strcpy(history[historyCounter++], cmdStrCpy);
      }

      //a 'menu' for the history/showpid/execvp/cd
      if (strcmp(token[0], "cd") == 0)
      {
        if (chdir(token[1]) != 0)
          perror("Error");
      }
      else if (strcmp(token[0], "showpids") == 0)
      {
        showpids(pids, pidCounter);
      }
      else if (strcmp(token[0], "history") == 0)
      {
        showHistory(history, historyCounter);
      }
      else
      {
        /*this if/else statement controls how the pids is stored.
        first we check if the pidCounter is at max size,
        if not then we continue to store the pid in pids. 
        if the pids array is at max capacity then we will
        shift every element to the left to make space for the
        new pid and then finally place the new pid at the end
        of the history array
        The intent of this code was to avoid a seg fault from passing
        15 pids
        since execvpFun returns the child pid, I store that to array pids
        to keep track of the past pids and increment pidCounter
        */
        if (pidCounter == 15)
        {
          for (int i = 0; i < 14; i++)
          {
            pids[i] = pids[i + 1];
          }
          pids[14] = execvpFun(token, token_count);
        }
        else
        {
          pids[pidCounter++] = execvpFun(token, token_count);
        }
      }
    }

    free(working_root);
  }
  return 0;
}