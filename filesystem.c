#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#ifdef _WIN32
#include <windows.h>
#else
#endif

#define NUM_BLOCKS 512
#define BLOCK_SIZE 2048

// Data structures
typedef struct {
    int num_blocks;
    int block_size;
    int free_blocks;
    int *bitmap;
} VolumeControlBlock;

typedef struct {
    char filename[50];
    int start_block;
    int file_size;
} DirectoryEntry;

typedef struct {
    int file_size;
    int first_block;
} FileControlBlock;

// Global variables
VolumeControlBlock vcb;
DirectoryEntry directory[NUM_BLOCKS];
FileControlBlock system_open_table[NUM_BLOCKS];
FileControlBlock per_process_open_table[NUM_BLOCKS];

// Mutex locks for thread safety
pthread_mutex_t system_open_lock;
pthread_mutex_t process_open_lock;

// Function prototypes
void initialize_file_system();
int create(char *filename, int size);
int open_file(char *filename); 
void close_file(int fid);       
void read_file(int fid);        
void write_file(int fid, char *content, int size); 
void dir();
void delete_file(char *filename);

// Thread function prototypes
void *thread_p1(void *param);
void *thread_p2(void *param);
void *thread_p3(void *param);

int main() {
    initialize_file_system();

    printf("File system program started.\n");

    // Create pthreads
    pthread_t p1, p2, p3;

    // Start p1
    pthread_create(&p1, NULL, thread_p1, NULL);

    // Wait for p1 to finish before starting p2 and p3
    pthread_join(p1, NULL);

    // Start p2 and p3
    pthread_create(&p2, NULL, thread_p2, NULL);
    pthread_create(&p3, NULL, thread_p3, NULL);

    // Wait for p2 and p3 to finish
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);

    // Test dir() operation
    printf("\nTesting dir() operation:\n");
    dir();

    // Test delete_file() operation
    printf("\nTesting delete_file() operation:\n");
    delete_file("file1");
    delete_file("file2");

    // Display files after deletion
    printf("\nFiles after deletion:\n");
    dir();

#ifdef _WIN32
    Sleep(2000); // Delay for 2 seconds (Windows)
#endif

    printf("\nPress Enter to exit...");
    getchar(); // Wait for user to press Enter key

    return 0;
}

// Initialize the file system
void initialize_file_system() {
    // Initialize Volume Control Block
    vcb.num_blocks = NUM_BLOCKS;
    vcb.block_size = BLOCK_SIZE;
    vcb.free_blocks = NUM_BLOCKS - 1; // -1 for the Volume Control Block
    vcb.bitmap = (int *)malloc(NUM_BLOCKS * sizeof(int));
    memset(vcb.bitmap, 0, NUM_BLOCKS * sizeof(int));
    
    // Set the first block (VCB) as used
    vcb.bitmap[0] = 1;

    // Initialize mutex locks
    pthread_mutex_init(&system_open_lock, NULL);
    pthread_mutex_init(&process_open_lock, NULL);
}

// Create a file with specified size
int create(char *filename, int size) {
    // Check if the file already exists 
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (strcmp(directory[i].filename, filename) == 0) {
            printf("Error: File '%s' already exists.\n", filename);
            return -1;
        }
    }

    // Find an empty directory entry
    int empty_index = -5;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (strlen(directory[i].filename) == 0) {
            empty_index = i;
            break;
        }
    }

    if (empty_index == -1) {
        printf("Error: Directory is full.\n");
        return -1;
    }

    // Find free blocks for the file
    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (blocks_needed > vcb.free_blocks) {
        printf("Error: Not enough free blocks to create file.\n");
        return -1;
    }

    // Mark blocks as used in the bitmap
    int blocks_allocated = 0;
    for (int i = 1; i < NUM_BLOCKS; i++) {
        if (!vcb.bitmap[i]) {
            vcb.bitmap[i] = 1;
            blocks_allocated++;
            if (blocks_allocated == blocks_needed) break;
        }
    }

    // Update directory entry
    strcpy(directory[empty_index].filename, filename);
    directory[empty_index].start_block = empty_index + 1; // Start block index
    directory[empty_index].file_size = size;

    // Update free blocks count
    vcb.free_blocks -= blocks_allocated;

    printf("File '%s' (FID: %d, Start Block: %d) created successfully.\n", filename, empty_index, empty_index + 1);
    return empty_index; // Return file identifier
}

// Open a file and return file identifier
int open_file(char *filename) {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (strcmp(directory[i].filename, filename) == 0) {
            printf("File '%s' (FID: %d, Start Block: %d) opened successfully.\n", filename, i, directory[i].start_block);
            return i; // Return file identifier
        }
    }
    printf("Error: File '%s' not found.\n", filename);
    return -1;
}

// Close a file
void close_file(int fid) {
    printf("File (FID: %d, Start Block: %d) closed.\n", fid, directory[fid].start_block);
}

// Read from a file
void read_file(int fid) {
    printf("Reading from file with FID: %d\n", fid);
}

// Write to a file
void write_file(int fid, char *content, int size) {
    printf("Writing to file with FID: %d\n", fid);
}

// Display all files in the file system
void dir() {
    printf("Listing files...\n");
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (strlen(directory[i].filename) > 0) {
            printf("File: %s (FID: %d, Start Block: %d), Size: %d\n", directory[i].filename, i, directory[i].start_block, directory[i].file_size);
        }
    }
}

// Remove a specified file from the directory
void delete_file(char *filename) {
    for (int i = 0; i < NUM_BLOCKS; i++) {
        if (strcmp(directory[i].filename, filename) == 0) {
            printf("File '%s' (FID: %d) deleted.\n", filename, i);
            strcpy(directory[i].filename, "");
            // Mark blocks as free in the bitmap
            int start_block = directory[i].start_block;
            int file_size = directory[i].file_size;
            int blocks_to_free = (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
            for (int j = start_block; j < start_block + blocks_to_free; j++) {
                vcb.bitmap[j] = 0;
            }
            // Update free blocks count
            vcb.free_blocks += blocks_to_free;
            return;
        }
    }
    printf("Error: File '%s' not found.\n", filename);
}

// Thread p1: create file1, write file1, close file1, create file2, write file2, close file2
void *thread_p1(void *param) {
    int fid1 = create("file1", 17);
    write_file(fid1, "Hello from file1!", 17);
    close_file(fid1);
    int fid2 = create("file2", 17);
    write_file(fid2, "Hello from file2!", 17);
    close_file(fid2);
    pthread_exit(NULL);
}

// Thread p2: open file1, read file1, close file1
void *thread_p2(void *param) {
    int fid = open_file("file1");
    if (fid != -1) {
        read_file(fid);
        close_file(fid);
    }
    pthread_exit(NULL);
}

// Thread p3: open file2, read file2, close file2
void *thread_p3(void *param) {
    int fid = open_file("file2");
    if (fid != -1) {
        read_file(fid);
        close_file(fid);
    }
    pthread_exit(NULL);
}