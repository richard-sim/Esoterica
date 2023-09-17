# Bugs

## ResourceServer

**[10/09/2023] #1 Slient Failure**  
Failure of shader compilation is slient, no error messages produces.
ResourceSystem will keep wait raw resources to be produced and the entire engine will not shutdown correctly.