#define maxPidSize 1000
#define maxFileNameSize 1000
#define maxClockTicks 100
#define maxCommandSize 1000
#define maxArguments 1000

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

char *checkForOutputFile(char *rem, int *p);
void runCommand(char *command, int pipingType, int fileDescp);
void checkCPUPercentage(char pId[maxPidSize], char outputFile[maxPidSize], int isAppendOutput, int pipintType, int fileDescpPiping);
void checkresidentmemory(char pId[maxPidSize], char outputFile[maxPidSize], int isAppend, int pipintType, int fileDescpPiping);
void listFiles();
void sortFile(char fileName[maxFileNameSize], char outputFile[maxPidSize], int isAppend, int pipintType, int fileDescpPiping);
void executeCommand(char fileName[maxFileNameSize]);
void twoParallelCommand(char parallelCommand[]);
void pipeCommand(char pipeCommand[]);
void stringtokenizer(char *command, char *command1, char *command2, char delim);
char *stringTok(char *rem, char delim, char **remaining);

char *stringTok(char *rem, char delim, char **remaining)
{
    char *temp = rem;
    while (*temp != '\0' && *temp != delim)
    {
        temp = temp + 1;
    }
    while (*temp == delim)
    {
        *temp = '\0';
        temp = temp + 1;
    }
    *remaining = temp;
    if (strlen(rem) == 0)
    {
        return NULL;
    }
    return rem;
}
void stringtokenizer(char *command, char *command1, char *command2, char delim)
{
    char *temp;
    int i = 0;
    int k = 0;
    while (command[i] != delim)
    {
        command1[i] = command[i];
        i++;
    }
    command1[i] = '\0';
    k = i;
    command = command + k + 1;
    while (command1[--i] == ' ')
    {
        command1[i] = '\0';
    }
    while (*command == ' ')
    {
        command = command + 1;
    }
    for (int p = 0; p < strlen(command); p++)
    {
        command2[p] = command[p];
    }
}
char *checkForOutputFile(char *rem, int *p)
{
    char *temp = rem;

    char *dGreater = strstr(temp, ">>");
    char *greater = strstr(temp, ">");
    if (dGreater != NULL)
    {
        *p = 1;
        temp = dGreater;
        dGreater += 2;
        if (*dGreater != ' ')
        {
            return dGreater;
        }
        stringTok(temp, ' ', &temp);
        return temp;
    }
    else if (greater != NULL)
    {
        temp = greater;
        greater += 1;
        if (*greater != ' ')
        {
            return greater;
        }
        stringTok(temp, ' ', &temp);
        return temp;
    }
    else
    {
        return NULL;
    }
}

