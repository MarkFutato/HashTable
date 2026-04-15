use crate::hash::jenkins_hash;

#[derive(Debug, Clone)]
pub struct HashRecord {
    pub hash: u32,
    pub name: String,
    pub salary: u32,
}

#[derive(Debug, Default)]
pub struct Database {
    records: Vec<HashRecord>,
}

impl Database {
    pub fn new() -> Self {
        Self {
            records: Vec::new(),
        }
    }

    pub fn insert(&mut self, name: &str, salary: u32) -> Result<HashRecord, u32> {
        let hash = jenkins_hash(name);

        if self.records.iter().any(|record| record.hash == hash) {
            return Err(hash);
        }

        let record = HashRecord {
            hash,
            name: name.to_string(),
            salary,
        };

        self.records.push(record.clone());
        Ok(record)
    }

    pub fn search(&self, name: &str) -> Option<HashRecord> {
        let hash = jenkins_hash(name);

        self.records
            .iter()
            .find(|record| record.hash == hash)
            .cloned()
    }

    pub fn update(&mut self, name: &str, new_salary: u32) -> Result<(HashRecord, HashRecord), u32> {
    let hash = jenkins_hash(name);

    if let Some(record) = self.records.iter_mut().find(|record| record.hash == hash) {
        let old_record = record.clone();
        record.salary = new_salary;
        let new_record = record.clone();
        Ok((old_record, new_record))
    } else {
        Err(hash)
    }
    }

    pub fn delete(&mut self, name: &str) -> Result<HashRecord, u32> {
        let hash = jenkins_hash(name);

        if let Some(index) = self.records.iter().position(|record| record.hash == hash) {
            Ok(self.records.remove(index))
        } else {
            Err(hash)
        }
    }

    pub fn sorted_records(&self) -> Vec<HashRecord> {
        let mut records = self.records.clone();
        records.sort_by_key(|record| record.hash);
        records
    }

    pub fn print_records(&self) {
        println!("Current Database:");

        for record in self.sorted_records() {
            println!("{},{},{}", record.hash, record.name, record.salary);
        }
    }
}