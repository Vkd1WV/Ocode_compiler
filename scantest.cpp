

#include "scanner.h"
#include <stdlib.h>

int main(int argc, char** argv){
	verbosity = V_DEBUG;
	debug_msg("main():start");
	
	Program_data prog_data;
	Scanner      scan(argv[1]);
	Scope_Stack  ss(&prog_data);
	
	
	ss.push(GLOBAL_SCOPE);
	
	
	while(scan.token() != T_EOF){
		printf("%s: at %u is '%s'\n",
			token_dex[scan.token()],
			scan.lnum(),
			scan.text()
		);
		
		//Print_sym(stdout, scan.sym, prog_data);
		
		scan.next_token();
	}
	
	//ss.pop();
	
	debug_msg("main(): stop");
	
	return EXIT_SUCCESS;
}