// function to run all the commands!
void runCommand(char command[maxCommandSize], int pipingType, int fileDescpPiping)
{
    char *tempArr = (char *)malloc((strlen(command) + 50) * sizeof(char));
    int i = 0;
    for (; i < strlen(command); i++)
    {
        tempArr[i] = command[i];
    }
    tempArr[i] = '\0';
    char *temp = tempArr;
    //Handling the case of command1 ; command2
    if (strstr(temp, ";") != NULL)
    {
        twoParallelCommand(command);
        exit(0);
    }

    if (strstr(temp, "|") != NULL)
    {
        pipeCommand(command);
        exit(0);
    }

    char *rem = tempArr;
    char *token = stringTok(rem, ' ', &rem); // firstToken

    if (strcmp(token, "checkcpupercentage") == 0)
    {
        char *pId;
        if (pipingType == 2) // take the input from the pipe!
        {
            char buf;
            int i = 0;
            char *PID = (char *)malloc(maxPidSize * sizeof(char));
            while (read(fileDescpPiping, &buf, 1) > 0)
            {
                PID[i++] = buf;
            }
            PID[i] = '\0';
            pId = PID;
        }
        if (pipingType != 2)
        {
            pId = stringTok(rem, ' ', &rem);
        }
        int isAppend = 0;
        char *outputFile = checkForOutputFile(rem, &isAppend);
        checkCPUPercentage(pId, outputFile, isAppend, pipingType, fileDescpPiping); // outputFile is NULL if no output redirection
        exit(0);
    }
    else if (strcmp(token, "checkresidentmemory") == 0)
    {

        char *pId;
        if (pipingType == 2) // take the input from the pipe!
        {
            char buf;
            int i = 0;
            char *PID = (char *)malloc(maxPidSize * sizeof(char));
            while (read(fileDescpPiping, &buf, 1) > 0)
            {
                PID[i++] = buf;
            }
            PID[i] = '\0';
            pId = PID;
        }
        if (pipingType != 2)
        {
            pId = stringTok(rem, ' ', &rem);
        }
        int isAppend = 0;
        char *outputFile = checkForOutputFile(rem, &isAppend);
        checkresidentmemory(pId, outputFile, isAppend, pipingType, fileDescpPiping); // outputFile is NULL if no output redirection
        exit(0);
    }
    else if (strcmp(token, "listFiles") == 0)
    {
        token = stringTok(rem, ' ', &rem);
        if (token != NULL)
        {
            printf("Illegal command or arguments\n");
            exit(1);
        }
        listFiles();
        exit(0);
    }
    else if (strcmp(token, "sortFile") == 0)
    {
        char *fileName;
        if (pipingType == 2) // take the input from the pipe!
        {
            char buf;
            int i = 0;
            char *FILENAME = (char *)malloc(maxPidSize * sizeof(char));
            while (read(fileDescpPiping, &buf, 1) > 0)
            {
                FILENAME[i++] = buf;
            }
            FILENAME[i] = '\0';
            fileName = FILENAME;
        }
        if (pipingType != 2)
        {
            if (*rem == '<')
            {
                rem = rem + 1;
            }
            while (*rem == ' ')
            {
                rem = rem + 1;
            }
            int i = 0;
            char *temp = (char *)malloc(maxFileNameSize * sizeof(char));
            while (*rem != '>' && *rem != ' ' && *rem != '\0')
            {
                temp[i++] = *rem;
                rem = rem + 1;
            }
            fileName = temp;
        }
        int isAppend = 0;
        char *outputFile = checkForOutputFile(rem, &isAppend);
        sortFile(fileName, outputFile, isAppend, pipingType, fileDescpPiping); // outputFile is NULL if no output redirection
        free(temp);
        exit(0);
    }
    else if (strcmp(token, "executeCommands") == 0)
    {
        char *fileName;
        if (pipingType == 2) // take the input from the pipe!
        {
            char buf;
            int i = 0;
            char *FILENAME = (char *)malloc(maxPidSize * sizeof(char));
            while (read(fileDescpPiping, &buf, 1) > 0)
            {
                FILENAME[i++] = buf;
            }
            FILENAME[i] = '\0';
            fileName = FILENAME;
        }
        if (pipingType != 2)
        {
            fileName = stringTok(rem, ' ', &rem);
        }
        executeCommand(fileName);
        exit(0);
    }
    else if (strcmp(token, "cat") == 0)
    {
        char *args[maxArguments];
        int k = 1;
        args[0] = "/bin/cat";
        int flag_write = 0; // 0 for STDOUT, 1 for output redirection in truncatedMode, 2 for output redirection int appendMode.
        int flag_read = 0;
        char *inputFile = (char *)malloc(maxFileNameSize * sizeof(char));
        char *outputFile = (char *)malloc(maxFileNameSize * sizeof(char));
        int isAppend = 0;
        while (1)
        {
            if (*rem == '<')
            {
                flag_read = 1;
                rem = rem + 1;
                if (*rem != ' ')
                {
                    int i = 0;
                    while (*rem != '>' && *rem != ' ' && *rem != '\0')
                    {
                        inputFile[i++] = *rem;
                        rem = rem + 1;
                    }
                }
                else if (*rem == ' ')
                {
                    stringTok(rem, ' ', &rem);
                    int i = 0;
                    while (*rem != '>' && *rem != ' ' && *rem != '\0')
                    {
                        inputFile[i++] = *rem;
                        rem = rem + 1;
                    }
                }
                while (*rem == ' ')
                {
                    rem = rem + 1;
                }
                break;
            }
            else if (*rem == '>')
            {
                flag_write = 1;
                rem = rem + 1;
                if (*(rem) == '>')
                {
                    rem = rem + 1;
                    isAppend = 1;
                }
                if (*rem != ' ')
                {
                    int i = 0;
                    while (*rem != '\0' && *rem != ' ' && *rem != '\0')
                    {
                        outputFile[i++] = *rem;
                        rem = rem + 1;
                    }
                }
                else
                {
                    stringTok(rem, ' ', &rem);
                    int i = 0;
                    while (*rem != '\0' && *rem != ' ' && *rem != '\0')
                    {
                        outputFile[i++] = *rem;
                        rem = rem + 1;
                    }
                }
                while (*rem == ' ')
                {
                    rem = rem + 1;
                }
                break;
            }
            while (*rem == ' ')
            {
                rem = rem + 1;
            }
            char *temp = (char *)malloc(maxCommandSize * sizeof(char));
            int q = 0;
            while (*rem != '<' && *rem != '>' && *rem != ' ' && *rem != '\0')
            {
                temp[q++] = *rem;
                rem = rem + 1;
            }
            if (strlen(temp) != 0)
                args[k++] = temp;
            if (strlen(rem) == 0)
                break;
            // free(temp);
        }
        args[k] = NULL;

        if (flag_read == 1) //means input redirection should be done!
        {
            int fileDescp_read = open(inputFile, O_RDONLY); // token here is inputfile.
            if (fileDescp_read < 0)
            {
                printf("Error in reading the file.\n");
                exit(1);
            }
            if (dup2(fileDescp_read, STDIN_FILENO) < 0)
            {
                printf("Error in Dup2\n");
                exit(1);
            }
        }
        int child = fork();
        if (child < 0)
        {
            printf("Fork Failed\n");
            exit(1);
        }
        int fileDescp_write;
        if (child == 0)
        {
            if (flag_write == 0)
            {
                outputFile = checkForOutputFile(rem, &isAppend);
            }
            if (outputFile != NULL)
            {
                int writeOption = 01000;
                if (isAppend == 1)
                    writeOption = 02000;
                fileDescp_write = open(outputFile, O_WRONLY | writeOption | O_CREAT, S_IRUSR | S_IWUSR);
                if (dup2(fileDescp_write, STDOUT_FILENO) < 0)
                {
                    printf("Error in Dup2\n");
                    exit(0);
                }
            }
            else if (pipingType == 1) // putting the output on pipe.
            {
                if (dup2(fileDescpPiping, STDOUT_FILENO) < 0)
                {
                    printf("Error in Dup2\n");
                    exit(0);
                }
            }
            execve("/bin/cat", args, NULL);
        }
        free(inputFile);
        free(outputFile);
        free(temp);
        wait(NULL);
        close(fileDescp_write);
        exit(0);
    }
    else if (strcmp(token, "grep") == 0)
    {
        //printf("grep");
        char *args[maxArguments];
        int k = 1;
        args[0] = "/bin/grep";
        int flag_write = 0; // 0 for STDOUT, 1 for output redirection in truncatedMode, 2 for output redirection int appendMode.
        int flag_read = 0;
        char *inputFile = (char *)malloc(maxFileNameSize * sizeof(char));
        char *outputFile = (char *)malloc(maxFileNameSize * sizeof(char));
        int isAppend = 0;
        while (1)
        {
            if (*rem == '<')
            {
                flag_read = 1;
                rem = rem + 1;
                if (*rem != ' ')
                {
                    int i = 0;
                    while (*rem != '>' && *rem != ' ' && *rem != '\0')
                    {
                        inputFile[i++] = *rem;
                        rem = rem + 1;
                    }
                }
                else if (*rem == ' ')
                {
                    stringTok(rem, ' ', &rem);
                    int i = 0;
                    while (*rem != '>' && *rem != ' ' && *rem != '\0')
                    {
                        inputFile[i++] = *rem;
                        rem = rem + 1;
                    }
                }
                while (*rem == ' ')
                {
                    rem = rem + 1;
                }
                break;
            }
            else if (*rem == '>')
            {
                flag_write = 1;
                rem = rem + 1;
                if (*(rem) == '>')
                {
                    rem = rem + 1;
                    isAppend = 1;
                }
                if (*rem != ' ')
                {
                    int i = 0;
                    while (*rem != '\0' && *rem != ' ' && *rem != '\0')
                    {
                        outputFile[i++] = *rem;
                        rem = rem + 1;
                    }
                }
                else
                {
                    stringTok(rem, ' ', &rem);
                    int i = 0;
                    while (*rem != '\0' && *rem != ' ' && *rem != '\0')
                    {
                        outputFile[i++] = *rem;
                        rem = rem + 1;
                    }
                }
                while (*rem == ' ')
                {
                    rem = rem + 1;
                }
                break;
            }
            while (*rem == ' ')
            {
                rem = rem + 1;
            }
            char *temp = (char *)malloc(maxCommandSize * sizeof(char));
            int q = 0;
            while (*rem != '<' && *rem != '>' && *rem != ' ' && *rem != '\0')
            {
                temp[q++] = *rem;
                rem = rem + 1;
            }
            if (strlen(temp) != 0)
                args[k++] = temp;
            if (strlen(rem) == 0)
                break;
            // free(temp);
        }
        args[k] = NULL;

        if (flag_read == 1) //means input redirection should be done!
        {
            int fileDescp_read = open(inputFile, O_RDONLY);
            if (fileDescp_read < 0)
            {
                printf("Error in reading the file\n");
                exit(1);
            }
            if (dup2(fileDescp_read, STDIN_FILENO) < 0)
            {
                printf("Error in Dup2\n");
                exit(1);
            }
        }
        if (pipingType == 2) // Taking input from the pipe!
        {
            if (dup2(fileDescpPiping, STDIN_FILENO) < 0)
            {
                printf("Error in Dup2\n");
                exit(1);
            }
        }
        int child = fork();
        if (child < 0)
        {
            printf("Fork Failed\n");
            exit(1);
        }
        int fileDescp_write;
        if (child == 0)
        {
            int isAppend = 0;
            if (flag_write == 0)
            {
                outputFile = checkForOutputFile(rem, &isAppend);
            }
            if (outputFile != NULL)
            {
                int writeOption = 01000;
                if (isAppend == 1)
                    writeOption = 02000;
                fileDescp_write = open(outputFile, O_WRONLY | writeOption | O_CREAT, S_IRUSR | S_IWUSR);
                if (dup2(fileDescp_write, STDOUT_FILENO) < 0)
                {
                    printf("Error in Dup2\n");
                    exit(0);
                }
            }
            if (pipingType == 1) // putting the input on the pipe.
            {
                if (dup2(fileDescpPiping, STDOUT_FILENO) < 0)
                {
                    printf("Error in Dup2\n");
                    exit(0);
                }
            }
            execve("/bin/grep", args, NULL);
        }
        free(inputFile);
        free(outputFile);
        free(temp);
        wait(NULL);
        close(flag_write);
        exit(0);
    }
    else
    {
        char str[maxCommandSize] = "/bin/";
        strcat(str, token);
        char *args[maxArguments];
        int i = 1;
        args[0] = str;
        while (1)
        {
            char *temp = stringTok(rem, ' ', &rem);
            if (temp == NULL)
            {
                break;
            }
            args[i++] = temp;
        }
        args[i] = NULL;
        int cId = fork();
        if (cId == 0)
        {
            execve(str, args, NULL);
            perror("Illegal command or arguments: ");
        }
    }
}
struct clockTicks
{
    int userModeClockTicks;
    int systemModeclockTicks;
};

