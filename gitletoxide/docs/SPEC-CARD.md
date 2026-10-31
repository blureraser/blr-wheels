# Gitlet 规格卡（CS61B sp21 · proj2）

来源：<https://sp21.datastructur.es/materials/proj/proj2/proj2>（抓取全文 1792 行）
本文件只写**验收需要的信息**：行为、逐字输出、失败信息。不解释设计动机。

---

## 0. 三条铁律

| 铁律 | 含义 |
|---|---|
| 提交图不可变 | commit 创建后永不修改、永不删除。`rm-branch` / `reset` 只挪指针 |
| 内容寻址 | id = SHA-1(内容)。判断"文件变没变"比较 SHA，**不比较内容**。父 commit 已有的 blob 不存第二份 |
| 一切状态在盘上 | 一次运行只执行一条命令，跨命令状态必须序列化到 `.gitlet/` |

---

## 1. 全局契约

### 1.1 四关检查（顺序不可换）

| # | 情况 | 输出 |
|---|---|---|
| G1 | `args` 为空 | `Please enter a command.` |
| G2 | 命令名不在表里 | `No command with that name exists.` |
| G3 | 操作数个数/格式不对 | `Incorrect operands.` |
| G4 | 非 `init` 且当前目录无 `.gitlet` | `Not in an initialized Gitlet directory.` |

先查目录再查操作数，会把 G3 错报成 G4。

### 1.2 输出纪律

1. 除规格要求的输出外，**一个字符都不打印**（spec 原文：autograder 会因为多打印而失败）。
2. **错误也 `exit 0`**（对应 Java 的 `System.exit(0)`）。测试用 `check_output`，非零退出直接判失败。
3. **不要往 stderr 写**。tester.py 用 `stderr=STDOUT`，debug 输出会污染比对。Rust 侧含义：禁用 `eprintln!`，禁用会 panic 的 `unwrap()` / `expect()`。

---

## 2. 命令规范

| # | 命令 | 用法 | 核心行为 | 失败信息（逐字） |
|---|---|---|---|---|
| 1 | init | `init` | 建 `.gitlet`；初始 commit：message `initial commit`、timestamp = **0**、快照空、无父；建 `master` 并设 HEAD | `A Gitlet version-control system already exists in the current directory.` |
| 2 | add | `add <file>` | 存 blob 进 staging.added；**与 HEAD 版本内容相同 → 不 stage 且从 added 移除**；同时从 removed 划掉 | `File does not exist.` |
| 3 | commit | `commit "msg"` | 快照 = 父快照 + staged 增删；清空 staging；HEAD 前移；**只改 `.gitlet/`，不动工作区** | `No changes added to the commit.` / `Please enter a commit message.` |
| 4 | rm | `rm <file>` | 已 staged 增加 → 取消 stage；HEAD 跟踪 → 标 removed **并从工作区删除** | `No reason to remove the file.` |
| 5 | log | `log` | HEAD 沿**第一父**回溯到初始提交，新的在前 | — |
| 6 | global-log | `global-log` | 所有 commit，顺序任意 | — |
| 7 | find | `find "msg"` | 打印所有 message 精确匹配的 commit id | `Found no commit with that message.` |
| 8 | status | `status` | 见 §3.1 | — |
| 9 | checkout① | `checkout -- <file>` | HEAD 版本覆盖工作区，**不 stage**，HEAD 不变 | `File does not exist in that commit.` |
| 10 | checkout② | `checkout <id> -- <file>` | 指定 commit 版本覆盖，id 可缩写 | `No commit with that id exists.` / `File does not exist in that commit.` |
| 11 | checkout③ | `checkout <branch>` | 铺满目标快照 → 切 HEAD → 删"当前跟踪但目标没有"的文件 → **清空 staging** | `No such branch exists.` / `No need to checkout the current branch.` / untracked-in-the-way |
| 12 | branch | `branch <name>` | 只建指针，**不切换** | `A branch with that name already exists.` |
| 13 | rm-branch | `rm-branch <name>` | 只删指针 | `A branch with that name does not exist.` / `Cannot remove the current branch.` |
| 14 | reset | `reset <id>` | = 对任意 commit 执行 checkout③ + 挪**当前分支**指针 + 清 staging | `No commit with that id exists.` / untracked-in-the-way |
| 15 | merge | `merge <branch>` | 见 §3.2 | `You have uncommitted changes.` / `A branch with that name does not exist.` / `Cannot merge a branch with itself.` / untracked-in-the-way |

**untracked-in-the-way 全文**（checkout③ / reset / merge 共用）：

```
There is an untracked file in the way; delete it, or add and commit it first.
```

必须在任何动作之前检查。

---

## 3. 展开规则

### 3.1 status

