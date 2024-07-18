#include "SatCommandHandler.h"
#include "CommandDictionary.h"
#include "string.h"
#include "SPL.h"

int trxvu_command_router(sat_packet_t *cmd)
{
    if (cmd == NULL)
    {
        return null_pointer_error;
    }

    int err = 0;
    switch (cmd->cmd_subtype)
    {
        case 8:
            printf("DDD");
            break;

    }

}