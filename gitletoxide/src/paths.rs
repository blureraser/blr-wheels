use std::path::{Path, PathBuf};

pub const GITLET_DIR: &str = ".gitlet";
pub const HEAD_FILE: &str = "HEAD";
pub const STAGING_FILE: &str = "staging";

pub fn gitlet(root: &Path) -> PathBuf {
    root.join(GITLET_DIR)
}

pub fn head(root: &Path) -> PathBuf {
    gitlet(root).join(HEAD_FILE)
}

pub fn staging(root: &Path) -> PathBuf {
    gitlet(root).join(STAGING_FILE)
}

pub fn refs_heads(root: &Path) -> PathBuf {
    gitlet(root).join("refs").join("heads")
}

pub fn branch_ref(root: &Path, name: &str) -> PathBuf {
    refs_heads(root).join(name)
}

pub fn objects_commits(root: &Path) -> PathBuf {
    gitlet(root).join("objects").join("commits")
}

pub fn objects_blobs(root: &Path) -> PathBuf {
    gitlet(root).join("objects").join("blobs")
}

pub fn commit_object(root: &Path, sha: &str) -> PathBuf {
    objects_commits(root).join(sha)
}

pub fn blob_object(root: &Path, sha: &str) -> PathBuf {
    objects_blobs(root).join(sha)
}
