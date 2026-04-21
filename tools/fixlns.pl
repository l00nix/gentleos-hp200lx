#
# Copyright (c) 2026 luke8086
# Distributed under the terms of GPL-2 License.
#
# File: fixlns.pl - Fix line endings in source code
#

my @GLOBS = (
    "apps/*",
    "boot/*",
    "boot2/*",
    "data/*",
    "gui/*",
    "include/*",
    "kernel/*",
    "lib/*",
    "misc/*",
    "tools/*.pl",
);

sub slurp {
    my ($path) = @_;
    open(my $f, "<", $path) or die "Cannot read $path: $!\n";
    binmode $f;
    local $/;
    my $data = <$f>;
    close $f;
    return $data;
}

sub spit {
    my ($path, $data) = @_;
    open(my $f, ">", $path) or die "Cannot write $path: $!\n";
    binmode $f;
    print $f $data;
    close $f or die "Write error on $path\n";
}

sub collect_files {
    my @files;
    foreach my $g (@GLOBS) {
        push @files, grep { -f $_ } glob($g);
    }
    return sort @files;
}

sub main {
    foreach my $path (collect_files()) {
        my $old = slurp($path);
        my $new = $old;
        $new =~ s/\r\n/\n/g;
        $new =~ s/\n/\r\n/g;

        next if $new eq $old;

        print "Fixing $path\n";
        spit($path, $new)
    }
}

main();
