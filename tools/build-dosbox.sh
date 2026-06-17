#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if ! command -v dosbox >/dev/null 2>&1; then
  echo "dosbox is not installed. On Ubuntu, run: tools/setup-ubuntu-dosbox.sh" >&2
  exit 127
fi

dosbox_cmd=(
  dosbox
  -c "mount c ${repo_root}"
  -c "c:"
  -c "call env"
  -c "copy _config.h config.h"
  -c "call autogen"
  -c "wmake"
  -c "exit"
)

if [[ -z "${DISPLAY:-}" ]] && command -v xvfb-run >/dev/null 2>&1; then
  xvfb-run -a "${dosbox_cmd[@]}"
else
  "${dosbox_cmd[@]}"
fi

err_files=$(find "${repo_root}" -maxdepth 1 -name "*.ERR" -print)
if [[ -n "${err_files}" ]]; then
  echo "Build produced error files:" >&2
  echo "${err_files}" >&2
  exit 1
fi

echo
echo "Build complete. Useful outputs:"
echo "  ${repo_root}/BUILD/KERNEL.COM"
echo "  ${repo_root}/BUILD/FD720.IMG"
echo "  ${repo_root}/BUILD/FD1440.IMG"
echo "  ${repo_root}/BUILD/DISK.IMG"