struct clockTicks countClockTicks(char *pId)
{

    char fileName[maxFileNameSize] = "/proc/";
    strcat(fileName, pId);
    strcat(fileName, "/stat");

    int fileDescp = open(fileName, O_RDONLY, 0); //opening file in 0_RDONLY mode as #define 0_RDONLY 00

    if (fileDescp < 0)
    {
        printf("No such file or Directory\n");
        exit(0);
    }
    char userModeClockTicks[maxClockTicks];
    for (int i = 0; i < maxClockTicks; i++)
    {
        userModeClockTicks[i] = '\0';
    }
    char systemModeClockTicks[maxClockTicks];
    for (int i = 0; i < maxClockTicks; i++)
    {
        systemModeClockTicks[i] = '\0';
    }

    char *temp = (char *)malloc(sizeof(char));
    int spaceCount = 0;
    while (1)
    {
        int sizeReaded = read(fileDescp, temp, 1);
        if (*temp == ' ')
            spaceCount++;
        if (spaceCount == 13)
            break;
    }
    int i = 0;
    read(fileDescp, temp, 1);
    userModeClockTicks[i++] = *temp; //this is the 14th string
    while (*temp != ' ')
    {
        int sizeReaded = read(fileDescp, temp, 1);
        if (*temp == ' ')
            break;
        userModeClockTicks[i++] = *temp;
    }
    userModeClockTicks[i] = '\0';
    i = 0;
    read(fileDescp, temp, 1);
    systemModeClockTicks[i++] = *temp; //this is the 15th string
    while (*temp != ' ')
    {
        int sizeReaded = read(fileDescp, temp, 1);
        if (*temp == ' ')
            break;
        systemModeClockTicks[i++] = *temp;
    }
    systemModeClockTicks[i] = '\0';
    int userMode, systemMode;
    sscanf(userModeClockTicks, "%d", &userMode);
    sscanf(systemModeClockTicks, "%d", &systemMode);
    close(fileDescp); // closing "/proc/pid/stat"

