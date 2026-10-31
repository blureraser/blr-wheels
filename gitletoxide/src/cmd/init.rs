use std::path::Path;

use crate::error::Result;

/// 建 .gitlet/、初始 commit、master 分支、HEAD。
/// 初始 commit：message = "initial commit"，timestamp = 0，快照为空，无父。
pub fn run(root: &Path) -> Result<()> {
    let _ = root;
    todo!()
}
