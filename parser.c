// Homework Three: Parser
// Ashton Ansag
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**//* NOTE: this is for me to find reminders to be edited at a later time*/

// this is the defined max table size
#define MAX_SYMTABLE_SIZE 100

// enum with token values for the lexemes
enum {
	nulsym = 1, identsym, numbersym, plussym, minussym,
	multsym, slashsym, oddsym, eqlsym, neqsym,
	lessym, leqsym, gtrsym, geqsym, lparentsym,
	rparentsym, commasym, semicolonsym, periodsym, becomessym,
	beginsym, endsym, ifsym, thensym, whilesym,
	dosym, callsym, constsym, varsym, procsym,
	writesym, readsym, elsesym
};

// enum to show what kind the symbol is
enum { CONST = 1, VAR, PROC };

// this is the structure that defines a symbol
typedef struct symbol{
	int kind, val, level, addr;
	char name[12];
}symbol;

// this is the symbol table that will store all of the important symbols
symbol symbol_table[MAX_SYMTABLE_SIZE];

// a global symbol variable that will be used as a temp to be added to table
symbol s;

// a counter to show the amount of items in the symbol table
int symCounter = 0;

// this is to store the current token being evaulated
int currentToken = 0;

// variable use to keep track of the current lex depth in the code
int procLevel = 0;

// array of all the reserved word/symbols
char table[34][12] = {
	"\0","\0","var","num","+","-",
	"*","/","odd","=","!=",
	"<","<=",">",">=","(",
	")",",",";",".",":=",
	"begin","end","if","then","while",
	"do","call","Const","Var","Proc",
	"write","read","else"
};

// the printable error messages stored in an array
char ERROR_MESSAGE[26][60] = {
	"end expected.\n",
	"Use of = instead of :=.\n",
	"= must be followed by a number.\n",
	"Identifier must be followed by =.\n",
	"const, var, procedure must be followed by identifier.\n",
	"Semicolon or comma missing.\n",
	"Incorrect symbol after procedure declaration.\n",
	"Statement expected.\n",
	"Incorrect symbol after statement part in block.\n",
	"Period expected.\n",
	"Semicolon between statements missing.\n",
	/**//*not used*/"Undeclared identifier.\n",
	/**//*not used*/"Assignment to constant or procedure is not allowed.\n",
	"Assignment operator expected.\n",
	"call must be followed by an identifier.\n",
	/**//*not used*/"Call of a constant or variable is meaningless.\n",
	"then expected.\n",
	"Semicolon or } expected.\n",
	"do expected.\n",
	"Incorrect symbol following statement.\n",
	"Relational operator expected.\n",
	"Expression must not contain a procedure identifier.\n",
	"Right parenthesis missing.\n",
	"The preceding factor cannot begin with this symbol.\n",
	"An expression cannot begin with this symbol.\n",
	"This number is too large.\n"
};

/**/// debugging string array
char identable[6][5] = { "\0", "CONS\0", "VAR\0", "PROC\0" };

// temp char array to store identifer names
char *buffer;

// this will be the filepointer to lexemelist.txt
FILE *fp;


// simple error printer that returns 0 which represents an error
int error( int number ){
	printf( "Error number %i, %s", number, ERROR_MESSAGE[number] );
	return 0;
}

// this table checks if the symbol is in the table, if not add to table
void addTo( symbol s ){
	int temp = 0;
	 
	while( temp<symCounter ){
		if( !strcmp(s.name, symbol_table[temp++].name) ) return;
	}
	symbol_table[symCounter++] = s;
}

/**///need to finish implementing this
int lookup( symbol s ){
	int temp = 0;
	
	return 0;
}

// functional prototypes because some of them depend on each other
int program(), block(), condition(), relation();
int constdec(), vardec(), procdec(), statement();
int expression(), term(), factor();

// this is a function to grab the next token from the lexemelist.txt
void getToken(){
	fscanf( fp, "%s ", buffer );
	currentToken = atoi(buffer);
	///**/printf( "(%s,%i)\n", buffer,currentToken );
}

