# Operating System Projects

This repository contains a collection of projects focused on core operating system concepts, including shell implementation, system call handling, memory allocation, and file system interpretation. Each project is designed to provide hands-on experience with fundamental mechanisms of operating systems, contributing to a deeper understanding of how modern OSes operate under the hood.

## Projects Overview

### Mav Shell (`msh.c`)

**Objective**: Implement a custom shell program, Mav shell (`msh`), similar to existing shells such as Bourne Shell (`bash`), C Shell (`csh`), or Korn Shell (`ksh`). The shell will process commands, fork child processes to execute these commands, and run continuously until the user decides to exit.

**Key Concepts**: Process creation and management, command-line parsing, environment handling.

### System Call Extension (`systemcall`)

**Objective**: Gain practical experience with compiling and installing a new kernel, and extending the Linux operating system with new system calls. This project focuses on the intricacies of kernel development, including moving memory between user space and kernel space.

**Key Concepts**: Kernel compilation, system call implementation, memory management.

### Custom Memory Allocation (`malloc.c`)

**Objective**: Develop a custom implementation of the `malloc` and `free` functions, which are essential for dynamic memory management in user space applications. This project involves creating a library that manages heap memory, interfacing directly with the operating system.

**Key Concepts**: Memory allocation strategies, heap management, system interaction.

### FAT32 File System Interpreter (`FAT32`)

**Objective**: Explore the FAT32 file system architecture by developing a user space shell application capable of interpreting a FAT32 file system image. The project emphasizes the importance of understanding file allocation tables, endianess, and file access mechanisms, ensuring the utility maintains the integrity of the file system image.

**Key Concepts**: File system structure, file allocation table (FAT) manipulation, endianess, robust file handling.