⚠️ 后两个区块是 **EC（32 分）**，可以只留标题。基础版只要 3 个区块。

```
=== Branches ===
*master
other-branch
=== Staged Files ===
wug.txt
wug2.txt
=== Removed Files ===
goodbye.txt
=== Modifications Not Staged For Commit ===
junk.txt (deleted)
wug3.txt (modified)
=== Untracked Files ===
random.stuff
```

- 区块之间一个空行，**整体以空行结尾**
- 条目字典序，`*` 不参与排序
- `*` 紧贴当前分支名
- "modified but not staged" 的四条定义：
  1. 当前 commit 跟踪、工作区内容变了、但没 stage → `(modified)`
  2. 没标 removed、被 commit 跟踪、但从工作区删了 → `(deleted)`
  3. staged for addition，但工作区内容与 staged 不同 → `(modified)`
  4. staged for addition，但从工作区删了 → `(deleted)`
- Untracked = 工作区里既没 staged 也没被跟踪的文件（包括"staged 删除后又被重新创建"的）。**忽略子目录**。

### 3.2 merge

**split point（最近公共祖先）**
1. 当前 HEAD 的所有祖先（含自身）成集合 C
2. 从给定 HEAD 逐层 BFS 上溯，第一个命中 C 的节点即 split point

**两个短路**
- split == 给定 HEAD → 打印 `Given branch is an ancestor of the current branch.`，什么都不做
- split == 当前 HEAD → 等价 checkout 给定分支 → 打印 `Current branch fast-forwarded.`

**八条逐文件规则**（base/cur/given 比 blob SHA）

| base | cur | given | 结果 |
|---|---|---|---|
| 有 | 没改 | 改了 | 取给定并自动 stage |
| 有 | 改了 | 没改 | 保持 |
| — | 改了 | 改法相同 | 不动 |
| 无 | 有 | 无 | 保留 |
| 无 | 无 | 有 | checkout 并 stage |
| 有 | 没改 | 删了 | 删除并 untrack |
| 有 | 删了 | 没改 | 保持不存在 |
| — | 改法不同 | | **冲突** |

"改法不同"包括：内容都改了且不同 / 一个改了另一个删了 / base 没有但两边内容不同。

**冲突拼接**（纯字符串拼接，删除的文件当空文件）

```
"<<<<<<< HEAD\n" + 当前分支内容 + "=======\n" + 给定分支内容 + ">>>>>>>"
```

⚠️ `>>>>>>>` 后面**没有换行**。文件本身末尾无换行时会出现
`...current branch=======` 这种连在一起的效果，这是正常的。

**收尾**
- 自动 commit，message = `Merged <给定分支> into <当前分支>.`（**有句号**）
- 两父：第一父 = 当前 HEAD，第二父 = 给定 HEAD
- 有冲突额外打印 `Encountered a merge conflict.`（打终端，不进 log）

### 3.3 缩写 ID

任意十六进制前缀，只要**唯一匹配**即可。歧义情况 spec 未定义，统一返回 `No commit with that id exists.` 即可。

### 3.4 log 格式

```
===
commit 3e8bf1d794ca2e9ef8a4007275acf3751c7170ff
Date: Sat Nov 11 12:30:00 2017 -0800
Merged development into master.

===
commit 4975af1...
```

- `===` 单独一行
- merge commit 在 commit 行下多一行：`Merge: <父1前7位> <父2前7位>`
- message 后面有一个空行
- 日期形状必须是 `Sat Nov 11 12:30:00 2017 -0800`（见 §6）

---

## 4. 持久化布局

```
.gitlet/
├── HEAD                  # 只存分支名，如 "master"
├── refs/heads/<branch>   # 内容 = commit 的 40 位 hex
├── objects/commits/<sha> # 序列化 Commit
├── objects/blobs/<sha>   # 文件原始字节
└── staging               # 序列化 Staging
```

**Rust 侧自由度**：格式随便（serde_json / bincode 都行），测试只检查 stdout、退出码、工作区文件。课程自己的 staff 实现甚至用 `.gitlet.enc` 加密，明确不让你看内部结构。

---

## 5. 错误路径勾选清单（25 条）

### 全局（4）

- [ ] G1 `Please enter a command.`
- [ ] G2 `No command with that name exists.`
- [ ] G3 `Incorrect operands.`
- [ ] G4 `Not in an initialized Gitlet directory.`

### init（1）

- [ ] `A Gitlet version-control system already exists in the current directory.`

### add（1）

- [ ] `File does not exist.`

### commit（2）

- [ ] `No changes added to the commit.`
- [ ] `Please enter a commit message.`

### rm（1）

- [ ] `No reason to remove the file.`

### find（1）

- [ ] `Found no commit with that message.`

### checkout①（1）

