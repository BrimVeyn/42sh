#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Basic N(Non-Terminals) T(Terminals) P(Production Rules) S(Start Symbol) parser
// Using LALR(1) Look-Ahead Left-to-right Rightmost Derivation parser

// With N as { S },
// T as { TOKEN_PLUS : '+' , TOKEN_MULT '*' , TOKEN_ID : [a-zA-Z_][a-zA-Z0-9_]* }
// P as :
//  S --> id | S + S | S * S
// S as S(Start Symbol)
//
// First we compute the LR(0) parsing table
// We need to compute closure and goto 
// For that we need the augmented grammar version of our grammar
//
// Augmented grammar(I) :
// S' --> S
// S  --> id         (1)
// S  --> S + S      (2)
// S  --> S * S      (3)
//                                             [S]  [     ACTION    ]  [GOTO]                                                                                                   
// Closure(I) : --> I0                              id    +    *    $    S                                                                                   
//											----------------------------------------
// S' --> • S                                   0   s2                   1                                                  
// S  --> • id                                  1        s3   s4    a                                                          
// S  --> • S + S                               2        r1   r1                                                                  
// S  --> • S * S                               3   s2                   5                                                     
//                                              4   s2                   6                                      
// goto(I0, S) --> I1                           5        r2   r2                                                                     
// S' --> S •                                   6        r3   r3                                                              
// S --> S • + S                                                                                                             
// S --> S • * S                                s = shift
//                                              a = accept                                                                                                          
// goto(I0, id) --> I2                          r = reduce                                                                                   
// S --> id •                                                                                                             
//                                  ------------------------                                                                          
// goto(I1, +) --> I3                                                                                                                               
// S --> S + • S                    goto(I3, S) --> I5                                                                                                             
// S --> • id                       S --> S + S •                                                                                                               
// S --> • S + S                    
// S --> • S * S                   
//                                                                                                                                 
// goto(I1, *) --> I4               goto(I3, id) => I2                                                                                               
// S --> S * • S                                                                                                                                
// S --> • id                       goto(I4, S) --> I6                                                                                       
// S --> • S + S                    S --> S * S •                                                                                                
// S --> • S * S                                                                                                             
//                                  goto(I4, id) => I2                                                         
//                                                                                                           

typedef enum {
	TOKEN_PLUS,
	TOKEN_MULT,
	TOKEN_ID,
	TOKEN_END,
} Terminals;

typedef struct {
	int state;
	Terminals tok;
} StackEntry;

StackEntry stack[100] = {0};

typedef struct {
	Terminals type;
	char value;
} Token;

Token tokens[10] = {0};

//push = 1, reduce -1

int TokenStates[9][4] = {
	{0},
};

// int main(void) {
// 	tokens[0] = (Token){TOKEN_ID, 'a'};
// 	tokens[1] = (Token){TOKEN_PLUS, '+'};
// 	tokens[2] = (Token){TOKEN_ID, 'b'};
//
// 	parse();
// }
