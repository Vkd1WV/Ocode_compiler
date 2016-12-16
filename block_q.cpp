/*******************************************************************************
 *
 *	occ : The Omega Code Compiler
 *
 *	Copyright (c) 2016 Ammon Dodson
 *
 ******************************************************************************/

#include "block_q.h"




inline void Block_Queue::Dump(FILE * fd) const {
	Instruction_Queue * blk_pt;
	
	debug_msg("\tDump_blkq(): start");
	if (!fd) err_msg("Internal: Dump_blkq(): received NULL file descriptor");
	
	if(!isempty()){
		blk_pt = first();
		fputs("LBL   :\tI_OP\t RESULT \t  ARG1  \t  ARG2", fd);
		do {
			if(!blk_pt) err_msg("\tDump_blkq(): found an empty block");
			
			else{
				fputs("\n", fd);
				blk_pt->Dump(fd);
			}
		} while(( blk_pt = next() ));
	}
	else {
		info_msg("\tDump_blkq(): The block queue is empty");
		fputs("Empty\n", fd);
	}
	
	debug_msg("\tDump_blkq(): stop");
}
