# Bugs

## ResourceServer

**[10/09/2023] #1 Slient Failure**  
Failure of shader compilation is slient, no error messages produces.
ResourceSystem will keep wait raw resources to be produced and the entire engine will not shutdown correctly.

**[24/09/2023] #2 Log Stack Corrupted**  
In Base/Log/Log_Win32.cpp, TraceMessage.  
When user feed in a very long log message, stack of messageBuffer will overflow and cause corruption.  
**Temporary fixed**: increase the size of messageBuffer to 1024.