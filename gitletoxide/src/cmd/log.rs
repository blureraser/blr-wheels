use crate::error::Result;
use crate::store::Repo;

/// 从 HEAD 沿第一父回溯到初始提交，新的在前。
pub fn log(repo: &Repo) -> Result<()> {
    let _ = repo;
    todo!()
}

/// 所有 commit，顺序任意。
pub fn global_log(repo: &Repo) -> Result<()> {
    let _ = repo;
    todo!()
}

/// message 精确匹配的 commit id，每行一个。
pub fn find(repo: &Repo, message: &str) -> Result<()> {
    let _ = (repo, message);
    todo!()
}
