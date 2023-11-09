# Required Word Set 
 
The words of the Required Word Set are grouped to show like 
characteristics. No implementation requirements should be inferred 
from this grouping. 
 
## Nucleus Words 
 
    !              n addr --                     112            "store" 
         Store n at addr. 
 
    *              n1 n2 -- n3                   138            "times" 
         Leave the arithmetic product of n1 times n2. 
 
    */             n1 n2 n3 -- n4                220     "times-divide" 
         Multiply  n1  by  n2,  divide the result by n3 and  leave  the 
         quotient n4.   n4 is rounded toward zero.   The product of  n1 
         times  n2  is maintained as an intermediate 32-bit  value  for 
         greater precision than the otherwise equivalent  sequence:  n1 
         n2 * n3 / 
 
    */MOD          n1 n2 n3 -- n4 n5             192 "times-divide-mod" 
         Multiply  n1  by  n2,  divide the result by n3 and  leave  the 
         remainder n4 and quotient n5.   A 32-bit intermediate  product 
         is used as for */ .  The remainder has the same sign as n1. 
 
    +              n1 n2 -- n3                   121             "plus" 
         Leave the arithmetic sum of n1 plus n2. 
 
    +!             n addr --                     157       "plus-store" 
         Add  n to the 16-bit value at the address,  by the  convention 
         given for + . 
 
    -              n1 n2 -- n3                   134            "minus" 
         Subtract n2 from n1 and leave the difference n3. 
 
    /              n1 n2 -- n3                   178           "divide" 
         Divide  n1  by n2 and leave the quotient n3.   n3  is  rounded 
         toward zero. 
 
    /MOD           n1 n2 -- n3 n4                198       "divide-mod" 
         Divide  n1 by n2 and leave the remainder n3 and  quotient  n4. 
         n3 has the same sign as n1. 
 
    0<             n -- flag                     144        "zero-less" 
         True if n is less than zero (negative) 
 
    0=             n -- flag                     180      "zero-equals" 
         True if n is zero. 
 
    0>             n -- flag                     118     "zero-greater" 
         True if n is greater than zero. 
 
    1+             n -- n+1                      107         "one-plus" 
         Increment n by one, according to the operation of + . 
 
    1-             n -- n-1                      105        "one-minus" 
         Decrement n by one, according to the operation of - . 
 
    2+             n -- n+2                      135         "two-plus" 
         Increment n by two, according to the operation of + . 
 
    2-             n -- n-1                      129        "two-minus" 
         Decrement n by two, according to the operation of - . 
 
    <              n1 n2 -- flag                 139        "less-than" 
         True if n1 is less than n2. 
 
         -32768 32767 <  must return true. 
         -32768 0 <  must be distinguished. 
 
    =              n1 n2 -- flag                 173           "equals" 
         True if n1 is equal to n2. 
 
    >              n1 n2 -- flag                 102     "greater-than" 
         True if n1 is greater than n2. 
 
    >R             n --                          C,200           "to-r" 
         Transfer n to the return stack.   Every >R must be balanced by 
         a  R> in the same control structure nesting level of a  colon- 
         definition. 
 
    ?DUP           n -- n ( n )                  184       "query-dupe" 
         Duplicate n if it is non-zero. 
 
    @              addr -- n                     199            "fetch" 
         Leave on the stack the number contained at addr. 
 
    ABS            n1 -- n1                      108         "absolute" 
         Leave the absolute value of a number. 
 
    AND            n1 n2 -- n3                   183 
         Leave the bitwise logical 'and' of n1 and n2. 
 
    C!             n addr --                     219          "c-store" 
         Store the least significant 8-bits of n at addr. 
 
    C@             addr -- byte                  156          "c-fetch" 
         Leave  on  the stack the contents of the byte  at  addr  (with 
         higher bits zero, in a 16-bit field). 
 
    CMOVE          addr1 addr2 n --              153           "c-move" 
         Move  n  bytes  beginning  at address  addr1  to  addr2.   The 
         contents  of  addr1  is moved  first  proceeding  toward  high 
         memory.  If n is zero nothing is moved. 
 
    D+             d1 d2 --- d3                  241           "d-plus" 
         Leave the arithmetic sum of d1 plus d2. 
 
    D<             d1 d2 -- flag                 244      "d-less-than" 
         True if d1 is less than d2. 
 
    DEPTH          -- n                          238 
         Leave the number of the quantity of 16-bit values contained in 
         the data stack, before n added. 
 
    DNEGATE        d -- -d                       245         "d-negate" 
         Leave the two's complement of a double number. 
 
    DROP           n --                          233 
         Drop the top number from the stack. 
 
    DUP            n -- n n                      205             "dupe" 
         Leave a copy of the top stack number. 
 
    EXECUTE        addr --                       163 
 
         Execute the dictionary entry whose compilation address is on 
         the stack. 
 
    EXIT                                         C,117 
         When compiled within a colon-definition,  terminate  execution 
         of that definition,  at that point.   May not be used within a 
         DO...LOOP. 
 
    FILL           addr n byte --                234 
         Fill  memory beginning at address with a sequence of n  copies 
         of  byte.   If the quantity n is less than or equal  to  zero, 
         take no action. 
 
    I              -- n                          C,136 
         Copy the loop index onto the data stack.   May only be used in 
         the form: 
              DO ... I ... LOOP   or 
              DO ... I ... +LOOP 
 
    J              -- n                          C,225 
         Return  the index of the next outer loop.   May only  be  used 
         within a nested DO-LOOP in the form: 
              DO ... DO ... J ... LOOP ... LOOP 
 
    LEAVE                                        C,213 
         Force  termination  of a DO-LOOP at the next LOOP or +LOOP  by 
         setting  the  loop  limit equal to the current  value  of  the 
         index.   The  index itself remains  unchanged,  and  execution 
         proceeds   normally   until  the  loop  terminating  word   is 
         encountered. 
 
    MAX            n1 n2 -- n3                   218              "max" 
         Leave the greater of two numbers. 
 
    MIN            n1 n2 -- n3                   127              "min" 
         Leave the lesser of two numbers. 
 
    MOD            n1 n2 -- n3                   104 
         Divide n1 by n2,  leaving the remainder n3, with the same sign 
         as n1. 
 
    MOVE           addr1 addr2 n --              113 
         Move the specified quantity n of 16-bit memory cells beginning 
         at addr1 into memory at addr2.  The contents of addr1 is moved 
         first.  If n is negative or zero, nothing is moved. 
 
    NEGATE         n -- -n                       177 
         Leave the two's complement of a number,  i.e.,  the difference 
         of zero less n. 
 
    NOT            flag1 -- flag2                165 
         Reverse the boolean value of flag1.  This is identical to 0=. 
 
    OR             n1 n2 -- n3                   223 
         Leave the bitwise inclusive-or of two numbers. 
 
    OVER           n1 n2 -- n1 n2 n1             170 
         Leave a copy of the second number on the stack. 
 
    PICK           n1 -- n2                      240 
         Return the contents of the n1-th stack value,  not counting n1 
         itself.  An error condition results for n less than one. 
              2 PICK  is equivalent to OVER.  {1..n} 
 
    R>             -- n                          C,110         "r-from" 
         Transfer n from the return stack to the data stack. 
 
    R@             -- n                          C,228        "r-fetch" 
         Copy the number on top of the return stack to the data stack. 
 
    ROLL           n --                          236 
         Extract  the  n-th stack value to the top of  the  stack,  not 
         counting  n  itself,  moving  the remaining  values  into  the 
         vacated position.   An error condition results for n less than 
         one.  {1..n} 
              3  ROLL  =  ROT 
              1  ROLL  =  null operation 
 
    ROT            n1 n2 n3 -- n2 n3 n1          212             "rote" 
         Rotate the top three values, bringing the deepest to the top. 
 
    SWAP           n1 n2 -- n2 n1                230 
         Exchange the top two stack values. 
 
    U*             un1 un2 -- ud3                242          "u-times" 
         Perform an unsigned multiplication of un1 by un2,  leaving the 
         double number product ud3.  All values are unsigned. 
 
    U/MOD          ud1 un2 -- un3 un4            243     "u-divide-mod" 
         Perform  the  unsigned division of double number ud1  by  un2, 
         leaving the remainder un3,  and the quotient un4.   All values 
         are unsigned. 
 
    U<             un1 un2 -- flag               150      "u-less-than" 
         Leave the flag representing the magnitude comparison of un1  < 
         un2 where un1 and un2 are treated as 16-bit unsigned integers. 
 
    XOR            n1 n2 -- n3                   174             "x-or" 
         Leave the bitwise exclusive-or of two numbers. 
 
 
