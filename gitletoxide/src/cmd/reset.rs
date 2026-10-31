use crate::error::Result;
use crate::store::Repo;

/// = 对任意 commit 执行 checkout③ + 挪当前分支指针 + 清 staging。
pub fn run(repo: &Repo, id: &str) -> Result<()> {
    let _ = (repo, id);
    todo!()
}