/* checks program syntax:
 *  - 1 block
 *  - 1 '.'
 */
int program(){
	getToken();
	if( block() )
		if( currentToken != periodsym ) return error(9);
		else /**/printf( "No errors, program is syntactically correct.\n" );
	return 1;
}

/* checks block syntax:
 *  - 1 constant  declaration (or none)
 *  - 1 variable  declaration (or none)
 *  - 1 procedure declaration (or none)
 *  - 1 statement             (or none)
 */
int block(){
	
	if( currentToken == constsym ){
		// the token is a constant symbol, test this declaration
		if( !constdec() ) return 0;
	}
	
	if( currentToken == varsym ){
		// the token is a variable symbol, test this declaration
		if( !vardec() ) return 0;
	}
	
	if( currentToken == procsym ){
		// the token is a procedure symbol, test this declaration
		if( !procdec() ) return 0;
	}
	
	// now test the statement at the end of the block
	return statement();
}

/* checks constant declaration syntax:
 *  - 1 "constsym" token (already found before function call)
 *  - 1 "identsym" token (or more seperated by commas)
 *    containing:
 *    - 1 "eqlsym" token
 *    - 1 "numbersym" token (which is not more than 5 digits)
 *  - 1 "semicolonsym" token
 */
int constdec(){
	
	// set the temp symbol to have CONST kind value and clear fields
	//   the fields will be refilled in as the information is parsed
	s.kind = CONST;
	s.val = s.level = s.addr = -1;
	
	// loop till "ident eql number semicolon" pattern is found
	//   note: commas can replace a semicolon and restart the pattern without error
	do{
		
		// get token and test for first pattern piece "identsym"
		getToken();
		if( currentToken != identsym ) return error(4);     // no ident error
		
		// an identifier was found the next token is its name
		getToken();
		strcpy( s.name, buffer );
		
		// get token and test for second pattern piece "eqlsym"
		getToken();
		if( currentToken != eqlsym )
			if( currentToken == becomessym ) return error(1); // := not = error
			else return error(3);                             // anyother error
		
		// get token and test for third pattern piece "numbersym"
		getToken();
		if( currentToken != numbersym ) return error(2);    // not a num error
		
		// a number was found the next token is its value
		getToken();
		if( currentToken >= 100000 ) return error(25);      // num too large error
		s.val = currentToken;
		
		// we reached the end of one constant declaration (try to) add to table
		addTo( s );
		
		// get the next token to decide if the declaration statement continues
		getToken();
		  
	}while( currentToken == commasym ); // seperated by commas
	
	if( currentToken != semicolonsym ) return error(5); // expected ; error
	
	// done with declaration statement get the next token
	getToken();
	return 1;
}

/* checks variable declaration syntax:
 *  - 1 "varsym" token (already found before function call)
 *  - 1 "identsym" token (or more seperated by commas)
 *  - 1 "semicolonsym" token
 */
int vardec(){
	
	// set the temp symbol to have VAR kind value and clear fields
	//   the fields will be refilled in as the information is parsed
	s.kind = VAR;
	s.val = s.level = s.addr = -1;
	
	// loop till "ident semicolon" pattern is found
	//   note: commas can replace a semicolon and restart the patter without error
	do{
		
		// get token and test for the first pattern piece "identsym"
		getToken();
		if( currentToken != identsym ) return error(4);     // no ident error
		
		// an identifier was found the next token is its name
		getToken();
		strcpy( s.name, buffer );
		s.level = procLevel;
		
		// we reached the end of one variable declaration (try to) add to table
		addTo( s );
		
		// get the next token to decide if the declaration statement continues
		getToken();
		
	}while( currentToken == commasym ); // seperated by commas
	
	if( currentToken != semicolonsym ) return error(5); // expected ; error
	
	getToken();
	return 1;
}

/* checks procedure declaration syntax:
 *  - 1 "procsym" token (already found before function call)
 *  - 1 "identsym" token
 *  - 1 "semicolonsym" token
 *  - 1 block
 *  - 1 "semicolonsym" token
 */
