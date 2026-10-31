use std::collections::BTreeMap;
use std::path::{Path, PathBuf};

use crate::error::Result;
use crate::model::{Commit, Sha, Staging};

/// 一个仓库句柄。
///
/// 关键设计：root 显式持有，**任何地方都不读 cwd**。
/// cwd 只在 main.rs 出现一次，这样 cargo test 可以多线程并行跑而互不干扰。
pub struct Repo {
    pub root: PathBuf,
}

/// SHA-1 十六进制。内容寻址的唯一入口，测试里可以直接调用它。
pub fn hash(bytes: &[u8]) -> Sha {
    use sha1::{Digest, Sha1};
    let mut h = Sha1::new();
    h.update(bytes);
    h.finalize().iter().map(|b| format!("{:02x}", b)).collect()
}

impl Repo {
    /// 建 .gitlet、初始 commit（message = "initial commit"，timestamp = 0，快照空）、master、HEAD。
    pub fn init(root: &Path) -> Result<()> {
        let _ = root;
        todo!()
    }

    /// 打不开就返回 "Not in an initialized Gitlet directory."。
    pub fn open(root: &Path) -> Result<Self> {
        let _ = root;
        todo!()
    }

    pub fn head_branch(&self) -> Result<String> {
        todo!()
    }

    pub fn set_head_branch(&self, name: &str) -> Result<()> {
        let _ = name;
        todo!()
    }

    pub fn head(&self) -> Result<Sha> {
        todo!()
    }

    pub fn branches(&self) -> Result<BTreeMap<String, Sha>> {
        todo!()
    }

    pub fn set_branch(&self, name: &str, sha: &Sha) -> Result<()> {
        let _ = (name, sha);
        todo!()
    }

    pub fn delete_branch(&self, name: &str) -> Result<()> {
        let _ = name;
        todo!()
    }

    pub fn commit(&self, sha: &Sha) -> Result<Commit> {
        let _ = sha;
        todo!()
    }

    /// 写对象并返回它的 SHA。同一个 Commit 两次写入必须得到同一个 SHA。
    pub fn put_commit(&self, c: &Commit) -> Result<Sha> {
        let _ = c;
        todo!()
    }

    pub fn blob(&self, sha: &Sha) -> Result<Vec<u8>> {
        let _ = sha;
        todo!()
    }

    /// 内容寻址：同样字节只存一份。
    pub fn put_blob(&self, data: &[u8]) -> Result<Sha> {
        let _ = data;
        todo!()
    }

    pub fn staging(&self) -> Result<Staging> {
        todo!()
    }

    pub fn set_staging(&self, s: &Staging) -> Result<()> {
        let _ = s;
        todo!()
    }

    /// 唯一前缀解析；0 个或多个匹配都算 "No commit with that id exists."。
    pub fn resolve_prefix(&self, prefix: &str) -> Result<Sha> {
        let _ = prefix;
        todo!()
    }

    /// 工作区普通文件名，已排序；忽略子目录。
    pub fn work_files(&self) -> Result<Vec<String>> {
        todo!()
    }

    /// 读工作区文件内容；不存在返回 None（不要报错，调用方自己决定语义）。
    pub fn work_bytes(&self, name: &str) -> Result<Option<Vec<u8>>> {
        let _ = name;
        todo!()
    }

    pub fn write_work_file(&self, name: &str, data: &[u8]) -> Result<()> {
        let _ = (name, data);
        todo!()
    }

    pub fn delete_work_file(&self, name: &str) -> Result<()> {
        let _ = name;
        todo!()
    }
}

/// 快捷构造：测试里用 tempfile::TempDir 的路径。
#[allow(dead_code)]
pub fn open_at(path: impl AsRef<Path>) -> Result<Repo> {
    Repo::open(path.as_ref())
}