- [ ] `File does not exist in that commit.`

### checkout②（2）

- [ ] `No commit with that id exists.`
- [ ] `File does not exist in that commit.`

### checkout③（3）

- [ ] `No such branch exists.`
- [ ] `No need to checkout the current branch.`
- [ ] `There is an untracked file in the way; delete it, or add and commit it first.`

### branch（1）

- [ ] `A branch with that name already exists.`

### rm-branch（2）

- [ ] `A branch with that name does not exist.`
- [ ] `Cannot remove the current branch.`

### reset（2）

- [ ] `No commit with that id exists.`
- [ ] untracked-in-the-way

### merge（4）

- [ ] `You have uncommitted changes.`
- [ ] `A branch with that name does not exist.`
- [ ] `Cannot merge a branch with itself.`
- [ ] untracked-in-the-way

---

## 6. 日期格式（唯一必须逐字对齐的东西）

tester.py 的 `${DATE}` 正则：

```
Date: \w\w\w \w\w\w \d+ \d\d:\d\d:\d\d \d\d\d\d [-+]\d\d\d\d
```

对应 `Sat Nov 11 12:30:00 2017 -0800`。

chrono 格式串：

```rust
format!("{}", dt.format("%a %b %-d %H:%M:%S %Y %z"))
```

- 必须用 `%-d`（无填充），`%e` 会输出 ` 1`（前面有空格），正则的 `\w\w\w \w\w\w \d+` 只允许一个空格，会匹配失败
- `%a` / `%b` 在 chrono 里固定输出英文缩写，不受系统 locale 影响
- 初始提交 timestamp = 0 → `Thu Jan 1 00:00:00 1970 +0000`

---

## 7. 测试分层

| 层次 | 工具 | 测什么 | 建议数量 |
|---|---|---|---|
| 单元 | `cargo test` + `tempfile::TempDir` | 哈希、LCA、合并决策、冲突拼接、日期格式化、status 计算、前缀解析 | 40–60 |
| 集成 | `.in` + tester.py | 跨进程命令序列、逐字输出、25 条错误路径、重启后状态 | 20–25 |
| 冒烟 | `assert_cmd`（可选） | 退出码恒 0、stderr 为空 | 3–5 |

**接 tester.py 的两行改动**（`~/Desktop/CS61B/proj2/testing/tester.py`）：

```python
JAVA_COMMAND = "/home/EraserB/Desktop/blr-wheels/gitlet-rs/target/release/gitlet"
GITLET_CLASS = ""
```

运行：

```bash
python3 tester.py --progdir ~/Desktop/CS61B/proj2 samples/test01-init.in
```

- `--progdir` 必须给：不给会去找 `gitlet/Main.class`，找不到直接退出
- 测试不检查 `.gitlet` 内部，只检查 stdout + 退出码 + 工作区文件（已实测）

**不接 tester.py 的最短自测**：

```bash
cd /tmp && rm -rf t && mkdir t && cd t
G=~/Desktop/blr-wheels/gitlet-rs/target/release/gitlet
$G init && $G status
```

---

## 8. Rust 实现坑

1. **禁 `unwrap()` / `expect()` / `panic!`** — 会写 stderr 且退出码非零，测试直接挂。全部走 `Result` + `?`。
2. **`BTreeMap` 而非 `HashMap`** — commit 的 SHA 由内容决定，Map 顺序不定会让同一内容算出不同 SHA。
3. **`cwd` 只在 `main.rs` 出现一次** — `Repo` 显式持有 `root`，否则 `cargo test` 多线程并行时 `set_current_dir` 互相踩。
4. **merge 先算计划再执行** — 遍历提交图时又要改工作区文件会撞借用检查器。先产出 `Vec<FileAction>`，再统一落盘。
5. **`serde_json` 的字段顺序** — 结构体字段顺序固定，`BTreeMap` 有序，所以序列化字节确定；但**不要**直接对 serde 输出去哈希 commit，建议显式拼接 `msg + timestamp + parent + parent2 + sorted entries`。
6. **对象写入要幂等** — `put_blob` 同样字节只写一次，否则 `.gitlet` 体积会超限（spec 有体积要求）。
7. **时间戳用毫秒 `i64`** — 初始提交恰好 0。
8. **`format!("{:x}")` 对 SHA-1 输出不可用** — 用 `.iter().map(|b| format!("{:02x}", b)).collect::<String>()`。

---

## 9. 实现顺序（依赖关系）

```
init ──> add ──> commit ──> log ──> checkout①
              │                    │
              └─> rm               └─> checkout② ──> checkout③ ──> branch ──> rm-branch
                                                          │
                                                          └─> reset
                                                               │
                                                          merge（最后）
```

`status` 可以在任意时刻插入（它只读状态，不产生变更）。