    struct clockTicks userAndSystemTicks;
    userAndSystemTicks.userModeClockTicks = userMode;
    userAndSystemTicks.systemModeclockTicks = systemMode;
    return userAndSystemTicks;
}
long checkTotalCputime()
{
    char *temp = (char *)malloc(sizeof(char));
    int fileDescp = open("/proc/stat", O_RDONLY, 0);
    int sizeReaded = read(fileDescp, temp, 1);

    //skipping the "cpu" and spaces
    while (read(fileDescp, temp, 1) > 0 && *temp != ' ')
        ;
    while (read(fileDescp, temp, 1) > 0 && *temp == ' ')
        ;

    long totalClockTicks = 0;
    char kthClockTicks[maxClockTicks]; // for storing the clock ticks temorarily
    for (int i = 0; i < maxClockTicks; i++)
    {
        kthClockTicks[i] = '\0';
    }
    int i = 0;
    while (1)
    {
        kthClockTicks[i++] = *temp;
        int sizeReaded = read(fileDescp, temp, 1);
        if (*temp == ' ')
        {
            int x;
            sscanf(kthClockTicks, "%d", &x);
            for (int i = 0; i < maxClockTicks; i++)
            {
                kthClockTicks[i] = '\0';
            }
            //printf("%d\n", x);
            totalClockTicks += x;
            i = 0;
            continue;
        }
        if (*temp == '\n')
        {
            int x;
            sscanf(kthClockTicks, "%d", &x);
            for (int i = 0; i < maxClockTicks; i++)
            {
                kthClockTicks[i] = '\0';
            }
            totalClockTicks += x;
            i = 0;
            break;
        }
    }
    close(fileDescp); //closing  "/proc/pid"
    return totalClockTicks;
}

