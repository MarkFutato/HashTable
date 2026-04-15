use crate::logger::{log_line, SharedLogger};
use std::sync::{Condvar, Mutex};

pub struct Scheduler {
    current_index: Mutex<usize>,
    condvar: Condvar,
}

impl Scheduler {
    pub fn new() -> Self {
        Self {
            current_index: Mutex::new(0),
            condvar: Condvar::new(),
        }
    }

    pub fn wait_turn(&self, order: usize, priority: i32, logger: &SharedLogger) {
        let mut current = self.current_index.lock().expect("Failed to lock scheduler");

        while order != *current {
            log_line(
                logger,
                &format!("THREAD {} WAITING FOR MY TURN", priority),
            );

            current = self
                .condvar
                .wait(current)
                .expect("Failed to wait on scheduler condition variable");
        }

        log_line(
            logger,
            &format!("THREAD {} AWAKENED FOR WORK", priority),
        );
    }

    pub fn finish_turn(&self) {
        let mut current = self.current_index.lock().expect("Failed to lock scheduler");
        *current += 1;
        self.condvar.notify_all();
    }
}