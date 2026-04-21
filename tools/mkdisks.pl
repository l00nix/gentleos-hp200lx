#
# Copyright (c) 2026 luke8086
# Distributed under the terms of GPL-2 License.
#
# File: mkdisks.pl - Script for creating disk images
#

sub slurp {
    my ($path) = @_;
    open(my $f, $path) or die "Cannot read $path\n";
    binmode $f;
    local $/;
    my $data = <$f>;
    close $f;
    return $data;
}

sub make_disk {
    my ($path, $size) = @_;
    my $boot = slurp("build/boot.bin");
    my $kernel = slurp("build/kernel.com");

    print "Creating $path... ";

    unlink $path;
    open(my $out, ">$path") or die "Cannot write $path\n";
    binmode $out;
    print $out $boot;
    print $out $boot;
    print $out $kernel;
    print $out "\0" x ($size - length($boot) * 2 - length($kernel));
    close $out or die "Write error\n";

    print "Done\n";
}

make_disk("build/fd720.img", 720 * 1024);
make_disk("build/fd1440.img", 1440 * 1024);