//command1
void checkCPUPercentage(char *pId, char outputFile[maxPidSize], int isAppend, int pipingType, int fileDescpPiping)
{
    if (strlen(pId) == 0)
    {
        printf("Process Id Not Fount!\n");
        exit(1);
    }
    // no need of input redirection.
    float totalticks_before = (float)checkTotalCputime();
    struct clockTicks ticks_before = countClockTicks(pId);
    sleep(1);
    struct clockTicks ticks_After = countClockTicks(pId);
    float totalticks_After = (float)checkTotalCputime();

    float userTicks_before = (float)ticks_before.userModeClockTicks;
    float systemTicks_before = (float)ticks_before.systemModeclockTicks;
    float userTicks_After = (float)ticks_After.userModeClockTicks;
    float systemTicks_After = (float)ticks_After.systemModeclockTicks;
    float userT = (((userTicks_After - userTicks_before) * 100) / (totalticks_After - totalticks_before));
    float systemT = (((systemTicks_After - systemTicks_before) * 100) / (totalticks_After - totalticks_before)); 

    int childPid = fork();
    if (childPid < 0)
    {
        printf("Fork failed\n");
        exit(1);
    }
    int fileDescp_write;
    if (childPid == 0)
    {

      // this code is relevant if you want to redirect the output
        if (outputFile != NULL)
        {
            int writeOption = 01000;
            if (isAppend)
                writeOption = 02000;
            fileDescp_write = open(outputFile, O_WRONLY | writeOption | O_CREAT, S_IRUSR | S_IWUSR);
            if (dup2(fileDescp_write, STDOUT_FILENO) < 0)
            {
                printf("Error in Dup2\n");
                exit(0);
            }
        }
        if (pipingType == 1) // print the output on the pipe!
        {
            if (dup2(fileDescpPiping, STDOUT_FILENO) < 0)
            {
                printf("Error in Dup2\n");
                exit(0);
            }
        }
        printf("user mode cpu percentage: %.2f %c \n", userT,'%');
        printf("system mode cpu percentage: %.2f %c \n", systemT,'%');
        exit(0);
    }
    wait(NULL);
    close(fileDescp_write);
}
//command2
void checkresidentmemory(char pId[maxPidSize], char outputFile[maxPidSize], int isAppend, int pipingType, int fileDescpPiping)
{
    if (strlen(pId) == 0)
    {
        printf("Process Id Not Fount\n");
        exit(1);
    }
    char *args[] = {"/bin/ps", "--format", "rss=", pId, NULL};

    int childPid = fork();
    if (childPid < 0)
    {
        printf("Fork failed\n");
        exit(0);
    }
    int fileDescp_write;
    if (childPid == 0)
    { // this code is relevant if you want to redirect the output
        if (outputFile != NULL)
        {
            int writeOption = 01000;
            if (isAppend)
                writeOption = 02000;
            fileDescp_write = open(outputFile, O_WRONLY | writeOption | O_CREAT, S_IRUSR | S_IWUSR);
            if (dup2(fileDescp_write, STDOUT_FILENO) < 0)
            {
                printf("Error in Dup2\n");
                exit(0);
            }
        }
        if (pipingType == 1)
        {
            if (dup2(fileDescpPiping, STDOUT_FILENO) < 0)
            {
                printf("Error in Dup2\n");
                exit(0);
            }
        }
        execve("/bin/ps", args, NULL);
    }
    wait(NULL);
    close(fileDescp_write);
}

