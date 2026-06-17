#!/usr/bin/env bash
set -euo pipefail

sudo apt-get update
sudo apt-get install -y \
  dosbox \
  git \
  qemu-system-x86 \
  xvfb

echo "Ubuntu DOSBox build prerequisites are installed."
echo "From the GentleOS checkout, run: tools/build-dosbox.sh"
