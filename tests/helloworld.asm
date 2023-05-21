    ;Simple Hello World Program
    .ORIG   X3000
    LEA R0, HW
    PUTS 
    HALT 
HW  .STRINGZ "Hello World!"
    .END
