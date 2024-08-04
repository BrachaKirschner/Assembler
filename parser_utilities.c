#include "parser_utilities.h"
#include "common.h"
#include <stdio.h>

char *comma_parser(char *token, int *comma_flag, FileInfo *file_info)
{
    /*if there wasn't a comma at the end of last operand then we'll check for it now*/
    if(comma_flag)
    {
        if(strcmp(token, ",") == 0)
        {
            token = strtok(NULL, " \t");
        }
        else if(token[0] != ',')
        {
            printf(ERROR_MESSAGE, "missing comma");
            file_info->error_status = 1;
            token++;
        }
        comma_flag = 0;
    }

    /*getting rid of the comma at the end*/
    if(token[strlen(token-1)] == ',')
    {
        token[strlen(token-1)] = '\0';
    }
    else
    {
        comma_flag = 1;
    }
    return token;
}

int read_line(char *line, FileInfo *file_info)
{
    /*reading the next line from file*/
    if(!fgets(line, MAX_LINE_LENGTH+2, file_info->file))
    {
        return 0;
    }
    /*checking if the line is too long (more than 80 characters)*/
    if(line[strlen(line)-1] != '\n')
    {
        printf(ERROR_MESSAGE, "error: line is too long");
        file_info->error_status = 1;
    }
    line[strlen(line)-1] = '\0'; /*getting rid of '\n'*/
    return 1;
}

int is_label(char line[])
{
    char label[MAX_LABEL_LENGTH];
    sscanf(line, "%s", label);
    if(label[strlen(label)-1] == ':')
    {
        return 1;
    }
    return 0;
}


