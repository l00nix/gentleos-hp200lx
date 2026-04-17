#
# Copyright (c) 2026 luke8086
# Distributed under the terms of GPL-2 License.
#
# File: clean.pl - Script for cleaning up build artifacts
#

use File::Path;

rmtree('build');
unlink('data/data.c');
unlink('Makefile');
