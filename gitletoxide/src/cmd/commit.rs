use crate::error::Result;
use crate::store::Repo;

/// 快照 = 父快照 + staged 增删；清 staging；HEAD 前移。只改 .gitlet/，不动工作区。
pub fn run(repo: &Repo, message: &str) -> Result<()> {
    let _ = (repo, message);
    todo!()
}
