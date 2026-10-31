use crate::error::Result;
use crate::store::Repo;

/// 求 split point -> 两个短路 -> 逐文件三路合并 -> 自动提交（两个父）。
pub fn run(repo: &Repo, branch: &str) -> Result<()> {
    let _ = (repo, branch);
    todo!()
}
