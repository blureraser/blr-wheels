use std::collections::BTreeMap;

use serde::{Deserialize, Serialize};

/// 40 位小写十六进制 SHA-1。
pub type Sha = String;

/// 一次提交。创建后永不修改（提交图不可变）。
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Commit {
    pub message: String,
    /// 毫秒时间戳；初始提交固定为 0（epoch）。
    pub timestamp: i64,
    /// 第一父；初始提交为 None。
    pub parent: Option<Sha>,
    /// 第二父；只有 merge commit 有。
    pub parent2: Option<Sha>,
    /// 文件名 -> blob 的 SHA。必须 BTreeMap：哈希输入要确定。
    pub files: BTreeMap<String, Sha>,
}

/// 暂存区。落盘时同样必须有序。
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct Staging {
    pub added: BTreeMap<String, Sha>,
    pub removed: BTreeMap<String, Sha>,
}

impl Staging {
    pub fn is_empty(&self) -> bool {
        self.added.is_empty() && self.removed.is_empty()
    }
}