//command 3
void listFiles() //no need of both input redirection (current directory implicitly) and output redirection file("files.txt" implicitly)
{
    int fileDescp = open("files.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (dup2(fileDescp, STDOUT_FILENO) < 0)
    {
        printf("Error in Dup2\n");
        exit(0);
    }
    int childPid = fork();
    if (childPid < 0)
    {
        printf("Error in the Fork\n");
        exit(0);
    }
    if (childPid == 0)
    {
        char *args[] = {"/bin/ls", NULL};
        execve("/bin/ls", args, NULL);
    }
    wait(NULL);
    close(fileDescp);
}

//command4
void sortFile(char fileName[maxFileNameSize], char outputFile[maxFileNameSize], int isAppend, int pipingType, int fileDescpPiping) // no need for inputFile as we are giving it ourself.
{
    if (strlen(fileName) == 0)
    {
        printf("FileName Not Fount\n");
        exit(1);
    } // the Name of the command is sortFile itself.
    int fileDescp_read = open(fileName, O_RDONLY);
    if (dup2(fileDescp_read, STDIN_FILENO) < 0)
    {
        printf("Error in Dup2!\n");
        exit(0);
    }

    int childPid = fork();
    if (childPid < 0)
    {
        printf("Fork failed\n");
        exit(0);
    }
    int fileDescp_write;
    if (childPid == 0)
    {
        //this code is relevent if you want to redirect the output
        if (outputFile != NULL)
        {
            int writeOption = 01000;
            if (isAppend)
                writeOption = 02000;
            fileDescp_write = open(outputFile, O_WRONLY | writeOption | O_CREAT, S_IRUSR | S_IWUSR);
            if (dup2(fileDescp_write, STDOUT_FILENO) < 0)
            {
                printf("Error in Dup2!\n");
                exit(0);
            }
        }
        if (pipingType == 1)
        {
            if (dup2(fileDescpPiping, STDOUT_FILENO) < 0)
            {
                printf("Error in Dup2\n");
                exit(0);
            }
        }
        char *args[] = {"bin/sort", STDIN_FILENO, NULL};
        execve("/bin/sort", args, NULL);
    }
    wait(NULL);
    close(fileDescp_read);
    close(fileDescp_write);
}

// command 5
void twoParallelCommand(char parallelCommand[])
{
    // you need to correct this thing

    char *command1 = (char *)malloc(maxCommandSize * sizeof(char));
    char *command2 = (char *)malloc(maxCommandSize * sizeof(char));
    stringtokenizer(parallelCommand, command1, command2, ';');
    int childPid_1 = fork();
    int childPid_2 = fork();

    if (childPid_1 < 0 || childPid_2 < 0)
    {
        printf("ForkFailed\n");
        exit(0);
    }
    if (childPid_1 == 0 && childPid_2 == 0)
        exit(0);

    if (childPid_1 == 0)
    {
        //execute command1 of full command
        runCommand(command1, 0, -1);
        exit(0);
    }
    if (childPid_2 == 0)
    {
        //execute command2 of full command
        runCommand(command2, 0, -1);
        exit(0);
    }
    waitpid((int)childPid_1, NULL, 0);
    waitpid((int)childPid_2, NULL, 0);
    free(command1);
    free(command2);
}

//command 8thb

void pipeCommand(char pipeCommand[])
{
    char *command1 = (char *)malloc(maxCommandSize * sizeof(char));
    char *command2 = (char *)malloc(maxCommandSize * sizeof(char));
    stringtokenizer(pipeCommand, command1, command2, '|');

    int pipeFd[2];
    if (pipe(pipeFd) < 0)
    {
        printf("Pipe Not opened\n");
        exit(1);
    }
    // pipint type - --------  0 for no piping, 1 for write in the pipe, 2 for reading from the pipe.
    int childPid = fork();
    if (childPid == 0)
    {
        // writes in child
        close(pipeFd[0]);
        runCommand(command1, 1, pipeFd[1]);
        close(pipeFd[1]);
        exit(1);
    }
    else
    {
        // reads in parent
        close(pipeFd[1]);
        wait(NULL);
        runCommand(command2, 2, pipeFd[0]);
        close(pipeFd[0]);
        free(command1);
        free(command2);
    }
}
//command6
void executeCommand(char fileName[maxFileNameSize]) // no need to have output redirection. and putting the output on pipe!
{
    if (strlen(fileName) == 0)
    {
        printf("File Name Not Fount\n");
        exit(1);
    }
    int fileDescp = open(fileName, O_RDONLY);
    char commandInFile[maxCommandSize];
    int i = 0;
    char *temp = (char *)malloc(sizeof(char));

    while (1)
    {
        int sizeReaded = read(fileDescp, temp, 1);
        if (sizeReaded == 0)
        {
            commandInFile[i++] = '\0';
            int cId = fork();
            if (cId == 0)
            {
                runCommand(commandInFile, 0, -1);
                exit(0);
            }
            else
            {
                wait(NULL);
                break;
            }
        }
        if (*temp == '\n')
        {
            commandInFile[i++] = '\0';

            int cId = fork();
            if (cId == 0)
            {
                runCommand(commandInFile, 0, -1);
                exit(0);
            }
            else
            {
                wait(NULL);
                i = 0;
                continue;
            }
        }
        commandInFile[i++] = *(char *)temp;
    }
    free(temp);
    close(fileDescp);
}
int exitCommand(char command[])
{
    return 0; //or 0!
}
void empty_stdin(void)
{
    int c = getchar();
    while (c != '\n' && c != EOF)
        c = getchar();
}

int main(int argc, char *argv[])
{
    //argv[0] = "checkcpupercentage 2122     ;  checkresidentmemory 2122";
    //printf("%s", argv[0]);
    runCommand(argv[0], 0, -1);
    return 0;
}