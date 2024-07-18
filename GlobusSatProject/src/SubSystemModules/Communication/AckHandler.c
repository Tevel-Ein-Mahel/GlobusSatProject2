#include "SubSystemModules/Communication/AckHandler.h"
#include "SPL.h"
#include "CommandDictionary.h"

int SendAckPacket(ack_subtype_t acksubtype, sat_packet_t *cmd, unsigned char *data, unsigned short length)
{
    if (acksubtype == NULL || cmd == NULL || data == NULL)
        return null_pointer_error;

    int err = AssembleCommand(data, length, ack_type, acksubtype, cmd->ID, cmd);
    if (err != command_succsess) {
        return err;
    }

    int avalFrames = 0;
    err = TransmitSplPacket(cmd, &avalFrames);
    if (err != command_succsess) {
        return err;
    }

    return command_succsess;
}
