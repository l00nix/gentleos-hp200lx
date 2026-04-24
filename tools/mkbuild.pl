#
# Copyright (c) 2026 luke8086
# Distributed under the terms of GPL-2 License.
#
# File: mkbuild.pl - Script for creating build output directories
#

my @SOURCE_DIRS = ("apps", "boot1", "boot2", "data", "kernel", "lib", "gui");

foreach my $d ("build", map { "build/$_" } @SOURCE_DIRS) {
    next if -d $d;
    mkdir $d or die "Cannot create $d: $!\n";
}