int procdec(){
	
	// set the temp symbol to have PROC kind value and clear fields
	//   the fields will be refilled as the information is parsed
	s.kind = PROC;
	s.val = s.level = s.addr = -1;
	
	// loop till "ident semicolon block semicolon" pattern is found
	//   note: procedures can follow each other and will restart the pattern search
	do{
		
		// get token and test for first pattern piece "identsym"
		getToken();
		if( currentToken != identsym ) return error(4);     // no ident error
		
		// an identifier was found the next token is its name
		getToken();
		strcpy( s.name, buffer );
		
		// get token and test for second pattern piece "semicolonsym"
		getToken();
		if( currentToken != semicolonsym ) return error(6); // expect ; error
		
		// we move inside a procedure and down a level
		s.level = procLevel++;
		
		// add to table
		addTo( s );
		
		// get token and test this block
		getToken();
		if( !block() ) return 0;
		
		// the procedure must end with a semicolon
		if( currentToken != semicolonsym ) return error(5); // expect ; error
		
		// we finished parsing in the procedure move back up a level
		procLevel--;
		
		// get token decided if there is another procedure to be declared
		getToken();
	}while( currentToken == procsym );
	
	return 1;
}

/* checks the statement syntax:
 *  - multiple patterns:
 *    i.   "ident becomes expression()"
 *    ii.  "call ident"
 *    iii. "begin statement()" (any number of: "semicolon statement()") "end"
 *    iv.  "if condition() then statement()" (optional: "else statement()")
 *    v.   "while condition() do statement()"
 *    vi.  "read ident"
 *    vii. "write ident"
 */
int statement(){
	
	// test for first pattern piece "identsym"
	if( currentToken == identsym ){
		
		// an identifier was found the next token is its name
		getToken();
		
		// get token and test for second pattern piece "becomessym"
		getToken();
		if( currentToken != becomessym ) return error(3);   // expect := error
		
		// get token and test the expression
		getToken();
		if( !expression() ) return 0;
		
	// test for first pattern piece "callsym"
	}else if( currentToken == callsym ){
		
		// get token and test for second pattern piece "identsym"
		getToken();
		if( currentToken != identsym ) return error(14);	  // no ident error
		
		// get token and move on
		getToken();
		
	// test for first pattern piece "beginsym"
	}else if( currentToken == beginsym ){
		
		// get token and test the statement
		getToken();
		if( !statement() ) return error(7);  // a correct statement was expected
		
		// while optional "semicolonsym" is found then keep testing statements
		while( currentToken	== semicolonsym ){
			
			// get token and test the statement
			getToken();
			if( !statement() ) return error(7);	// a correct statement was expected
		}
		
		// test for the last pattern piece "endsym"
		if( currentToken != endsym ) return error(8);       // no end error
		
		// get token and move on
		getToken();
		
	// test for first pattern piece "ifsym"
	}else if( currentToken == ifsym ){
		
		// get token and test the condition
		getToken();
		if( !condition() ) return 0;	                      // error
		
		// test for the third pattern piece "thensym"
		if( currentToken != thensym ) return error(16);	    // no then error
		
		// get token and test the statement
		getToken();
		if( !statement() ) return 0;	                      // error
		
		/**/ // get to adding something for an else
		
	// test for first pattern piece "whilesym"
	}else if( currentToken == whilesym){
		
		// get token and test the condition
		getToken();
		if( !condition() ) return 0;	                      // error
		
		// test for the third pattern piece "dosym"
		if( currentToken != dosym ) return error(18);	      // no do error
		
		// get token and test the statement
		getToken();
		if( !statement() ) return 0;	                      // error
	 
	}
	/**/// implement things for read and write
	/*
	// test for first pattern piece "readsym"
	}else if( currentToken == readsym ){
		
		// get token and test for second pattern piece "identsym"
		getToken();
		if( currentToken != identsym ) return error(14);	  // no ident error
		
	// test for first pattern piece "writesym"
	}else if( currentToken == writesym ){
		
		// get token and test for second pattern piece "identsym"
		getToken();
		if( currentToken != identsym ) return error(14);	  // no ident error
		
	}
	*/
	
	return 1;
}

