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
├── RUST_NOTES.md
└── src
    ├── main.rs
    ├── command.rs
    ├── db.rs
    ├── hash.rs
    ├── logger.rs
    └── scheduler.rs
```

# Assignment Instructions

A concurrent hash table is a data structure that allows multiple threads to
perform operations on a shared collection of key-data pairs, without causing
data corruption or inconsistency. The concurrent hash table consists of a linked
list of nodes, each of which stores some data associated with the key. The hash
value is computed by applying a hash function to the key.

Note that for this assignment, we will not be taking into consideration hash
collisions. We will use Jenkins's one_at_a_time hash functionLinks to an
external site.

Links to an external site.Links to an external site. which has a very low
collision rate for the number of hashes we will be working with. Regardless,
your test data and the final data set you will be graded against will be
guaranteed collision-free.

## The concurrent hash table supports the following functions:

`insert(key, values)`: This function inserts a new key-data pair into the hash
table. If the entry already exists, report an error. To insert a key-data pair,
the function first computes the hash value of the given name (the key). Then, it
acquires the write lock that protects the list and searches the linked list for
the hash. It then creates a new node and inserts it in the list. Finally, it
releases the write lock and returns.

`delete(key)`: This function deletes a key-data pair from the hash table, if it
exists. To delete a key-data pair, the function first computes the hash value of
the key and obtains a writer lock. Then it searches the linked list for the key.
If the key is found, it removes the node from the list and frees the memory.
Otherwise, it does nothing. Finally, it releases the write lock and returns.

`updateSalary(key, value)`: This function takes a key and an integer value. If
it finds a node with a matching key, it updates the salary field with the given
value.

`search(key)`: This function searches for a key-data pair in the hash table and
returns the value, if it exists. To search for a key-data pair, the function
first computes the hash value of the key acquires a reader lock. Then, it
searches the linked list for the key. If the key is found, it returns the value.
Otherwise, it returns NULL. Finally, it releases the read lock and returns. The
caller should then print the record or "No Record Found" if the return is NULL.

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

## Reader-Writer Lock Reference

The OSTEP authors have provided sample implementations that you should
reference:

[https://github.com/remzi-arpacidusseau/ostep-code/tree/master/threads-sema](https://github.com/remzi-arpacidusseau/ostep-code/tree/master/threads-sema)

## The Command File

Your program must read a text file called `commands.txt` containing some
configuration information, commands, and data values. Do not read it from the
console. You should hard code "commands.txt" into your program.

### Commands and Their Parameters

The program reads commands from `commands.txt`. Each line contains one command
and its associated parameters.

| Command  | Parameters                   | Description                                                         |
| -------- | ---------------------------- | ------------------------------------------------------------------- |
| `insert` | `<name>,<salary>,<priority>` | Inserts a new record with the given name and salary.                |
| `delete` | `<name>,<priority>`          | Deletes the record for the given name, if it exists.                |
| `update` | `<name>,<new salary value>`  | Updates the salary for the given name.                              |
| `search` | `<name>,<priority>`          | Searches for the record with the given name and prints it if found. |
| `print`  | `<priority>`                 | Prints the entire contents of the database.                         |

### Sample `commands.txt`

```text
insert,Richard Garriot,40000,1
insert,Sid Meier,50000,2
insert,Shigeru Miyamoto,51000,3
delete,Sid Meier,4
insert,Hideo Kojima,45000,5
insert,Gabe Newell,49000,6
insert,Roberta Williams,45900,7
delete,Richard Garriot,8
insert,Carol Shaw,41000,9
search,Sid Meier,10
```

## Expected output

### Command Output

| Command  | Output                                                                                                                  |
| -------- | ----------------------------------------------------------------------------------------------------------------------- |
| `INSERT` | `Inserted <values>`<br>On duplicate entry: `Insert failed. Entry <hash> is a duplicate.`                                |
| `UPDATE` | `Updated record <hash> from <old values> to <new values>`<br>On missing entry: `Update failed. Entry <hash> not found.` |
| `DELETE` | `Deleted record for <values>`<br>On missing entry: `Entry <hash> not deleted. Not in database.`                         |
| `SEARCH` | Found: `<values>`<br>Not found: `<search string> not found.`                                                            |
| `PRINT`  | Current Database: `<all records in the database, sorted by hash>`                                                       |

At the end of the run, you will need to run a final PRINT (to the
console/stdout), even if the last command of the `commands.txt` file is PRINT.

**NOTE:**

- Only the final list pring is evaluated for grading purposes. The exact
  ordering of thread execution will be random, as we've discussed in class.
- The delete function may incur one or two sets of lock operations, depending on
  your implementation:
    - If you use the search function you already wrote to find the record to
      delete, you will have two sets of lock operations -- the outer lock for
      the delete and the inner one for the search.
    - Otherwise, you'll just have one set of operations.

## Why Priority and Locking?

### Rationale for Priority Ordering AND Mutual Exclusion

Log File `hash.log`

We also need diagnostic output to make sure that our licks and CVs are occurring
as expected.

Write out each command as they're executed, along with their parameters in this
format:

```text
<timestamp>: THREAD <priority>,<command and parameters>
```

You will also write out when locks are acquired and realeased:

```text
<timestamp>: THREAD <priority>WAITING FOR MY TURN
<timestamp>: THREAD <priority>AWAKENED FOR WORK
<timestamp>: THREAD <priority>READ LOCK ACQUIRED
<timestamp>: THREAD <priority>READ LOCK RELEASED
<timestamp>: THREAD <priority>WRITE LOCK ACQUIRED
<timestamp>: THREAD <priority>WRITE LOCK RELEASED
```

Use the following function to obtain accurate timing values:

```c
#include "`sys/time.h"

