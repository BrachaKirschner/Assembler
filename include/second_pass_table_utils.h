#ifndef SECOND_PASS_TABLE_UTILS_H
#define SECOND_PASS_TABLE_UTILS_H

#include "defs.h"

/**
 * @brief Assigns the addresses of the labels to the direct addressing operands in the machine code image.
 * 
 * @param file_info A pointer to the FileInfo struct containing information about the file.
 * @param tables A pointer to the Tables struct containing the tables.
 * @param machine_code_image A pointer to the MachineCodeImage struct containing the machine code image. 
 */
void process_label_operands(FileInfo *file_info, Tables *tables, MachineCodeImage *machine_code_image);

/**
 * @brief Prints the labels that are defined as entries.
 * 
 * @param file_info A pointer to the FileInfo struct containing information about the file.
 * @param tables A pointer to the Tables struct containing the tables.
 */
void print_entry_labels(FileInfo *file_info, Tables *tables);

/**
 * @brief Prints the labels that are defined as externs.
 * 
 * @param file_info A pointer to the FileInfo struct containing information about the file.
 * @param tables A pointer to the Tables struct containing the tables.
 */
void print_extern_labels(FileInfo *file_info, Tables *tables);

/**
 * @brief Frees the memory allocated for the tables.
 * 
 * @param tables A pointer to the Tables struct containing the tables.
 */
void free_tables(Tables *tables);

#endif