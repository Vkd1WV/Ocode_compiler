

#include "scanner.h"
#include "errors.h"

#include "scope.h"

#include <stdlib.h>

int main(int argc, char** argv){
	verbosity = V_DEBUG;
	debug_msg("main():start");
	
	argc = argc;
	
	Program_data prog_data;
	Scanner      scan(argv[1]);
	Scope_Stack  ss;
	
	
	ss.push(GLOBAL_SCOPE);
	
	
	do {
		printf("%s\t: at %u is '%s'\n",
			token_dex[scan.token()],
			scan.lnum(),
			scan.token() != T_NL? scan.text() : ""
		);
		
		//Print_sym(stdout, scan.sym());
		
		scan.next_token();
	} while(scan.token() != T_EOF);
	
	delete ss.pop();
	
	debug_msg("main(): stop");
	
	return EXIT_SUCCESS;
}
