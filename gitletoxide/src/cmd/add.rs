use crate::error::Result;
use crate::store::Repo;

/// 存 blob 进 staging.added；与 HEAD 版本同内容则取消 stage；并从 removed 划掉。
pub fn run(repo: &Repo, file: &str) -> Result<()> {
    let _ = (repo, file);
    todo!()
}
