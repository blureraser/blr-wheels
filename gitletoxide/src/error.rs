/// 所有失败都退化成给用户看的字符串。
///
/// 错误信息会被逐字比对（句号不可省），所以不要用 anyhow/thiserror，
/// 也不要在这里加前缀或包装。
pub type Result<T> = std::result::Result<T, String>;
