use crate::error::Result;
use crate::store::Repo;

/// 已 staged 增加则取消；HEAD 跟踪则标 removed 并从工作区删除。
pub fn run(repo: &Repo, file: &str) -> Result<()> {
    let _ = (repo, file);
    todo!()
}
