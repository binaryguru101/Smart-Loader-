# SimpleSmartLoader Project

This project builds upon the **SimpleLoader** implementation from Assignment-1 by introducing a smarter version called **SimpleSmartLoader**. Unlike its predecessor, the SimpleSmartLoader uses lazy loading to load program segments. This means that segments are loaded only when needed during program execution, mimicking the behavior of modern operating systems like Linux.

## Key Features

1. **Lazy Loading**: Program segments are not loaded upfront but are dynamically loaded during execution.
2. **ELF 32-Bit Executables**: Supports only ELF 32-bit executables, without using glibc APIs.
3. **Enhanced Efficiency**: Reduces resource usage by loading segments on demand.
4. **Foundation on SimpleLoader**: Builds upon the core concepts of the SimpleLoader implementation while overcoming its limitations.

## Learning Objectives

- Understand the concept of lazy loading and its advantages in operating systems.
- Work with ELF executables and explore their structure.
- Implement efficient memory and process management techniques in C.
