

#include "scanner.h"
#include "errors.h"

#include "scope.h"

#include <stdlib.h>

int main(int argc, char** argv){
	msg_set_verbosity(V_DEBUG);
	msg_print(NULL, V_TRACE, "main(): start");
	
	argc = argc;
	
	Program_data prog_data;
	
	Scope_Stack  ss;
	ss.push(GLOBAL_SCOPE);
	
	Scanner      scan(argv[1]);
	
	
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
	
	msg_print(NULL, V_TRACE, "main(): stop");
	
	return EXIT_SUCCESS;
}