long long current_timestanp() {
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    long long microseconds = (te.tv_sec * 10000000) + te.tv_usec: // calculate milliseconds
    return microseconds;
}
```

Sample log output looks something like this:

```text
1721428978841092: THREAD 0 INSERT,2569965317,Hideo Kojima,45000
1721428978841096: THREAD 0 WRITE LOCK ACQUIRED
1721428978841098: THREAD 0 WRITE LOCK RELEASED
```

## Final Deliverable:

### Submission Files

| Filename             | Purpose                                                                                                                     |
| -------------------- | --------------------------------------------------------------------------------------------------------------------------- |
| `chash.c`            | Main program that reads `commands.txt` and produces output to the console.                                                  |
| `Makefile`           | Builds the project into the final executable. The code should be modular and split across multiple source files.            |
| `Other Source Files` | Any additional source files used in the project. The implementation should not be written as one monolithic `chash.c` file. |
| `README.txt`         | Contains anything the instructor or graders need to know, including the required AI-use citation.                           |

All of these should be uploaded as a single zip file contatining all of the
files.

We should be able to:

- Unzip your file.
- Run `make` to compile it into the main executable `chash`
- Your program will read `commands.txt` and then produce `hash.log` and console
  output for the product of each command.

### Sample Input and Output Files

- See Docs folder.

## AI Policy

When using AI assistants, please adhere to the following policies and
suggestions:

### Put Effort into Crafting High-Quality Prompts

Tools like ChatGPT and Copilot, while useful, have serious limitations and are
often incorrect. If you provide minimum-effort prompts, you will get low-quality
results. You will need to refine your prompts in order to get good outcomes.
This takes deliberate effort.

### Be Aware of AI Limitations

Even if you have crafted well-constructed prompts, do not blindly trust anything
an AI assistant says or outputs. If it gives a snippet of code, number, or fact,
assume that it is wrong unless you either already know it to be correct or
verify it with another source. You are responsible for any errors or omissions
provided by the tool, and these tools tend to work best for topics you already
understand well.

### Give the AI Tool Proper Attribution

AI is a tool, and one that you must acknowledge using. Thus, you must provide
proper attribution if you use it for assignments. You are required to include a
paragraph or two, either in a comment or in the README file, explaining what you
used the AI for and what prompts you used to get the results. Failure to do this
will result in a violation of UCF Academic Integrity policies.

### Know When to Use and Not Use AI Tools

Be thoughtful about when AI tools are useful and when they are not. Do not use
the tool if it is not appropriate, or if you do not have a full grasp of the
concept from class.

_Credit to Dr. Kevin Moran for this succinct summary of AI use policy._

## Extra Credit

For extra credit, you may choose to implement this assignment in Rust with one
major new requirement:

You must document your project as if the reader were a complete beginner to
Rust.

In other words, use this project as a teaching example. Specific things the
instructor is looking for include:

- the thread-safe features of Rust
- the memory-safe features of Rust
- how those features were used in your project
- places where Rust differs significantly from C

### Documentation Requirements

- The documentation should be in a separate file, preferably **not** the README.
- The README is reserved for build and run instructions and your AI citation(s).
- Ideally, the documentation should be written in **Markdown**.
- A Word file or PDF will still be accepted, but Markdown is preferred.

### Total Points Available

`10`
