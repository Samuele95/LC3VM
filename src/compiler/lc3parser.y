%{

#include <stdio.h>
#include <stdint-gcc.h>
#include <stdlib.h>
#include <string.h>
#include "lc3instrbuilder.c"
#include "lc3parsetree.c"
#include "lc3symboltable.c"
#include "lc3programqueue.c"
#define UINT16STRLENGTH(size) ((size % 2 == 0) ? ((size/2)+1) : ((size/2)+1)+1)
#define PC_START 0x3000

/* Lex/Yacc utility variables */
extern int yylex(void);
extern FILE *yyin;
extern int yylineno;
void yyerror (char const *s);
int yydebug=1;


/* Private utility parsing functions */
void initparser(int argc, char** argv);
void parsing_phase(int ph);
int closeparser();
uint16_t parse_instruction(uint16_t instruction, char*);
uint16_t* parse_declaration(char* label, void* data, uint8_t stringflag);
uint16_t* parse_stringlabel(nelem_t* elem);
int set_field_len(nelem_t* field);
void reset_program_counter();

/* Global static variables */
static FILE *fin;		/* Input file stream  (file to compile) */
static FILE *fout;		/* Output file stream (result) */
static int pc;			/* Program counter / line number */
static int phase;		/* Parsing phase */
static node_t* root;		/* Abstract parse tree root node */
static uint16_t* result;        /* Memory buffer for storing data into symbol table */
static int accumulator;	        /* Data length program counter modifier. It extends the program counter 
				   accordingly to the length of data fields such as arrays or strings */


%}

%union
{
    int16_t number;
    char *string;
    node_t* instrnode;
    uint16_t instruction;
    uint16_t* declaration;
}

%type <instrnode> instr_seq decl_seq begin
%type <instruction> fstprog instr op3rlog op3ilog op 
%type <declaration> decl
%type <number> offset

%token <number> ADD AND OPCODE3I OPCODE2R OPCODE2I OPCODE1R OPCODE1I REGISTER IMMED HALT TRAPINT BR
%token <string> LABEL STRING UTSTRING
%token INTASSIGN STRASSIGN SPACE OP_SEP COMMENT EMPTYLINE ENDLINE START FINISH
%start program
%%


program : fstprog sep decl_seq FINISH {if (phase == 2) {printqueue(fout);} YYACCEPT;} ;

fstprog : sep begin instr_seq HALT { $$ = parse_instruction(op_1_i(0xF,$4), "trap"); if (phase == 2) enqueue($$); } ;

begin : START SPACE IMMED ENDLINE { if ($3 != PC_START) {fprintf(stderr,"Program must start from address %d!\n", PC_START); exit(EXIT_FAILURE);}
                                    if (phase == 0) {$$ = insert(root,"pgm"); root = $$;} 
                                    if (phase == 2) enqueue($3);
                                  } ;

sep : sep ENDLINE
    | %empty {}
    ;

instr_seq : instr_seq instr sep { if (phase == 0) $$ = insert(root, "instr_seq");  }
          | %empty {}
          ;

decl_seq : decl_seq decl sep { if (phase == 0) $$ = insert(root, "decl_seq"); }
         | %empty {}
         ;

instr: op      { $$ = $1; }
     | op3rlog { $$ = $1; }
     | op3ilog { $$ = $1; }
     | TRAPINT { $$ = parse_instruction(op_1_i(0xF,$1), "trap"); if (phase == 2) enqueue($$); }		/* Trap routine */
     | LABEL   { if (phase == 0) {install($1, create_field(pc, (uint16_t* ) (intptr_t) pc, 0, 1));}	/* Subroutine start */
                 if (phase == 1) set_field_len(lookup($1)->defn); } 
     ;

decl: LABEL SPACE INTASSIGN SPACE IMMED  { $$ = (uint16_t*) malloc(sizeof(uint16_t)); $$ = parse_declaration($1, (void*) $5, 0); if (phase == 2) enqueue(*($$)); }
    | LABEL SPACE STRASSIGN SPACE STRING { $$ = (uint16_t*) malloc(sizeof(uint16_t) * UINT16STRLENGTH(strlen($5))); $$ = parse_declaration($1, (void*) $5, 1); if (phase == 2) enqueue_array($$, UINT16STRLENGTH(strlen($5))); }
    ;
    


op3rlog: ADD SPACE REGISTER OP_SEP REGISTER OP_SEP REGISTER { $$ = parse_instruction((op_3_r($1,$3,$5,$7) & 0xFFC7), "instr"); if (phase == 2) enqueue($$); }
       | AND SPACE REGISTER OP_SEP REGISTER OP_SEP REGISTER { $$ = parse_instruction((op_3_r($1,$3,$5,$7) & 0xFFC7), "instr"); if (phase == 2) enqueue($$); }
       ;


op3ilog: ADD SPACE REGISTER OP_SEP REGISTER OP_SEP IMMED { $$ = parse_instruction((op_3_i($1,$3, $5, ($7 & 0x001F), 1) | 0x0020), "instr"); if (phase == 2) enqueue($$); }
       | AND SPACE REGISTER OP_SEP REGISTER OP_SEP IMMED { $$ = parse_instruction((op_3_i($1,$3, $5, ($7 & 0x001f), 1) | 0x0020), "instr"); if (phase == 2) enqueue($$); }
       ;

