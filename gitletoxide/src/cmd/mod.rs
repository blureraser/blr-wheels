use std::path::Path;

use crate::error::Result;
use crate::store::Repo;

pub mod add;
pub mod branch;
pub mod checkout;
pub mod commit;
pub mod init;
pub mod log;
pub mod merge;
pub mod reset;
pub mod rm;
pub mod status;

pub const COMMANDS: &[&str] = &[
    "init",
    "add",
    "commit",
    "rm",
    "log",
    "global-log",
    "find",
    "status",
    "checkout",
    "branch",
    "rm-branch",
    "reset",
    "merge",
];

/// 四关顺序不能换：命令名 → 操作数 → 是否已初始化。
/// 先查目录再查操作数，会把 "Incorrect operands." 错报成
/// "Not in an initialized Gitlet directory."。
pub fn dispatch(root: &Path, args: &[String]) -> Result<()> {
    if args.is_empty() {
        return Err("Please enter a command.".to_string());
    }
    let name = args[0].as_str();
    if !COMMANDS.contains(&name) {
        return Err("No command with that name exists.".to_string());
    }
    let rest = &args[1..];
    check_operands(name, rest)?;
    if name == "init" {
        return init::run(root);
    }
    let repo = Repo::open(root)?;
    match name {
        "add" => add::run(&repo, &rest[0]),
        "commit" => commit::run(&repo, &rest[0]),
        "rm" => rm::run(&repo, &rest[0]),
        "log" => log::log(&repo),
        "global-log" => log::global_log(&repo),
        "find" => log::find(&repo, &rest[0]),
        "status" => status::run(&repo),
        "checkout" => checkout::run(&repo, rest),
        "branch" => branch::branch(&repo, &rest[0]),
        "rm-branch" => branch::rm_branch(&repo, &rest[0]),
        "reset" => reset::run(&repo, &rest[0]),
        "merge" => merge::run(&repo, &rest[0]),
        _ => unreachable!(),
    }
}

fn check_operands(name: &str, rest: &[String]) -> Result<()> {
    let ok = match name {
        "init" | "log" | "global-log" | "status" => rest.is_empty(),
        "add" | "commit" | "rm" | "find" | "branch" | "rm-branch" | "reset" | "merge" => {
            rest.len() == 1
        }
        "checkout" => (1..=3).contains(&rest.len()),
        _ => false,
    };
    if ok {
        Ok(())
    } else {
        Err("Incorrect operands.".to_string())
    }
}
