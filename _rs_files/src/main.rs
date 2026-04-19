mod command;
mod db;
mod hash;
mod logger;
mod scheduler;

use command::{parse_command_line, CommandType};
use db::Database;
use logger::{
    create_logger, log_command, log_read_lock_acquired, log_read_lock_released,
    log_write_lock_acquired, log_write_lock_released,
};
use scheduler::Scheduler;
use std::fs;
use std::sync::{Arc, RwLock};

fn main() {
    let contents = fs::read_to_string("commands.txt").expect("Unable to read commands.txt");

    let logger = create_logger("_rs_files/hash.log").expect("Unable to create hash.log");

    let scheduler = Arc::new(Scheduler::new());

    let mut commands = Vec::new();

    for (order, line) in contents.lines().enumerate() {
        if let Some(command) = parse_command_line(line, order) {
            commands.push(command);
        }
    }

    commands.sort_by_key(|command| command.priority);

    for (order, command) in commands.iter_mut().enumerate() {
        command.order = order;
    }

    let db = Arc::new(RwLock::new(Database::new()));

    let mut handles = Vec::new();

    for command in commands {
        let db = Arc::clone(&db);
        let logger = Arc::clone(&logger);
        let scheduler = Arc::clone(&scheduler);

        let handle = std::thread::spawn(move || {
            scheduler.wait_turn(command.order, command.priority, &logger);

            match command.cmd_type {
                CommandType::Insert => {
                    log_command(&logger, command.priority, &command.original_line);

                    let mut db = db.write().expect("Failed to acquire write lock");
                    log_write_lock_acquired(&logger, command.priority);

                    match db.insert(&command.name, command.salary) {
                        Ok(record) => {
                            println!("Inserted {},{},{}", record.hash, record.name, record.salary);
                        }
                        Err(hash) => {
                            println!("Insert failed. Entry {} is a duplicate.", hash);
                        }
                    }

                    log_write_lock_released(&logger, command.priority);
                }

                CommandType::Delete => {
                    log_command(&logger, command.priority, &command.original_line);

                    let mut db = db.write().expect("Failed to acquire write lock");
                    log_write_lock_acquired(&logger, command.priority);

                    match db.delete(&command.name) {
                        Ok(_) => {
                            println!("Deleted record for {}", command.name);
                        }
                        Err(hash) => {
                            println!("Delete failed. Entry {} not found.", hash);
                        }
                    }

                    log_write_lock_released(&logger, command.priority);
                }

                CommandType::Update => {
                    log_command(&logger, command.priority, &command.original_line);

                    let mut db = db.write().expect("Failed to acquire write lock");
                    log_write_lock_acquired(&logger, command.priority);

                    match db.update(&command.name, command.salary) {
                        Ok((old_record, new_record)) => {
                            println!(
                                "Updated record {} from {},{} to {},{}",
                                new_record.hash,
                                old_record.name,
                                old_record.salary,
                                new_record.name,
                                new_record.salary
                            );
                        }
                        Err(hash) => {
                            println!("Update failed. Entry {} not found.", hash);
                        }
                    }

                    log_write_lock_released(&logger, command.priority);
                }

                CommandType::Search => {
                    log_command(&logger, command.priority, &command.original_line);

                    let db = db.read().expect("Failed to acquire read lock");
                    log_read_lock_acquired(&logger, command.priority);

                    match db.search(&command.name) {
                        Some(record) => {
                            println!("Found: {},{},{}", record.hash, record.name, record.salary);
                        }
                        None => {
                            println!("Not Found: {} not found.", command.name);
                        }
                    }

                    log_read_lock_released(&logger, command.priority);
                }

                CommandType::Print => {
                    log_command(&logger, command.priority, &command.original_line);

                    let db = db.read().expect("Failed to acquire read lock");
                    log_read_lock_acquired(&logger, command.priority);

                    db.print_records();

                    log_read_lock_released(&logger, command.priority);
                }
            }

            scheduler.finish_turn();
        });

        handles.push(handle);
    }

    for handle in handles {
        handle.join().expect("Thread failed");
    }

    println!("\nFinal Database:");
    let db = db.read().expect("Failed to acquire read lock");
    db.print_records();
}