/* checks expression syntax:
 *  - (optional: "(plussym or minussym)" )
 *    - 1 "term"
 *    - (any number of: "(plus or minus) term" )
 */
int expression(){
	
	// test for optional "plussym" or "minussym"
	if( currentToken ==	plussym || currentToken == minussym ) getToken();
	
	// test this term
	if( !term() ) return 0;                               // error
	
	// while option "plussym" or "minussym" is found keep testing terms
	while( currentToken == plussym || currentToken == minussym ){
		getToken();
		if( !term() ) return 0;                             // error
	}
	return 1;
}

/* checks condition syntax:
 *  - multiple patterns
 *    i.    "odd expression()"
 *    ii.   "expression() relation() expression()"
 */
int condition(){
	
	// test for first pattern piecet "oddsym"
	if( currentToken == oddsym ){
		
		// get token and test the expression()
		getToken();
		if( !expression() ) return 0;                       // error
		
	}else{
		
		// test the expression
		if( !expression() ) return 0;                       // error
		
		// test the relation
		if( currentToken != relation() ) return error(20);	// rel-op error
		
		// get token and test the expression
		getToken();
		if( !expression() ) return 0;                       // error
	}
	return 1;
}

/* checks the term syntax:
 *  - 1 factor
 *  - (any number of: "("multsym" or "slashsym") factor" )
 */
int term(){
	
	// test the factor
	if( !factor() ) return 0;                             // error
	
	// while optional "multsym" or "slashsym" found test the factor
	while( currentToken == multsym || currentToken == slashsym ){
		    
		// get token and test
		getToken();
		if( !factor() ) return 0;                           // error
	}
	return 1;
}

/* checks the factor syntax:
 *  - multiple patterns:
 *    i.   "ident"
 *    ii.  "number"
 *    iii. "lparent expression() rparent"
 */
int factor(){
	
	// test for first patterm piece "identsym"
	if( currentToken == identsym ){
		getToken();
		
	// test for first pattern piece "number"
	}else if( currentToken == numbersym ){
		
		// a number is found the next number is its value
		getToken();
		if( currentToken >= 100000 ) return error(25);      // num too large error
		
	// test for first pattern piece "lparentsym"
	}else if( currentToken == lparentsym ){
		
		// get token and test the expression
		getToken();
		if( !expression() ) return 0;                       // error
		
		// test for last pattern piece "rparentsym"
		if( currentToken != rparentsym ) return error(22);  // missing ) error
		
		// get token carry on
		getToken();
		
	}else	return error(23);                               // bad factor error
	
	// get token carry on
	getToken();
	return 1;
}

// test if currentToken is a relational operator
int relation(){
	switch(currentToken){
	case eqlsym: // equal
	case neqsym: // not equal
	case lessym: // less than
	case leqsym: // less than equal to
	case gtrsym: // greater than
	case geqsym: // greater than equal to
		return currentToken; // bleed to a true return
	default:
		return error(20);                                   // rel-op error
	}
}

int main( int argc, char *argv[] ){
	 
	/**/// simply way to print lexemelist if run with -l*
	if( argc > 1 ){
		if( argv[1][0] == '-' && argv[1][1] == 'l' ){
			fp = fopen( "lexemelist.txt","r" );
			buffer = (char *)malloc( 10000 );
			fscanf(fp, "%[^\n]\n", buffer );
			printf("Lexemelist:\n%s\n\n", buffer );
			fclose( fp );
		}
	}
	
	fp = fopen( "lexemelist.txt","r" );
	int i, current = 0;
	buffer = (char *)malloc(12);
	
	program();
	
	// testing the symboltable
	///**/printf( "\n\nkind\tname\tlevel\tvalue\n" );
	for( i=0; i<symCounter; i++ ){
		s = symbol_table[i];
	///**/printf( "%s\t%s\t%i\t%i\n", identable[s.kind], s.name, s.level, s.val );
	}
	///**/printf( "\n" );
	
	fclose( fp );
		
	
	return 0;
}
