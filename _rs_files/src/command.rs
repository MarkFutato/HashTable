#[derive(Debug, Clone)]
pub enum CommandType {
    Insert,
    Delete,
    Update,
    Search,
    Print,
}

#[derive(Debug, Clone)]
pub struct Command {
    pub cmd_type: CommandType,
    pub name: String,
    pub salary: u32,
    pub priority: i32,
    pub order: usize,
    pub original_line: String,
}

pub fn parse_command_line(line: &str, order: usize) -> Option<Command> {
    let parts: Vec<&str> = line.trim().split(',').collect();

    if parts.is_empty() {
        return None;
    }

    match parts[0] {
        "insert" => {
            if parts.len() != 4 {
                return None;
            }

            let name = parts[1].to_string();
            let salary = parts[2].parse::<u32>().ok()?;
            let priority = parts[3].parse::<i32>().ok()?;

            Some(Command {
                cmd_type: CommandType::Insert,
                name,
                salary,
                priority,
                order,
                original_line: line.trim().to_string(),
            })
        }

        "delete" => {
            if parts.len() != 3 {
                return None;
            }

            let name = parts[1].to_string();
            let priority = parts[2].parse::<i32>().ok()?;

            Some(Command {
                cmd_type: CommandType::Delete,
                name,
                salary: 0,
                priority,
                order,
                original_line: line.trim().to_string(),
            })
        }

        "update" => {
            if parts.len() != 3 {
                return None;
            }

            let name = parts[1].to_string();
            let salary = parts[2].parse::<u32>().ok()?;

            Some(Command {
                cmd_type: CommandType::Update,
                name,
                salary,
                priority: 0,
                order,
                original_line: line.trim().to_string(),
            })
        }

        "search" => {
            if parts.len() != 3 {
                return None;
            }

            let name = parts[1].to_string();
            let priority = parts[2].parse::<i32>().ok()?;

            Some(Command {
                cmd_type: CommandType::Search,
                name,
                salary: 0,
                priority,
                order,
                original_line: line.trim().to_string(),
            })
        }

        "print" => {
            if parts.len() != 2 {
                return None;
            }

            let priority = parts[1].parse::<i32>().ok()?;

            Some(Command {
                cmd_type: CommandType::Print,
                name: String::new(),
                salary: 0,
                priority,
                order,
                original_line: line.trim().to_string(),
            })
        }

        _ => None,
    }
}