use crate::error::Result;
use crate::store::Repo;

/// 建指针，不切换。
pub fn branch(repo: &Repo, name: &str) -> Result<()> {
    let _ = (repo, name);
    todo!()
}

/// 只删指针。
pub fn rm_branch(repo: &Repo, name: &str) -> Result<()> {
    let _ = (repo, name);
    todo!()
}