op: OPCODE3I SPACE REGISTER OP_SEP REGISTER OP_SEP IMMED { $$ = parse_instruction(op_3_i($1,$3, $5, $7, 0), "instr"); if (phase == 2) enqueue($$); }
  | OPCODE2R SPACE REGISTER OP_SEP REGISTER             { $$ = parse_instruction(op_2_r($1,$3,$5), "instr"); if (phase == 2) enqueue($$); }
  | OPCODE2I SPACE REGISTER OP_SEP offset               { $$ = parse_instruction(op_2_i($1,$3,$5), "instr"); if (phase == 2) enqueue($$); }
  | OPCODE1R SPACE REGISTER                            { $$ = parse_instruction(op_1_r($1,$3), "instr"); if (phase == 2) enqueue($$); }
  | OPCODE1I SPACE offset                              { $$ = parse_instruction(op_1_i($1,$3), "instr"); if (phase == 2) enqueue($$); }
  | BR SPACE offset 				       { $$ = parse_instruction(branch($1, $3), "branch"); if (phase == 2) enqueue($$);}
  ;

  
offset: LABEL { $$ = (lookup($1) != NULL) ?  (int16_t) ((lookup($1)->defn->line) - (pc + 1)) : 0; }  /* Evil offset  */
      | IMMED { $$ = $1 - (pc + 1); }
      ;


%%
#include "lex.yy.c"

int main(int argc, char** argv) {

    initparser(argc, argv);
    parsing_phase(0);
    parsing_phase(1);
    parsing_phase(2);
    return closeparser();
}

void yyerror (char const *s) {
   int errorline = yylineno;
   fprintf(stderr, "Program did not compile due to %s in line %d\n", s, errorline);
   exit(EXIT_FAILURE);
}

void initparser(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stdout, "Usage: ----\n");
        exit(EXIT_FAILURE);
    }
    if (!(yyin = fin = fopen(argv[1], "rb"))) {
        perror("Error in opening file for reading");
        exit(EXIT_FAILURE);
    }
    if (!(fout = fopen(strcat(strtok(argv[1],"."), ".obj"), "wb"))) {
        perror("Error in opening file for writing");
        exit(EXIT_FAILURE);
    }
    accumulator = 0;
}

int closeparser() {
    fclose(fin);
    fclose(fout);
    free_queue();
    deletenode(root);
    printf("Program compiled successfully !\n");
    return EXIT_SUCCESS;
}

void parsing_phase(int ph) {
    int virtmemory_size = 0xFDFF - PC_START;
    if ((phase = ph) == 0) 
        printf("Phase-0 Compiling: Building Abstract Syntax Tree\n");
    if (phase == 1) 
        printf("Phase-1 Compiling: Computing data and instruction addresses\n");
    if (phase == 2) {
        allocate_queue(0xFDFF - PC_START);
        printf("Phase-2 Compiling: Code Generation\n");
    }
    reset_program_counter();
    while (!feof(yyin)) yyparse();
    
    if (pc > virtmemory_size) {
        perror("Fatal error. Program exceeded virtual memory space. Exiting..");
        exit(EXIT_FAILURE);
    }
}



uint16_t parse_instruction(uint16_t instruction, char* type) {
    pc++;
    if (phase == 0) insert(root, type);
    return instruction;
}

uint16_t* parse_declaration(char* label, void* data, uint8_t stringflag) {
    if (result != NULL);
        free(result);
    if (phase == 0 && stringflag == 0) {
        insert(root, "num_decl");
        install(label, create_field(pc++, (uint16_t* ) (intptr_t) data, stringflag, 1));
    }
    if (phase == 0 && stringflag == 1) {
        insert(root, "str_decl");
        install(label, create_field(pc++, (uint16_t* ) (char*) data, 1, UINT16STRLENGTH(strlen((char*) data))));
    }
    if (phase == 1) 
        set_field_len(lookup(label)->defn);
    if (phase == 2 && stringflag == 0) {
         result = (uint16_t*) malloc(sizeof(uint16_t));
        *result = parse_instruction((uint16_t) (intptr_t) data, "const");
    }
    if (phase == 2 && stringflag == 1) {
         result = (uint16_t*) calloc(sizeof(uint16_t),  lookup(label)->defn->byteslen);
         result = parse_stringlabel(lookup(label)->defn);
    }
    return result;
}


uint16_t* parse_stringlabel(nelem_t* elem) {
    if (elem == NULL) {
        perror("The given string reference is null");
        exit(EXIT_FAILURE);
    }
    if (elem->stringflag != 1) {
        perror("The given element is not a string");
        exit(EXIT_FAILURE);
    }
    return elem->bytes;
}



int set_field_len(nelem_t* field) {
    field->line += accumulator;
    accumulator += (field->byteslen - 1);
    pc++;
    return accumulator;
}

void reset_program_counter() {

    if (fseek(yyin, 0L, SEEK_SET) != 0) {
        fprintf(stderr, "Phase-%d Compiling\n", phase);
        perror("Error in entering Compiling Phase. Can't reset input file's pointer");
        exit(EXIT_FAILURE);
    }
    pc = PC_START;    
    
}

