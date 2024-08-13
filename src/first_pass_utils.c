#include "first_pass_utils.h"
#include "defs.h"
#include "parser_utils.h"
#include "data_utils.h"
#include "instruction_utils.h"
#include "first_pass_table_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int is_empty(char line[])
{
    int i;
    for(i = 0; i < strlen(line); i++)
    {
        if(line[i] != ' ' && line[i] != '\t')
        {
            return 0;
        }
    }
    return 1;
}

int is_comment(char line[])
{
    if(line[0] == ';')
    {
        return 1;
    }
    return 0;
}

int search_for_label(char line[], FileInfo *file_info, Tables *tables)
{
    char label[MAX_LABEL_LENGTH];
    sscanf(line, "%s", label);
    if(label[strlen(label)-1] == ':')
    {
        label[strlen(label)-1] = '\0';
        if(is_valid_label(label, file_info, tables))
        {
            return 1;
        }

        printf(ERROR_MESSAGE, "error: invalid label name");
        file_info->error_status = 1;
    }
    return 0;
}

int is_data(char line[])
{
    int i;
    for(i = 0; i < NUM_OF_DATA; i++)
    {
        if(strstr(line, data_table[i].name) != NULL)
        {
            return 1;
        }
    }
    return 0;
}

int is_instruction(char line[])
{
    int i;
    for(i = 0; i < NUM_OF_OP; i++)
    {
        if(strstr(line, operation_table[i].name) != NULL)
        {
            return 1;
        }
    }
    return 0;
}

char *process_label(char line[], FileInfo *file_info, Tables *tables, int counter)
{
    char label_name[MAX_LABEL_LENGTH], data_name[MAX_DATA_NAME_LENGTH];
    sscanf(line, "%s", label_name);
    label_name[strlen(label_name) - 1] = '\0'; /*removing the ':' from the label*/
    line = line + strlen(label_name) + 1; /*+1 for ':'*/
    sscanf(line, "%s", data_name);

    if(strcmp(data_name, data_table[2].name) == 0 || strcmp(data_name, data_table[3].name) == 0)
    {
        printf(ERROR_MESSAGE, "warning: label cannot be on a line with an entry or extern instruction");
        file_info->error_status = 1;
    }
    if(strcmp(data_name, data_table[0].name) == 0 || strcmp(data_name, data_table[1].name) == 0)
    {
        add_label_to_table(label_name, counter, "data", file_info, tables);
    }
    else /*this is an instruction label*/
    {
        add_label_to_table(label_name, counter, "instruction", file_info, tables);
    }
    return line;
}

void process_data(char *line, FileInfo *file_info, Tables *tables, MachineCodeImage *machine_code_image)
{
    char data_name[MAX_DATA_NAME_LENGTH];
    line = walkthrough_white_spaces(line);
    sscanf(line, "%s", data_name);
    line = line + strlen(data_name);
    if(strcmp(data_name, data_table[0].name) == 0)
    {
        process_data_data(line, file_info, machine_code_image);
    }
    else if(strcmp(data_name, data_table[1].name) == 0)
    {
        process_string_data(line, file_info, machine_code_image);
    }
    else if(strcmp(data_name, data_table[2].name) == 0)
    {
        process_entry_data(line, file_info, tables);
    }
    else if(strcmp(data_name, data_table[3].name) == 0)
    {
        process_extern_data(line, file_info, tables);
    }
    else
    {
        printf(ERROR_MESSAGE, "error: invalid data name");
        file_info->error_status = 1;
    }
}

void process_instruction(char *line, FileInfo *file_info, Tables *tables, MachineCodeImage *machine_code_image)
{
    char *token;
    int i, opcode;
    int source_register_flag = 0; /*keeps track if the source operand addressing method is 2 or 3*/
    int word_count = 0; /*keeps track of the number of information words in the instruction_array*/
    int comma_flag = 0; /*flag for indecating the comma is expected in the beginning of next data*/

    token = strtok(line, " \t\n");/*this token consists of the operation name*/

    /*getting the operation code*/
    for(opcode = 0; opcode < NUM_OF_OP; opcode++)
    {
        if(strcmp(token, operation_table[opcode].name) == 0)
        {
            machine_code_image->instruction_array[machine_code_image->IC] = 1 << 2; /*inserting 'A' field into the instruction word*/
            machine_code_image->instruction_array[machine_code_image->IC] |= operation_table[opcode].code << 11;
            break;
        }
    }

    /*checking if operation name is invalid*/
    if(opcode == NUM_OF_OP)
    {
        printf(ERROR_MESSAGE, "error: invalid operation name");
        file_info->error_status = 1;
        return;
    }

    for(i = 0; i < operation_table[opcode].num_of_operands; i++)
    {
        token = strtok(NULL, " \t\n"); /*this token consists of the operand*/

        /*checking if there are to few operands*/
        if(token == NULL)
        {
            printf(ERROR_MESSAGE, "error: missing operand");
            file_info->error_status = 1;
            return;
        }

        token = comma_parser(token, &comma_flag, file_info);
        
        /*finding the addressing method of the operand*/
        if(token[0] == '#' && is_valid_integer(token + 1))
        {
            process_immediate(token, opcode, i+1, ++word_count, file_info, machine_code_image);
        }
        else if(token[0] == '*' && is_register(token + 1))
        {
            /*dealing with the situation that both the operands are registers and therfore there most be only one data word*/
            if(i == 0)
            {
                source_register_flag = 1;
            }
            word_count += (i == 1 && source_register_flag)? 0 : 1;
            process_indirect_register(token, opcode, i+1, word_count, file_info, machine_code_image);
        }
        else if(is_register(token))
        {
            /*dealing with the situation that both the operands are registers and therfore there most be only one data word*/
            if(i == 0)
            {
                source_register_flag = 1;
            }
            word_count += (i == 1 && source_register_flag)? 0 : 1;
            process_direct_register(token, opcode, i+1, word_count, file_info, machine_code_image);
        }
        else
        {
            process_direct(token, opcode, i+1, ++word_count, file_info, tables, machine_code_image);
        }
    }
    machine_code_image->IC += word_count + 1; /*+1 for the instruction word*/

    /*if the next token isn't empty*/
    if((token = strtok(NULL, " \t\n")))
    {
        if(!comma_flag || (comma_flag && (token[0] == ','))) /*if there is a comma at the end of the last line or at the beginnig of this line*/
        {
            printf(ERROR_MESSAGE, "error: to many operands");
        }
        else
        {
            printf(ERROR_MESSAGE, "error: extraneous text after the end of the command");
        }
        file_info->error_status = 1;
        return;
    }

    /*checking if there is a comma at the end of the line of a command with operands*/
    if(!comma_flag && i != 0)
    {
        printf(ERROR_MESSAGE, "error: unnecessary comma at the end of the command");
        file_info->error_status = 1;
    }
}


