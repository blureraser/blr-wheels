use crate::error::Result;
use crate::store::Repo;

/// 三种形态：
///   checkout -- <file>          -> ["--", file]
///   checkout <id> -- <file>     -> [id, "--", file]
///   checkout <branch>           -> [branch]
/// untracked-in-the-way 检查必须在任何动作之前。
pub fn run(repo: &Repo, args: &[String]) -> Result<()> {
    let _ = (repo, args);
    todo!()
}
