#ifndef DROGI_COMMANDS_H
#define DROGI_COMMANDS_H

#include "map.h"

/** @brief Gets the commands and executes.
 * If command starts with '#' then it is a comment and will be ignored.
 * Each not right command will result to 'ERROR n', where n will be the number
 * of the line of the command.
 * @param[in,out] map - pointer on a map.
 */
void execCommand(Map *map);

#endif //DROGI_COMMANDS_H
