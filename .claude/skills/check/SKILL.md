---
name: check
description: Use before proposing a commit or a pull request: runs every KuickShare check (rustfmt, Clippy, Rust tests, vendored library tests, shell build, ctest, qmllint, catalogs) the way CI does.
---

# Full check

Run from the repository root; everything that needs Qt, KF6 or `dbus-daemon` goes through `build/build-env.sh` (the host has no ECM: a bare `cmake --build` fails). Report only what you saw pass.

```sh
build/build-env.sh build/check.sh      # rustfmt (also third_party/rqs_lib), Clippy, Rust tests (also rqs_lib), shell build, ctest, qmllint, metainfo, desktop file, catalogs
python3 .claude/scripts/po.py status
```

The script is the one list of checks and CI runs it too: add new checks there, not here. After changing `build/Containerfile`, rebuild the local image first (`podman build -t localhost/kuickshare-build:fedora45 -f build/Containerfile build`).

Afterwards:
- `git status --short` must show only intended files; `dist/` and `target/` are ignored.
- Hardware paths (BLE, Wi-Fi, NetworkManager, phones) have no automated test: say which changes are untested on a phone.
- Then the `code-reviewer` agent, as the global instructions say, before suggesting a commit.
