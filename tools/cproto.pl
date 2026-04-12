#
# Copyright (c) 2026 luke8086
# Distributed under the terms of GPL-2 License.
#
# File: cproto.pl - Script for auto-generating header files with prototypes
#

my @SOURCE_DIRS = ("apps", "data", "kernel", "lib", "gui");

sub process_file {
    my ($file) = @_;
    my @protos;

    open(my $fh, "<", $file) or die "Cannot open $file: $!";
    my $content = do { local $/; <$fh> };
    close($fh);

    $content =~ s/\r\n/\n/g;

    while ($content =~ /^global ([^{=;]*)[{=;]/mg) {
        my $decl = $1;
        $decl =~ s/\n/ /g;
        $decl =~ s/\s+/ /g;
        $decl =~ s/^\s+|\s+$//g;
        push @protos, "extern $decl;";
    }

    return @protos;
}

sub main {
    foreach my $dir (@SOURCE_DIRS) {
        my @files = sort(glob("$dir/*.c"));
        my @lines;

        foreach my $file (@files) {
            my @protos = process_file($file);
            next unless @protos;
            push @lines, "/* $file */";
            push @lines, @protos;
        }

        my $header = "include/p_${dir}.h";
        open(my $fh, ">", $header) or die "Cannot open $header: $!";
        binmode($fh);
        print $fh join("\r\n", @lines) . "\r\n";
        close($fh);
    }
}

main();
