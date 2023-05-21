    ;Simple program to test TRAPS: PUTS GETC OUT HALT
    .ORIG   x3000
    ;PUTS-Output null termn string to console starting at address contained in R0
    ADD R0, R0, #0
    LEA R0, S1
    PUTS
    ;GETC-Read one input character from the keyboard and store it into R0
    ;   without echoing the character to the console.
    LEA R0, S2
    PUTS
    GETC
    OUT
    LEA R0, S3
    PUTS
    HALT
S1  .STRINGZ	"PUTS: Printing...\n"
S2  .STRINGZ    "GETC: Press a Key\n"
S3  .STRINGZ	"\nOUT: Printing was done by OUT\n"
    .END