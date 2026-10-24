#!/usr/bin/env bash
# Quick hand test after a rebuild: starts the built daemon and shell with settings, history and received files in RAM (see sandbox.sh); everything is deleted when the shell closes.
# The sandbox starts with a synthetic transfer history (demo-history.py); NO_DEMO=1 starts it empty.
# Extra arguments go to the shell, for example: scripts/testing/run.sh --page settings
# Build first: build/build-env.sh cargo build && build/build-env.sh cmake --build dist/tmp/cmake-dev
set -u

[ "$(id -u)" -ne 0 ] || { echo "run as your normal user, without sudo" >&2; exit 1; }
ROOT=$(cd "$(dirname "$0")/../.." && pwd)
GUI=$ROOT/dist/tmp/cmake-dev/kuickshare
DAEMON=$ROOT/target/debug/kuickshared
[ -x "$GUI" ] && [ -x "$DAEMON" ] || { echo "binaries missing, build first (see header)" >&2; exit 1; }

if gdbus call --session -d org.freedesktop.DBus -o /org/freedesktop/DBus \
    -m org.freedesktop.DBus.NameHasOwner io.github.the_beper.KuickShare | grep -q true; then
    echo "another KuickShare is running on this session: quit it first" >&2
    exit 1
fi

# shellcheck source=sandbox.sh
. "$(dirname "$0")/sandbox.sh"
if [ -z "${NO_DEMO:-}" ]; then
    mkdir -p "$SANDBOX/home/.local/share/kuickshare"
    "$(dirname "$0")/demo-history.py" "$SANDBOX/home/.local/share/kuickshare/history.json"
fi
DAEMON_PID=
cleanup() {
    if [ -n "$DAEMON_PID" ]; then
        kill "$DAEMON_PID" 2>/dev/null
        sleep 1
        kill -9 "$DAEMON_PID" 2>/dev/null
    fi
    sandbox_cleanup
}
trap cleanup EXIT
trap 'exit 0' INT TERM HUP

RUST_LOG=${RUST_LOG:-info} env "${SANDBOX_ENV[@]}" "$DAEMON" &
DAEMON_PID=$!
sleep 2
env "${SANDBOX_ENV[@]}" "$GUI" "$@"