## Run-Time Code for Compiling Words 
 
These lower  case entries refer to just  the  run-time  code 
corresponding to a compiling word: 
 
    begin 
    colon 
    constant 
    create 
    do 
    does> 
    else 
    if 
    literal 
    loop 
    repeat 
    semicolon 
    then 
    until 
    variable 
    while 
 
## Interpreter Words 
 
    #              ud1 -- ud2                    158            "sharp" 
         Generate  from  an unsigned double number d1,  the next  ASCII 
         character which is placed in an output string.   Result d2  is 
         the  quotient after division by BASE is maintained for further 
         processing.  Used between <# and #> . 
 
    #>             d -- addr n                   190    "sharp-greater" 
         End pictured numeric output conversion.   Drop d,  leaving the 
         text address, and character count, suitable for TYPE. 
 
    #S             ud -- 0 0                     209          "sharp-s" 
         Convert  all digits of an unsigned 32-bit  number  ud,  adding 
         each  to the pictured numeric output text,  until remainder is 
         zero.   A  single  zero is added to the output string  if  the 
         number was initially zero.  Use only between <# and #>. 
 
    '              -- addr                       I,171           "tick" 
         Used in the form: 
              '  <name> 
         If  executing,  leave the parameter field address of the  next 
         word accepted from the input stream.   If  compiling,  compile 
         this  address  as a literal;  later execution will place  this 
         value  on the stack.   An error condition exists if not  found 
         after a search of the CONTEXT and FORTH vocabularies.   Within 
         a  colon-definition  '  <name> is identical to [  '  <name>  ] 
         LITERAL. 
 
    (              --                            I,122          "paren" 
         Used in the form: 
              ( ccc) 
         Accept  and ignore comment characters from the  input  stream, 
         until  the  next  right parenthesis.   As  a  word,  the  left 
         parenthesis  must be followed by one blank.   It may freely be 
         used while executing or compiling.   An error condition exists 
         if the input stream is exhausted before the right parenthesis. 
 
    -TRAILING      addr n1 -- addr n2            148    "dash-trailing" 
         Adjust  the character count n1 of a text string  beginning  at 
         addr  to  exclude trailing blanks,  i.e.,  the  characters  at 
         addr+n2 to addr+n1-1 are blanks.  An error condition exists if 
         n1 is negative. 
 
    .              n --                          193              "dot" 
         Display  n converted according to BASE in a free field  format 
         with one trailing blank.  Display only a negative sign. 
 
    79-STANDARD                                  119 
         Execute assuring that a FORTH-79 Standard system is available, 
         otherwise an error condition exists. 
 
    <#                                           169       "less-sharp" 
         Initialize pictured numeric output.  The words: 
              #  #>  #S  <#  HOLD  SIGN 
         can   be used to specify the conversion of a  double-precision 
         number into an ASCII character string stored in  right-to-left 
         order. 
 
    >IN            -- addr                       U,201          "to-in" 
         Leave  the  address of a variable which contains  the  present 
         character offset within the input stream {{0..1023}} 
         See:  WORD  (  ."  FIND 
 
    ?              addr --                       194    "question-mark" 
         Display the number at address, using the format of "." . 
 
    ABORT                                        101 
         Clear  the  data and return stacks,  setting  execution  mode. 
         Return control to the terminal. 
 
    BASE           -- addr                       U,115 
         Leave  the address of a variable containing the current input- 
         output numeric conversion base.  {{2..70}} 
 
    BLK            -- addr                       U,132          "b-l-k" 
         Leave  the address of a variable containing the number of  the 
         mass storage block being interpreted as the input stream.   If 
         the  content  is  zero,  the input stream is  taken  from  the 
         terminal. 
 
    CONTEXT        -- addr                       U,151 
         Leave  the address of a variable specifying the vocabulary  in 
         which   dictionary   searches   are   to   be   made,   during 
         interpretation of the input stream. 
 
    CONVERT        d1 addr1 -- d2 addr2          195 
         Convert  to the equivalent stack number the text beginning  at 
         addr1+1  with regard to BASE.   The new value  is  accumulated 
         into double number d1, being left as d2.  addr2 is the address 
         of the first non-convertible character. 
 
    COUNT          addr -- addr+1 n              159 
         Leave  the  address  addr+1 and the character  count  of  text 
         beginning  at addr.   The first byte at addr must contain  the 
         character count n.  Range of n is {0..255}. 
 
    CR                                           160              "c-r" 
         Cause  a carriage-return and line-feed to occur at the current 
         output device. 
 
    CURRENT        -- addr                       U,137 
         Leave the address of a variable specifying the vocabulary into 
         which new word definitions are to be entered. 
 
    DECIMAL                                      197 
         Set the input-output numeric conversion base to ten. 
 
    EMIT           char --                       207 
         Transmit character to the current output device. 
 
    EXPECT         addr n --                     189 
         Transfer  characters  from  the terminal  beginning  at  addr, 
         upward,  until a "return" or the count of n has been received. 
         Take  no action for n less than or equal to zero.   One or two 
         nulls are added at the end of text. 
 
    FIND           -- addr                       203 
         Leave the compilation address of the next word name,  which is 
         accepted from the input stream.   If that word cannot be found 
         in  the  dictionary after a search of CONTEXT and FORTH  leave 
         zero. 
 
    FORTH                                        I,187 
         The name of the primary vocabulary.  Execution makes FORTH the 
         CONTEXT  vocabulary.   New  definitions become a part  of  the 
         FORTH  until  a differing CURRENT vocabulary  is  established. 
         User  vocabularies  conclude by 'chaining'  to  FORTH,  so  it 
         should  be  considered that FORTH is 'contained'  within  each 
         user's vocabulary. 
 
    HERE           -- addr                       188 
         Return the address of the next available dictionary location. 
 
    HOLD           char --                       175 
         Insert char into a pictured numeric output string.   May  only 
         be used between <# and #> . 
 
    KEY            -- char                       100 
         Leave the ASCII value of the next available character from the 
         current input device. 
 
    PAD            -- addr                       226 
         The  address of a scratch area used to hold character  strings 
         for  intermediate processing.   The minimum capacity of PAD is 
         64 characters (addr through addr+63). 
 
    QUERY                                        235 
         Accept input of up to 80 characters (or until a 'return') from 
         the operator's terminal, into the terminal input buffer.  WORD 
         may  be  used  to accept text from this buffer  as  the  input 
         stream, by setting >IN and BLK to zero. 
 
    QUIT                                         211 
         Clear  the return stack,  setting execution mode,  and  return 
         control to the terminal.  No message is given. 
 
    SIGN           n --                          C,140 
         Insert  the ASCII "-" (minus sign) into the  pictured  numeric 
         output string, if n is negative. 
 
    SPACE                                        232 
         Transmit an ASCII blank to the current output device. 
 
    SPACES         n --                          231 
         Transmit  n  spaces  to the current output  device.   Take  no 
         action for n of zero or less. 
 
    TYPE           addr n --                     222 
         Transmit  n  characters beginning at address  to  the  current 
         output device.  No action takes place for n less than or equal 
         to zero. 
 
    U.             un --                         106            "u-dot" 
         Display  un converted according to BASE as an unsigned number, 
         in a free-field format, with one trailing blank. 
 
    WORD           char -- addr                  181 
         Receive  characters  from the input stream until the  non-zero 
         delimiting  character  is encountered or the input  stream  is 
         exhausted,  ignoring leading delimiters.   The characters  are 
         stored  as  a  packed string with the character count  in  the 
         first  character position.   The actual delimiter  encountered 
         (char  or  null)  is stored at the end of  the  text  but  not 
         included  in the count.   If the input stream was exhausted as 
         WORD is called,  then a zero length will result.   The address 
         of the beginning of this packed string is left on the stack. 
 
 
## Compiler Words 
 
    +LOOP          n --                          I,C,141    "plus-loop" 
         Add  the  signed  increment  n to the  loop  index  using  the 
         convention for +,  and compare the total to the limit.  Return 
         execution to the corresponding DO until the new index is equal 
         to or greater than the limit (n>0),  or until the new index is 
         less  than the limit (n<0).   Upon the exiting from the  loop, 
         discard  the  loop control  parameters,  continuing  execution 
         ahead.   Index and  limit are  signed  integers in  the  range 
         {-32,768..32,767}. 
 
         (Comment:  It is a historical precedent that the limit for n<0 
         is irregular.   Further consideration of the characteristic is 
         unlikely.) 
 
    ,              n --                          143            "comma" 
         Allot two bytes in the dictionary, storing n there. 
 
    ."                                           I,133      "dot-quote" 
         Interpreted or used in a colon definition in the form: 
              ." ccc" 
         Accept the following text from the input stream, terminated by 
         "  (double-quote).   If executing,  transmit this text to  the 
         selected output device.   If compiling,  compile so that later 
         execution  will  transmit  the  text to  the  selected  output 
         device.   At least 127 characters are allowed in the text.  If 
         the  input stream is exhausted before the terminating  double- 
         quote, an error condition exists. 
 
    :                                            116            "colon" 
         A defining word executed in the form: 
              :  <name>  ...  ; 
         Select  the  CONTEXT  vocabulary to be identical  to  CURRENT. 
         Create  a  dictionary entry for <name>  in  CURRENT,  and  set 
         compile   mode.    Words  thus  defined  are  called   'colon- 
         definitions'.   The  compilation addresses of subsequent words 
         from the input stream which are not immediate words are stored 
         in  the  dictionary  to  be  executed  when  <name>  is  later 
         executed.  IMMEDIATE words are executed as encountered. 
 
         If a word is not found after a search of the CONTEXT and FORTH 
         vocabularies,  conversion and compilation of a literal  number 
         is attempted,  with regard to the current BASE;  that failing, 
         an error condition exists . 
 
    ;                                            I,C,196   "semi-colon" 
         Terminate  a  colon  definition  and  stop  compilation.    If 
         compiling  from mass storage and the input stream is exhausted 
         before encountering ; an error condition exists. 
 
    ALLOT          n --                          154 
         Add  n  bytes  to the parameter field  of  the  most  recently 
         defined word. 
 
    BEGIN                                        I,C,147 
         Used in a colon-definition in the form: 
              BEGIN ... flag UNTIL   or 
              BEGIN ... flag WHILE ... REPEAT 
         BEGIN  marks  the  start of a  word  sequence  for  repetitive 
         execution.   A BEGIN-UNTIL loop will be repeated until flag is 
         true.   A  BEGIN-WHILE-REPEAT loop will be repeated until flag 
         is  false.   The words after UNTIL or REPEAT will be  executed 
         when  either loop is finished.   flag is always dropped  after 
         being tested. 
 
    COMPILE                                      C,146 
         When  a  word containing COMPILE executes,  the  16-bit  value 
         following   the  compilation  address  of  COMPILE  is  copied 
         (compiled) into the dictionary.   i.e.,  COMPILE DUP will copy 
         the compilation address of DUP. 
              COMPILE  [ 0 , ]   will copy zero. 
 
    CONSTANT       n --                          185 
         A defining word used in the form: 
              n CONSTANT <name> 
         to  create  a dictionary entry for <name>,  leaving n  in  its 
         parameter  field.   When <name> is later executed,  n will  be 
         left on the stack. 
 
    CREATE                                       239 
         A defining word used in the form: 
              CREATE  <name> 
         to  create a dictionary entry for <name>,  without  allocating 
         any  parameter  field memory.   When  <name>  is  subsequently 
         executed,  the address of the first byte of <name>'s parameter 
         field is left on the stack. 
 
    DEFINITIONS                                  155 
         Set  CURRENT  to  the CONTEXT vocabulary  so  that  subsequent 
         definitions  will  be  created in  the  vocabulary  previously 
         selected as CONTEXT. 
 
    DO             n1 n2 --                      I,C,142 
         Used in a colon-definition: 
              DO ... LOOP   or 
              DO ... +LOOP 
         Begin a loop which will terminate based on control parameters. 
         The loop index begins at n2, and terminates based on the limit 
         n1.   At LOOP or +LOOP, the index is modified by a positive or 
         negative  value.   The range of a DO-LOOP is determined by the 
         terminating word.   DO-LOOP may be nested.  Capacity for three 
         levels  of  nesting  is specified as a  minimum  for  standard 
         systems. 
 
    DOES>                                        I,C,168         "does" 
         Define  the run-time action of a word created by a  high-level 
         defining word.  Used in the form: 
              :  <name> ... CREATE ... DOES> ... ; 
              and then   <namex>  <name> 
         Marks  the  termination of the defining part of  the  defining 
         word <name> and begins the defining of the run-time action for 
         words  that will later be defined by <name>.   On execution of 
         <namex>  the  sequence  of  words  between  DOES>  and  ;  are 
         executed, with the address of <namex>'s parameter field on the 
         stack. 
 
    ELSE           --                            I,C,167 
         Used in a colon-definition in the form: 
              IF ... ELSE ... THEN 
         ELSE executes after the true part following IF.   ELSE  forces 
         execution  to skip till just after THEN.   It has no effect on 
         the stack.  (see IF) 
 
    FORGET                                       186 
         Execute in the form: 
              FORGET  <name> 
         Delete  from  the dictionary <name> (which is in  the  CURRENT 
         vocabulary)  and  all  words added  to  the  dictionary  after 
         <name>,  regardless  of  their vocabulary.   Failure  to  find 
         <name> in CURRENT or FORTH is an error condition. 
 
    IF             flag --                       I,C,210 
         Used in a colon-definition in the form: 
              flag  IF ... ELSE ... THEN   or 
              flag  IF ... THEN 
         If  flag is true,  the words following IF are executed and the 
         words following ELSE are skipped.   The ELSE part is optional. 
         If flag is false, words between IF and ELSE, or between IF and 
         THEN  (when  no  ELSE is  used),  are  skipped.   IF-ELSE-THEN 
         conditionals may be nested. 
 
    IMMEDIATE                                    103 
         Marks the most recently made dictionary entry as a word  which 
         will  be  executed when encountered during compilation  rather 
         than compiled. 
 
    LITERAL        n --                          I,215 
         If  compiling,  then  compile  the stack value n as  a  16-bit 
         literal, which when later executed, will leave n on the stack. 
 
    LOOP                                         I,C,124 
         Increment  the DO-LOOP index by one,  terminating the loop  if 
         the  new  index is equal to or greater than  the  limit.   The 
         limit  and  index  are signed numbers in  the  range  {-32,768 
         ..32,767}. 
 
    REPEAT         --                            I,C,120 
         Used in a colon-definition in the form: 
              BEGIN ... WHILE ... REPEAT 
         At  run-time,  REPEAT returns to just after the  corresponding 
         BEGIN. 
 
    STATE          -- addr                       U,164 
         Leave  the address of the variable containing the  compilation 
         state.  A non-zero content indicates compilation is occurring, 
         but the value itself may be installation dependent. 
 
    THEN                                         I,C,161 
         Used in a colon-definition in the form: 
              IF ... ELSE ... THEN   or 
              IF ... THEN 
         THEN  is  the point where execution resumes after ELSE  or  IF 
         (when no ELSE is present). 
 
    UNTIL          flag --                       I,C,,237 
         Within a colon-definition, mark the end of a BEGIN-UNTIL loop, 
         which will terminate based on flag.  If flag is true, the loop 
         is  terminated.   If flag is false,  execution returns to  the 
         first word after BEGIN.  BEGIN-UNTIL structures may be nested. 
 
    VARIABLE                                     227 
         A defining word executed in the form: 
              VARIABLE  <name> 
         to  create a dictionary entry for <name> and allot  two  bytes 
         for  storage  in the parameter field.   The  application  must 
         initialize  the stored value.   When <name> is later executed, 
         it will place the storage address on the stack. 
 
    VOCABULARY                                   208 
         A defining word executed in the form: 
              VOCABULARY  <name> 
         to  create (in the CURRENT vocabulary) a dictionary entry  for 
         <name>,   which   specifies  a  new  ordered  list   of   word 
         definitions.   Subsequent execution of <name> will make it the 
         CONTEXT   vocabulary.    When   <name>  becomes  the   CURRENT 
         vocabulary (see DEFINITIONS), new definitions will be  created 
         in that list. 
 
         In lieu of any further specification, new vocabularies 'chain' 
         to  FORTH.   That  is,  when  a dictionary  search  through  a 
         vocabulary is exhausted, FORTH will be searched. 
 
    WHILE          flag --                       I,C,149 
         Used in the form: 
              BEGIN ... flag WHILE ... REPEAT 
         Select conditional execution based on flag.   On a true  flag, 
         continue execution through to REPEAT,  which then returns back 
         to just after BEGIN.   On a false flag, skip execution to just 
         after REPEAT, exiting the structure. 
 
    [                                            I,125   "left-bracket" 
         End the compilation mode.   The text from the input stream  is 
         subsequently executed.  See ] 
 
    [COMPILE]                                I,C,179  "bracket-compile" 
         Used in a colon-definition in the form: 
              [COMPILE] <name> 
         Forces  compilation  of  the  following  word.    This  allows 
         compilation  of  an IMMEDIATE word when it would otherwise  be 
         executed. 
 
    ]                                            126    "right-bracket" 
         Sets the compilation mode.   The text from the input stream is 
         subsequently compiled.  See [ 
 
## Device Words 
 
    BLOCK          n -- addr                     191 
         Leave the address of the first byte in block n.   If the block 
         is not already in memory,  it is transferred from mass storage 
         into whichever memory buffer has been least recently accessed. 
         If  the  block occupying that buffer has  been  UPDATEd  (i.e. 
         modified), it is rewritten onto mass storage before block n is 
         read  into the buffer.   n is an unsigned number.   If correct 
         mass storage read or write is not possible, an error condition 
         exists.  Only data within the latest block referenced by BLOCK 
         is valid by byte address, due to sharing of the block buffers. 
 
    BUFFER         n -- addr                     130 
         Obtain the next block buffer,  assigning it to block  n.   The 
         block  is  not  read  from  mass  storage.   If  the  previous 
         contents  of  the  buffer has been marked as  UPDATEd,  it  is 
         written to mass storage.   If correct writing to mass  storage 
         is not possible,  an error condition exists.  The address left 
         is the first byte within the buffer for data storage.  n is an 
         unsigned number. 
 
    EMPTY-BUFFERS                                145 
         Mark all block buffers as empty, without necessarily affecting 
         their actual contents.  UPDATEd blocks are not written to mass 
         storage. 
 
    LIST           n --                          109 
         List  the ASCII symbolic contents of screen n on  the  current 
         output device, setting SCR to contain n.  n is unsigned. 
 
    LOAD           n --                          202 
         Begin  interpretation  of  screen n by  making  it  the  input 
         stream;  preserve  the  locators of the present  input  stream 
         (from  >IN  and  BLK).   If interpretation is  not  terminated 
         explicitly  it  will be terminated when the  input  stream  is 
         exhausted.    Control   then  returns  to  the  input   stream 
         containing  LOAD,  determined by the input stream locators >IN 
         and BLK. 
 
    SAVE-BUFFERS                                 221 
         Write  all  blocks to mass-storage that have been  flagged  as 
         UPDATEd.   An  error condition results if mass-storage writing 
         is not completed. 
 
    SCR            -- addr                       U,217 
         Leave  the address of a variable containing the number of  the 
         screen most recently listed. 
 
    UPDATE                                       229 
         Mark  the  most recently referenced block  as  modified.   The 
         block  will subsequently be automatically transferred to  mass 
         storage  should  its memory buffer be needed for storage of  a 
         different block, or upon execution of SAVE-BUFFERS. 
 
 
# ASSEMBLER WORD SET 
 
    ;CODE                                    C,I,206  "semi-colon-code" 
        Used in the form: 
             : <name> ...  ;CODE 
        Stop  compilation  and  terminate  a  defining  word   <name>. 
        ASSEMBLER  becomes  the CONTEXT vocabulary.   When  <name>  is 
        executed in the form: 
             <name>  <namex> 
        to  define the new <namex>,  the execution address of  <namex> 
        will  contain  the address of the code sequence following  the 
        ;CODE  in <name>.   Execution of any <namex> will  cause  this 
        machine code sequence to be executed. 
     
    ASSEMBLER                                    I,166 
        Select assembler as the CONTEXT vocabulary. 
     
    CODE                                         111 
        A defining word used in the form: 
             CODE  <name> ... END-CODE 
        to  create  a dictionary entry for <name> to be defined  by  a 
        following  sequence  of assembly  language  words.   ASSEMBLER 
        becomes the context vocabulary. 
 
    END-CODE 
        Terminate a code definition,  resetting the CONTEXT vocabulary 
        to the CURRENT vocabulary.   If no errors have  occurred,  the 
        code definition is made available for use. 
 
# REFERENCE WORD SET 
 
The Reference Word Set contain both Standard Word Definitions (with 
serial number identifiers in the range 100 through 999), and 
uncontrolled word definitions. 
 
Uncontrolled definitions are included for public reference of words 
that have present usage and/or are candidates for future 
standardization. 
 
No restrictions are placed on the definition or usage of uncontrolled 
words. However, use of these names for procedures differing from the 
given definitions is discouraged. 
 
    !BITS          n1 addr n2 --                           "store-bits" 
         Store the value of n1  masked by n2 into the equivalent masked 
         part  of the contents of addr,  without affecting bits outside 
         the mask. 
 
    **             n1 n2 -- n3                                  "power" 
         Leave the value of n1 to the power n2. 
 
    +BLOCK         n1 -- n2                                "plus-block" 
         Leave the sum  of  n1  plus  the number  of  the  block  being 
         interpreted, n1 and n2 are unsigned. 
 
    -'             -- ( addr )  flag                        "dash-tick" 
         Used in the form: 
              -' <name> 
         Leave the parameter field of  <name>  beneath zero  (false) if 
         the name can be found in the  CONTEXT  vocabulary;  leave only 
         true if not found. 
 
    -->            I,131     "next-block" 
         Continue interpretation on the next sequential block.   May be 
         used within a colon definition that crosses a block boundary. 
 
    -MATCH         addr1 n1 addr2 n2 -- addr3 f            "dash-match" 
         Attempt  to  find the n2-character string beginning  at  addr2 
         somewhere  in  the  n1-character string  beginning  at  addr1. 
         Return  the last+1 character address addr3 of the match  point 
         and a flag which is zero if a match exists. 
 
    -TEXT          addr1 n1 addr2 -- n2                     "dash-text" 
         Compare two strings over the length n1 beginning at addr1  and 
         addr2.   Return  zero if the strings are equal.   If  unequal, 
         return   n2,   the  difference  between  the  last  characters 
         compared:  addr1(i) - addr2(i) 
 
    .R             n1 n2 --                                     "dot-r" 
         Print n1 right aligned in a field of n2 characters,  according 
         to  BASE.   If  n2  is less than  1,  no  leading  blanks  are 
         supplied. 
 
    /LOOP          n --                                       "up-loop" 
         A DO-LOOP terminating word.   The loop index is incremented by 
         the  unsigned  magnitude  of n.   Until  the  resultant  index 
         exceeds  the  limit,  execution  returns  to  just  after  the 
         corresponding   DO:   otherwise,   the  index  and  limit  are 
         discarded.  Magnitude logic is used. 
 
    1+!            addr --                             "one-plus-store" 
         Add one to the 16-bit contents at addr. 
 
    1-!            addr --                            "one-minus-store" 
         Subtract 1 from the 16-bit contents at addr. 
 
    2*             n1 -- n2                                 "two-times" 
         Leave 2*(n1). 
 
    2/             n1 -- n2                                "two-divide" 
         Leave (n1)/2. 
 
    ;:             C   "semi-colon-oolon" 
         Used to specify a new defining word: 
              : <name>  ... 
                   ;:   ...  ; 
              <name>  <namex> 
         When <name> is executed, it creates an entry for the new  word 
         <namex>.  Later execution of <namex> will execute the sequence 
         of  words between  ;:  and  ; , with the address of the  first 
         (if any) parameters associated with <namex> on the stack. 
 
    ;S             "semi-s" 
         Stop interpretation of a block.  For execution only. 
 
    <>             n1 n2 -- flag                            "not-equal" 
         Leave true if n1 is not equal to n2. 
 
    <BUILDS        C           "builds" 
         Used in conjunction with DOES> in defining words, in the form: 
              : <name>  . . .  <BUILDS  . . . 
                   DOES>  ...    ; 
         and then  <name>  <namex> 
         When  <name> executes, <BUILDS creates a dictionary entry  for 
         the  new <namex>.  The sequence of words between  <BUILDS  and 
         DOES> established a parameter field for <namex>.  When <namex> 
         is later executed, the sequence of words following DOES>  will 
         be  executed, with the parameter field address of  <namex>  on 
         the data stack. 
 
    <CMOVE         addr1 addr2 n --                    "reverse-c-move" 
         Copy  n bytes beginning at addr1 to addr2.   The move proceeds 
         within the bytes from high memory toward low memory. 
 
    ><             n1 -- n2                                 "byte-swap" 
         Swap the high and low bytes within n1. 
 
    >MOVE<         addr1 addr2 n --                    "byte-swap-move" 
         Move  n  bytes beginning at addr1 to the memory  beginning  at 
         addr2.   During  this  move, the order of each  byte  pair  is 
         reversed. 
 
    @BITS          addr n1 -- n2                          "fetch-bits" 
         Return the 16-bits at addr masked by n1. 
 
    ABORT"         flag --                       I,C      "abort-quote" 
         Used in a colon-definition in the form: 
                   ABORT" stack empty" 
         If the flag is true,  print the following text,  till ".  Then 
         execute ABORT. 
 
    AGAIN          I,C,114 
         Effect  an  unconditional jump back to the start of  a  BEGIN- 
         AGAIN loop. 
 
    ASCII          -- char (executing) 
                   --      (compiling)           I,C 
         Leave  the  ASCII  character  value  of  the  next   non-blank 
         character in the input stream.   If compiling, compile it as a 
         literal, which will be later left when executed. 
 
    ASHIFT         n1 n2 -- n3 
         Shift  the  value  n1 arithemetically n2 bits left  if  n2  is 
         positive,  shifting  zeros  into  the  least-significant   bit 
         positions.   If  n2 is negative, n1 is  shifted  right.   Sign 
         extension is to be consistent with the processor's  arithmetic 
         shift. 
 
    B/BUF          -- 1024                           "bytes-per-buffer" 
         A constant leaving 1024, the number of bytes per block buffer. 
 
    BELL 
         Activate  a terminal bell or noise-maker as appropriate to the 
         device in use. 
 
 
    BL             -- n                          176              "b-l" 
         Leave the ASCII character value for space (decimal 32). 
 
    BLANKS         addr n --                     152 
         Fill  an area of memory over n bytes with the value for  ASCII 
         blank,  starting at addr.  If n is less than or equal to zero, 
         take no action. 
 
    C,             n --                                       "c-comma" 
         Store  the  low-order  8 bits of n at the  next  byte  in  the 
         dictionary, advancing the dictionary pointer. 
 
    CHAIN 
         Used in the form: 
              CHAIN  <name> 
         Connect  the CURRENT vocabulary to all definitions that  might 
         be  entered  into the vocabulary <name> in  the  future.   The 
         CURRENT  vocabulary may not be FORTH or ASSEMBLER.  Any  given 
         vocabulary may only be chained  once, but may be the object of 
         any  number  of chainings.  For  example,  every  user-defined 
         vocabulary may include the sequence: 
              CHAIN  FORTH 
 
    COM            n1 -- n2 
         Leave the one's complement of n1. 
 
    CONTINUED      n -- 
         Continue  interpretation at block n. 
 
    CUR            -- addr 
         A variable pointing to the physical record number before which 
         the tape is currently positioned.  REWIND sets CUR=1. 
 
    DBLOCK         d -- addr                                  "D-block" 
         Identical to  BLOCK  but with a 32-bit block unsigned number. 
 
    DPL            -- addr                                      "d-p-l" 
         A   variable  containing  the  number  of  places  after   the 
         fractional point for output conversion.  If DPL contains zero, 
         the  last character output will be a decimal point.   No point 
         is  output if DPL contains a negative value.   DPL may be  set 
         explicitly,  or by certain output words,  but is unaffected by 
         number input. 
 
    DUMP           addr n --                     123 
         List the contents of n addresses at addr.  Each line of values 
         may be preceded by the address of the first value. 
 
    EDITOR         I,172 
         The  name  of  the  editor  vocabulary.   When  this  name  is 
         executed, EDITOR is established as the CONTEXT vocabulary. 
 
    END            I,C,224 
         A synonym for UNTIL. 
 
    ERASE          addr n --                     182 
         Fill  an area of memory over n bytes with zeros,  starting  at 
         addr.  If n is zero or less, take no action. 
 
    FLD            -- addr                                      "f-l-d" 
         A variable pointing to the field length reserved for a  number 
         during output conversion. 
 
    FLUSH 
         A synonym for SAVE-BUFFERS. 
 
    H.             n -- 
         Output  n  as a hexadecimal integer with one  trailing  blank. 
         The current base is unchanged. 
 
    HEX            --                            162 
         Set the numeric input-output conversion base to sixteen. 
 
    I'             -- n                          C            "i-prime" 
         Used within a colon-definition executed only from within a DO- 
         LOOP to return the corresponding loop index. 
 
    IFEND 
         Terminate  a  conditional  interpretation  sequence  begun  by 
         IFTRUE. 
 
    IFTRUE         flag -- 
         Begin an 
                   IFTRUE  ...  OTHERWISE  ...  IFEND 
         conditional sequence.  These conditional words operate like 
              IF  ...  ELSE  ...  THEN 
         except  that  they cannot be nested, and are to be  used  only 
         during interpretation.  In conjunction with the words [ and  ] 
         they  may  be  used  within  a  colon-definition  to   control 
         compilation, although they are not to be compiled. 
 
    INDEX          n1 n2 -- 
         Print  the first line of each screen over the range  {n1..n2}. 
         This  displays  the first line of each screen of source  text, 
         which conventionally contains a title. 
 
    INTERPRET 
         Begin interpretation at the character indexed by the  contents 
         of  >IN  relative  to  the  block  number  contained  in  BLK, 
         continuing  until  the  input stream  is  exhausted.   If  BLK 
         contains  zero,  interpret characters from the terminal  input 
         buffer. 
 
    K              -- n                          C 
         Within a nested DO-LOOP,  return the index of the second outer 
         loop. 
 
    LAST           -- addr 
         A variable containing the address of the beginning of the last 
         dictionary  entry  made,  which may not yet be a  complete  or 
         valid entry. 
 
    LINE           n -- addr 
         Leave  the  address of the beginning of line n for the  screen 
         whose number is contained in SCR.  The range of n is {0..15}. 
 
    LINELOAD       n1 n2 -- 
         Begin interpretation at line n1 of screen n2. 
 
    LOADS          n -- 
         A defining word used in the form: 
              n  LOADS   <name> 
         When <name> is subsequently executed, block n will be loaded. 
 
    MAP0           -- addr 
         A variable pointing to the first location in the tape map. 
 
    MASK           n1 -- n2 
         Leave a mask of n1 most significant bits if n1 is positive, or 
         n least significant bits if n1 is negative. 
 
    MS             n -- 
         Delay for approximately n milliseconds. 
 
    NAND           n1 n2 -- n3 
         Hie one's complement of the logical and of n1 and n2. 
 
    NOR            n1 n2 -- n3 
         The one's complement of the logical or of n1 and n2. 
 
    NUMBER         addr -- n 
         Convert  the count and character string at addr,  to a  signed 
         32-bit integer, using the current base.  If numeric conversion 
         is not possible,  an error condition exists.   The string  may 
         contain a preceding negative sign. 
 
    O.             n -- 
         Print n in octal format with one trailing blank.  The value in 
         base is unaffected. 
 
    OCTAL 
         Set the number base to 8. 
 
    OFFSET         -- addr                       128 
         A  variable that contains the offset added to the block number 
         on  the stack by BLOCK to determine the actual physical  block 
         number.   The user must add any desired offset when  utilizing 
         BUFFER. 
 
    OTHERWISE 
         An interpreter-level conditional word.  See IFTRUE. 
 
    PAGE 
         Clear the terminal screen or perform an action suitable to the 
         output device currently active. 
 
    READ-MAP 
         Read   to   the  next  file  mark  on  tape   constructing   a 
         correspondence  table  in memory (the map)  relating  physical 
         block  position  to  logical block number.   The  tape  should 
         normally  be rewound to its load point before executing  READ- 
         MAP. 
 
    REMEMBER 
         A defining word used in the form: 
              REMEMBER <name> 
         Defines a word which, when executed, will cause <name> and all 
         subsequently defined words to be deleted from the  dictionary. 
         <name>  may  be  compiled  into  and  executed  from  a  colon 
         definition.  The sequence 
              DISCARD  REMEMBER  DISCARD 
         provides a standardized preface to any group of transient word 
         definitions. 
 
    REWIND 
         Rewind the tape to its load point, setting CUR=1. 
 
    ROTATE         n1 n2 -- n3 
         Rotate  the value n1 left n2 bits if n2 is positive, right  n2 
         bits  if n2 is negative.  Bits shifted out of one end  of  the 
         cell are shifted back in at the opposite end. 
 
    S0             -- addr                                     "s-zero" 
         Returns the address of the bottom of the stack, when empty. 
 
    SET            n addr -- 
         A defining word used  in the form: 
              n  addr  SET  <name> 
 
         Defines  a  word <name> which, when executed, will  cause  the 
         value  n  to be stored at address. 
 
    SHIFT          n1 n2 -- n3 
         Logical  shift  n1  left  n2  bits if  n2  in positive,  right 
         if  n2  is  negative.   Zeros are  shifted  into  vacated  bit 
         positions. 
 
    SP@            -- addr                       214        "s-p-fetch" 
         Return  the address of the top of the stack,  just before  SP@ 
         was executed. 
 
    TEXT           c -- 
         Accept  characters from the input stream,  as for  WORD,  into 
         PAD, blank-filling the remainder of PAD to 64 characters. 
 
    THRU           n1 n2 -- 
         Load consecutively the blocks from n1 through n2. 
 
    U.R            un1 n2 --                     216          "u-dot-r" 
         Output un1 as an unsigned number right justified in a field n2 
         characters  wide.   If  n2  is  smaller  than  the  characters 
         required for n1, no leading spaces are given. 
 
    USER           n -- 
         A defining word used in the form: 
              n  USER  <name> 
         which  creates a user variable <name>.   n is the cell  offset 
         within  the  user area where the value for <name>  is  stored. 
         Execution  of  <name> leaves its absolute  user  area  storage 
         address. 
 
    VLIST 
         List  the  word names of the CONTEXT vocabulary starting  with 
         the most recent definition. 
 
    WHERE 
         Output information about the status of FORTH,  (e.g., after an 
         error  abort).   Indicate at least the last word compiled  and 
         the last block accessed. 
 
    \LOOP          n --                          I,C        "down-loop" 
         A DO-LOOP terminating word.   The loop index is decremented by 
         n  and  the loop terminated when the resultant  index  becomes 
         equal to or less than the limit.  Magnitude logic is used, and 
         n must be positive. 
 
* WORDS MENTIONED IN FORTH-79 MISSING FROM THE ABOVE 
 
    D+             d1 d2 --- d3                  241           "d-plus" 
    D<             d1 d2 -- flag                 244      "d-less-than" 
    DNEGATE        d -- -d                       245         "d-negate" 
    2!             d addr --                                "two-store" 
    2@             addr -- d                                "two-fetch" 
    2CONSTANT      d --                                  "two-constant" 
    2DROP          d --                                      "two-drop" 
    2DUP           d -- d d                                  "two-dupe" 
    2OVER          d1 d2 -- d1 d2 d1                         "two-over" 
    2ROT           d1 d2 d3 -- d2 d3 d1                      "two-rote" 
    2SWAP          d1 d2 -- d2 d1                            "two-swap" 
    2VARIABLE                                            "two-variable" 
    D-             d1 d2 -- d3                   129          "d-minus" 
    D.             d --                          129            "d-dot" 
    D.R            d n --                                     "d-dot-r" 
    D0=            d -- flag                            "d-zero-equals" 
    D=             d1 d2 -- flag                              "d-equal" 
    DABS           d1 -- d2                                "d-absolute" 
    DMAX           d1 d2 -- d3                                  "d-max" 
    DMIN           d1 d2 -- d3                                  "d-min" 
    DU<            ud1 ud2 -- flag                           "d-u-less" 
