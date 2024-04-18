#include <hcc/api_fs_err.h>
#include <TLM_management.h>
/*
 * TLM_Management.c
 *
 *  Created on: 18 באפר 2024
 *      Author: hp
 */

FileSystemResult InitializeFS()
{
	FileSystemResult ret;

	hcc_mem_init(); /* Initialize the memory to be used by filesystem */

	ret = fs_init(); /* Initialize the filesystem */
	ASSERT( (ret == F_NO_ERROR ), "fs_init pb: %d\n\r", ret);

	ret = f_enterFS(); /* Register this task with filesystem */
	ASSERT( (ret == F_NO_ERROR ), "f_enterFS pb: %d\n\r", ret);

	return ret;
}
