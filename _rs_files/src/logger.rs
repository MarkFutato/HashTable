use std::fs::File;
use std::io::{Result, Write};
use std::sync::{Arc, Mutex};
use std::time::{SystemTime, UNIX_EPOCH};

pub type SharedLogger = Arc<Mutex<File>>;

pub fn create_logger(path: &str) -> Result<SharedLogger> {
    let file = File::create(path)?;
    Ok(Arc::new(Mutex::new(file)))
}

pub fn current_timestamp() -> u128 {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .expect("System time error")
        .as_micros()
}

pub fn log_line(logger: &SharedLogger, message: &str) {
    let mut file = logger.lock().expect("Failed to lock log file");
    writeln!(file, "{}: {}", current_timestamp(), message)
        .expect("Failed to write to hash.log");
}

pub fn log_command(logger: &SharedLogger, priority: i32, command_text: &str) {
    log_line(logger, &format!("THREAD {},{}", priority, command_text));
}

pub fn log_read_lock_acquired(logger: &SharedLogger, priority: i32) {
    log_line(logger, &format!("THREAD {} READ LOCK ACQUIRED", priority));
}

pub fn log_read_lock_released(logger: &SharedLogger, priority: i32) {
    log_line(logger, &format!("THREAD {} READ LOCK RELEASED", priority));
}

pub fn log_write_lock_acquired(logger: &SharedLogger, priority: i32) {
    log_line(logger, &format!("THREAD {} WRITE LOCK ACQUIRED", priority));
}

pub fn log_write_lock_released(logger: &SharedLogger, priority: i32) {
    log_line(logger, &format!("THREAD {} WRITE LOCK RELEASED", priority));
}