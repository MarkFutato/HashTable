# Concurrent Hash Table in Rust

This project implements a concurrent hash table for processing employee salary
records from a `commands.txt` input file. The program supports concurrent
command execution using threads, coordinates execution order using command
priority, and protects shared data with synchronization primitives so that reads
and writes occur safely.

Although the original assignment describes the data structure in C and
references a linked-list-based implementation, this submission is written in
**Rust**. The Rust version preserves the required behavior of the assignment
while using Rust’s thread-safe and memory-safe abstractions to manage shared
state and synchronization. :contentReference[oaicite:0]{index=0}

## Features

- Reads commands from a hardcoded `commands.txt` file
- Supports the following commands:
    - `insert`
    - `delete`
    - `update`
    - `search`
    - `print`
- Computes keys using the **Jenkins one-at-a-time hash**
- Uses multiple threads to process commands
- Enforces command execution in **priority order**
- Uses synchronization to protect shared data from race conditions
- Writes command results to **stdout**
- Writes diagnostic thread/lock activity to `hash.log`
- Performs a **final print** of the database at the end of execution, even if
  the last command is already `print` :contentReference[oaicite:1]{index=1}

## Project Structure

Example structure:

```text
.
├── Cargo.toml
├── commands.txt
├── README.md
└── src
    ├── main.rs
    ├── command.rs
    ├── db.rs
    ├── hash.rs
    ├── logger.rs
    └── scheduler.rs
```

## The Hash Table Structure

```c
typdef struct hash_struct {
    uint32_t hash;
    char name[50];
    uint32_t salary;
    struct hash_struct *next;
} hashRecord;
```

- Field Description
- Hash 32-bit unsigned integer for the hash value produced by running the name
  test through the Jenkins one-at-a-time function--links to an external site.
- Name Arbitrary string up to 50 characters long
- Salary 32-bit unsignd integer (who wants a negative salaray, eh?) to represent
  and annual salary.
- Next pointer to the next record

### AI Tool Citations

Originally the assignment was written in C to have a better understanding of the
functionally and be more familiar with it, making writng it in Rust less hectic.
The assignment was rewritten in Rust to complete that task some AI help was
used. ChatGPT helped to check the rust version was correct or if it needed some
changes to be made. To get results from AI I copied my code version in C and in
RUST, and asked to compare both codes and that the functionality was correct.
