# Logo

Recursive-descent parser and interpreter for the LOGO language with visualisation of "turtle graphics" via SDL.

BNF:
```
<PROG>   ::= "START" <INSLST>

<INSLST> ::= "END" | <INS> <INSLST>
<INS>    ::= <FWD> | <RGT> | <COL> | <LOOP> | <SET>

<FWD>    ::= "FORWARD" <VARNUM>
<RGT>    ::= "RIGHT" <VARNUM>
<COL>    ::= "COLOUR" <VAR> | "COLOUR" <WORD>
<LOOP>   ::= "LOOP" <LTR> "OVER" <LST> <INSLST>
<SET>    ::= "SET" <LTR> "(" <PFIX>
<SPEED>  ::= "SPEED" <PERCENT>
  % Affects both the pace of instructions 
  being printed on the command line, as well as the 
  turtles movement in the visualisation. A speed     
  of 0 causes forward intructions to not be executed until 
  speed is set to >=1 (see Instructions/racing.ttl). 
<SQUARE> ::= "SQUARE"
  % Predefined shape
<CIRCLE> ::= "CIRCLE"
  % Predefined shape
<TRIANGLE> ::= "TRIANGLE"
  % Predefined shape


<VARNUM> ::= <VAR> | <NUM>
  % Variables e.g. $A, $B, $Z etc.
<VAR>    ::= $<LTR>
  % One Uppercase letter
<LTR>    ::= A, B ... Z
  % Any valid double (as defined by scanf("%lf"...)
<NUM>    ::= 10 or -17.99 etc.
  % Valid unsigned (as defined by scanf("%u"...) between 0 and 100
<PERCENT> ::= 0, 10, ... 100

  % A single word (as defined by scanf("%s"...) with double-quotes around it
  % Valid colours include "BLACK", "RED", "GREEN", "BLUE",
  % "YELLOW", "CYAN", "MAGENTA", "WHITE"
<WORD>   ::=  "RED" "BLUE" "HELLO!" "178"

<LST>    ::= "{" <ITEMS>
<ITEMS>  ::= "}" | <ITEM> <ITEMS>
<ITEM>   ::= <VARNUM> | <WORD> 

<PFIX>   ::= ")" | <OP> <PFIX> | <VARNUM> <PFIX>
  % A single mathematical operation character
<OP>     ::= + - / *
```
Visualisation:
  - Pixel art turtle sprites in all valid colours (see Sprites/)
  - Turtle rotates according to movement direction, smoothly
    walks to the new position and pauses for 1s
  - Works with all provided ttl files but demonstrates with 
    shapes.ttl which contains shapes and speed instructions
  - Start program with 'make render' and quit with the 'esc' key 

Tests:
  - parse.c is tested with a long list of assert statements and
    interpret.c is tested with the SPUT unit testing framework.
  - Each function is tested against grammar key words like "START",
    as well as values defined by <NUM> etc, braces, operators 
    and valid colours. If the next expected token is not found,a 
    non-zero exit is made describing the error.Correct name/number of command line arguments
  - Additional tests for empty files, sucessfull memory allocation,
    "unexpected" instructions like negative forward values and 
     degrees greater than 360 (see Instructions/shapes.ttl)
  - Instructions continue to be interpreted when the turtle is out of 
    bounds however its position is not executed until it is within bounds again.
    Incorrect postfix expressions get ignored as does other 
    invalid grammar (see Instructions/garbage.ttl). The interpreter 
    instead searches for the next valid instruction until the
    last word is reached.
