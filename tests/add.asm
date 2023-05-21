        ;Testing Opcode ADD
        .ORIG   x3000           ; start of program
        ; display input prompt string to user
        AND R0, R0, #0          ;CLEAR R0
        IN                      ; read first input value into R0
        ADD R2, R0, #0
        ; display second input prompt string to user
        IN                      ; read second input value into R0
        ADD R3, R0, #0
        ; display output string to user
        ADD     R0, R2, R3      ; add the values in R2 and R3, and store the result in R0
        OUT                     ; output the value in R0
        HALT                    ; end of program
        .END
