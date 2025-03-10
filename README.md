# Simple File System

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.cppreference.com/w/c)
[![Threading](https://img.shields.io/badge/Threading-pthread-green.svg)](https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread.h.html)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A lightweight, thread-safe file system implementation in C that simulates core file operations with concurrent access management.

## Overview

This Simple File System (SFS) project demonstrates fundamental file system concepts through a practical implementation. It manages file creation, access, and deletion with memory allocation for file blocks, and handles concurrent file operations using mutex locks for thread safety.

## Features

- **Basic File Operations**
  - Create, open, read, write, and delete files
  - Directory listing capability
  - File system initialization

- **Resource Management**
  - Bitmap-based block allocation tracking
  - Efficient memory utilization
  - Free block accounting

- **Concurrency Support**
  - Thread-safe operations using mutex locks
  - Proper synchronization for file access
  - Support for concurrent processes accessing the file system

- **Data Structures**
  - Volume Control Block (VCB) for file system metadata
  - Directory entries for file organization
  - System-wide and per-process file control blocks

## System Architecture

### Core Components

1. **Volume Control Block (VCB)**
   - Tracks total and free blocks available
   - Maintains a bitmap of allocated blocks
   - Stores block size configuration

2. **Directory Structure**
   - Maps filenames to block locations
   - Maintains file size information
   - Supports lookup operations

3. **File Control Blocks**
   - System-wide open file table
   - Per-process open file table
   - Tracks file state and access information

4. **Thread Synchronization**
   - Mutex protection for shared resources
   - Prevents race conditions during concurrent access

## Usage

### Building the Project

Compile the project using a C compiler with pthread support:

```bash
gcc -o filesystem filesystem.c -lpthread
```

### Running the Program

Execute the compiled binary:

```bash
./filesystem
```

The program will:
1. Initialize the file system
2. Create and operate on files through simulated processes
3. Demonstrate thread-safe file operations
4. Display directory contents
5. Test file deletion

### Example Operations

The system demonstrates several file operations:

```c
// Create a file
int fid = create("example.txt", 1024);

// Open a file
int fid = open_file("example.txt");

// Write to a file
write_file(fid, "Hello, World!", 13);

// Read from a file
read_file(fid);

// Close a file
close_file(fid);

// List files in the directory
dir();

// Delete a file
delete_file("example.txt");
```

## Implementation Details

### File Creation and Allocation

The file creation process:
1. Checks if the file already exists
2. Finds an empty directory entry
3. Calculates blocks needed based on file size
4. Allocates blocks by setting bits in the bitmap
5. Updates the directory entry and free block count

```c
int create(char *filename, int size) {
    // Implementation details in code...
    // Returns file identifier or -1 on error
}
```

### Concurrency Management

Thread safety is achieved through:
- Mutex locks for critical sections
- Per-process open file tables
- System-wide resource tracking

```c
pthread_mutex_t system_open_lock;
pthread_mutex_t process_open_lock;
```

### Memory Management

The file system manages:
- 512 total blocks
- 2048 bytes per block
- Bitmap tracking of allocated blocks

## Testing

The system is tested through three simulated processes:
- **Process 1**: Creates and writes to two files
- **Process 2**: Opens, reads, and closes the first file
- **Process 3**: Opens, reads, and closes the second file

The test verifies:
- Proper file creation and storage
- Successful file access 
- Correct concurrent operation
- Proper file deletion and resource reclamation

## Limitations

- Fixed directory size (limited to NUM_BLOCKS entries)
- No hierarchical directory structure
- No file permissions or access control
- Simplified read/write operations (simulated)
- In-memory only (no persistence)

## Future Enhancements

- Implement persistent storage capabilities
- Add hierarchical directory structure
- Support for file permissions and access control
- Fragmentation management
- Performance optimizations
- More robust error handling

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by UNIX file system design principles
- Developed as an educational demonstration of file system concepts
- Based on concurrent programming concepts using POSIX threads
