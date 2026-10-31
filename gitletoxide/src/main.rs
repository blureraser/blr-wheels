use std::path::PathBuf;

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();
    let root: PathBuf = std::env::current_dir().expect("cannot read current directory");
    if let Err(msg) = gitlet::cmd::dispatch(&root, &args) {
        println!("{msg}");
    }
    std::process::exit(0);
}
