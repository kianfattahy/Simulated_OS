# Memory Management in Simulated OS 
## COMP 310 – ECSE 427, McGill University

### Description:
This project involved the development of a memory manager, extending the capabilities of a previously simulated operating system. At the conclusion of this implementation, the simulated OS is able to:
- Execute simple shell commands.
- Manage processes using various scheduling techniques.
- Employ a mechanism of demand paging, allowing for the execution of programs that are larger than the shell memory size.

### Key Features:
- **Paging Infrastructure:** The memory manager supports paging by segmenting the shell memory into pages. As programs are executed, only necessary pages are loaded into memory. When memory is exhausted, older pages are swapped out to make space.
- **Demand Paging:** Allows programs larger than the shell memory size to be executed. The program is divided into pages, with only the essential pages loaded into memory. The old pages are replaced as the shell memory fills up.
- **LRU Replacement Policy:** The Least Recently Used (LRU) policy has been implemented for demand paging, ensuring efficient page swapping.
- **Backed by Backing Store:** A backing store, simulated as a directory, supports the paging system. It holds information that isn't currently in the main memory.
- **Dynamic Memory Setting at Compile Time:** The sizes of the frame store and variable store can be dynamically set at compile time using compilation flags.
- **Handling Page Faults:** The OS handles page faults efficiently. If a program tries to execute a line of code from a page not currently in memory, a page fault is triggered, effectively managing the execution of the interrupted process and updating the memory as needed.

### Testing:
The project has been rigorously tested with 10 test cases provided in the starter code repository. These tests ensure that the memory manager behaves as expected and correctly implements the desired features.

