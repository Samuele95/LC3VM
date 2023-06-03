%option yylineno
%{

#include "../memory/memory.h"
#include "../instructionset/opcodes.h"
#include "lc3parser.tab.h"
extern YYSTYPE yylval;
int opval(char* op_code);
int brval(char* br_code);
int trapval(char* trap_code);
int regaddress(char* regname);
int const_value(const char* numstring, int base);


%}

/* Trap interrupts */
HALT       HALT
TRAPINT    GETC|IN|OUT|PUTS[P]?
BR         BR[N|Z|P]

/* Opcode types */
ADD        ADD
AND        AND
OPCODE3I   (LD|ST)R
OPCODE2R   NOT
OPCODE2I   ST[I]?|L(EA|D[I]?)
OPCODE1R   J(MP|SRR)
OPCODE1I   BR|JSR|TRAP

/* Operand types */
REGISTER   [rR][0-7]
IMMED      ([xX][-]?[0-9a-fA-F]+)|([#]?[-]?[0-9]+)  
LABEL      [A-Za-z][A-Za-z_0-9]*
STRING     \"([^\"]*|(\\\"))*\"
UTSTRING   \"[^\n\r]*



/* Program directives */
START      \.ORIG
FINISH     \.END
INTASSIGN  .FILL
STRASSIGN  .STRINGZ


/* Operand and white space spec */
SPACE      [ \t]
OP_SEP     {SPACE}*,{SPACE}*
COMMENT    [;][^\n\r]*
EMPTYLINE  {SPACE}*{COMMENT}?
ENDLINE    {EMPTYLINE}\r?\n\r?




%%
{HALT}         yylval.number = trapval("HALT"); return HALT;
{TRAPINT}      yylval.number = trapval(yytext); return TRAPINT;
{BR}           yylval.number = brval(yytext);   return BR;

{ADD}          yylval.number = opval(yytext); return ADD;
{AND}          yylval.number = opval(yytext); return AND;
{OPCODE3I}     yylval.number = opval(yytext); return OPCODE3I;
{OPCODE2R}     yylval.number = opval(yytext); return OPCODE2R;
{OPCODE2I}     yylval.number = opval(yytext); return OPCODE2I;
{OPCODE1R}     yylval.number = opval(yytext); return OPCODE1R;
{OPCODE1I}     yylval.number = opval(yytext); return OPCODE1I;
{REGISTER}     yylval.number = regaddress(yytext); return REGISTER;
{IMMED}        { yylval.number = (yytext[0] == 'x' || yytext[0] == 'X') ? const_value(yytext, 16) : const_value(yytext, 10);
                 return IMMED; }

{LABEL}        yylval.string = strdup(yytext); return LABEL;
{STRING}       { yylval.string = (char*) calloc(yyleng-1, sizeof(char));
                 strncpy(yylval.string, &yytext[1], yyleng-2); //already null terminated with \0, by using calloc before
                 return STRING;
               }

{UTSTRING}     yylval.string = strdup(yytext); return UTSTRING;


{START}        return START;
{FINISH}       return FINISH;
{INTASSIGN}    return INTASSIGN;
{STRASSIGN}    return STRASSIGN;

{SPACE}        return SPACE;
{OP_SEP}       return OP_SEP;
{COMMENT}      return COMMENT;
{EMPTYLINE}    return EMPTYLINE;
{ENDLINE}      return ENDLINE;



%%

int opval(char* op_code) {
    if  (strcmp(op_code, "ADD") == 0)    return OP_ADD;   /* add  */
    if  (strcmp(op_code, "LD") == 0)     return OP_LD;    /* load */
    if  (strcmp(op_code, "ST") == 0)     return OP_ST;    /* store */
    if  (strcmp(op_code, "JSR") == 0)    return OP_JSR;   /* jump register */
    if  (strcmp(op_code, "AND") == 0)    return OP_AND;   /* bitwise and */
    if  (strcmp(op_code, "LDR") == 0)    return OP_LDR;   /* load register */
    if  (strcmp(op_code, "STR") == 0)    return OP_STR;   /* store register */
    if  (strcmp(op_code, "RTI") == 0)    return OP_RTI;   /* unused */
    if  (strcmp(op_code, "NOT") == 0)    return OP_NOT;   /* bitwise not */
    if  (strcmp(op_code, "LDI") == 0)    return OP_LDI;   /* load indirect */
    if  (strcmp(op_code, "STI") == 0)    return OP_STI;   /* store indirect */
    if  (strcmp(op_code, "JMP") == 0)    return OP_JMP;   /* jump */
    if  (strcmp(op_code, "LEA") == 0)    return OP_LEA;   /* load effective address */
    if  (strcmp(op_code, "TRAP") == 0)   return OP_TRAP;  /* execute trap */
    exit(EXIT_FAILURE);
}

int brval(char* br_code) {
    if (strcmp(br_code, "BRP") == 0)   return 0x1;    /* branch if positive condition codes */
    if (strcmp(br_code, "BRZ") == 0)   return 0x2;    /* branch if zero condition codes     */
    if (strcmp(br_code, "BRN") == 0)   return 0x4;    /* branch if negative condition codes */
    exit(EXIT_FAILURE);
}

int trapval(char* trap_code) {
    if (strcmp(trap_code, "GETC") == 0)  return TRAP_GETC;
    if (strcmp(trap_code, "OUT") == 0)   return TRAP_OUT;
    if (strcmp(trap_code, "PUTS") == 0)  return TRAP_PUTS;
    if (strcmp(trap_code, "IN") == 0)    return TRAP_IN;
    if (strcmp(trap_code, "PUTSP") == 0) return TRAP_PUTSP;
    if (strcmp(trap_code, "HALT") == 0)  return TRAP_HALT;
    exit(EXIT_FAILURE);
}

int regaddress(char* regname) {
    if (strcmp(regname, "R0") == 0) return R_R0;   /* Bin addr: 0000 */
    if (strcmp(regname, "R1") == 0) return R_R1;   /* Bin addr: 0001 */
    if (strcmp(regname, "R2") == 0) return R_R2;   /* Bin addr: 0010 */
    if (strcmp(regname, "R3") == 0) return R_R3;   /* Bin addr: 0011 */
    if (strcmp(regname, "R4") == 0) return R_R4;   /* Bin addr: 0100 */
    if (strcmp(regname, "R5") == 0) return R_R5;   /* Bin addr: 0101 */
    if (strcmp(regname, "R6") == 0) return R_R6;   /* Bin addr: 0110 */
    if (strcmp(regname, "R7") == 0) return R_R7;   /* Bin addr: 0111 */
    exit(EXIT_FAILURE);
}

int const_value(const char* numstring, int base) {
    if (numstring[0] == '#' || numstring[0] == 'x' || numstring[0] == 'X') numstring = numstring+1;
    return strtol(numstring, NULL, base);
}
