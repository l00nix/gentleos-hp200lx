#
# Copyright (c) 2026 luke8086
# Distributed under the terms of GPL-2 License.
#
# File: genmake.pl - Script for genereating Makefile and linker scripts
#

my @SOURCE_DIRS = ("apps", "data", "kernel", "lib", "gui");

my $MAKEFILE_TPL = <<'EOT';
all: disks .SYMBOLIC
    @echo All done!

disks: build/kernel.com build/boot.bin .SYMBOLIC
    perl misc/mkdisks.pl

run: all .SYMBOLIC
    build\kernel.com

boot: all .SYMBOLIC
    boot build\fd1440.img

regen: .SYMBOLIC
    perl misc/procbmp.pl
    perl misc/cproto.pl
    perl misc/genmake.pl

procbmp: .SYMBOLIC
    perl misc/procbmp.pl

clean: .SYMBOLIC
	-del build\apps\*.*
	-del build\data\*.*
	-del build\gui\*.*
	-del build\lib\*.*
	-del build\kernel\*.*
	-del build\*.*

OBJS = &
<OBJS>

INCLUDES = &
<INCLUDES>

build/boot.bin: boot/boot.s
    nasm -o build/boot.bin boot/boot.s

build\kernel.com: $(OBJS)
	wlink @misc/kernel.lnk

<OBJECT_RULES>
EOT

my $C_RULE_TPL = <<'EOT';
<OBJ_PATH>: <SRC> $(INCLUDES)
!ifdef TC
	tcc -mt -u -g1 -c -Iinclude -n<OBJ_DIR> <SRC>
!else
	wcc -fo=<OBJ_PATH> <SRC> @misc\wcc.occ
!endif
EOT

my $S_RULE_TPL = <<'EOT';
<OBJ_PATH>: <SRC>
	nasm -f obj -o <OBJ_PATH> <SRC>
EOT

sub path_to_dos {
    my ($p) = @_;
    $p =~ s{/}{\\}g;
    return $p;
}

sub dirname {
    my ($p) = @_;
    if ($p =~ m{^(.*)/[^/]*$}) { return $1; }
    return "";
}

sub splitext {
    my ($p) = @_;
    if ($p =~ m{^(.*)(\.[^./\\]*)$}) { return ($1, $2); }
    return ($p, "");
}

sub collect_sources {
    my @sources;

    foreach my $d (@SOURCE_DIRS) {
        push @sources, glob("$d/*.c");
        push @sources, glob("$d/*.s");
    }

    @sources = sort @sources;
    @sources = grep { $_ ne "kernel/start.s" } @sources;
    unshift @sources, "kernel/start.s";

    return @sources;
}

sub collect_includes {
    my @includes = sort(glob("include/*.h"));
    @includes = grep { !/_config\.h$/ } @includes;
    return @includes;
}

sub make_object_rule {
    my ($src) = @_;
    my ($base, $ext) = splitext($src);
    my $tpl = ($ext eq ".c") ? $C_RULE_TPL : $S_RULE_TPL;
    $tpl =~ s/\n+$//;

    my $obj_path = "build\\" . path_to_dos($base) . ".obj";
    my $obj_dir  = "build\\" . path_to_dos(dirname($src));
    my $src_dos  = path_to_dos($src);

    $tpl =~ s/<SRC>/$src_dos/g;
    $tpl =~ s/<OBJ_PATH>/$obj_path/g;
    $tpl =~ s/<OBJ_DIR>/$obj_dir/g;

    return $tpl;
}

sub generate_makefile {
    my ($sources_ref, $includes_ref) = @_;

    my $objs = join("\n", map {
        my ($b, $e) = splitext($_);
        "\tbuild/" . $b . ".obj &"
    } @$sources_ref);

    my $incs = join("\n", map { "\t" . $_ . " &" } @$includes_ref);

    my $rules = join("\n\n", map { make_object_rule($_) } @$sources_ref);

    my $content = $MAKEFILE_TPL;
    $content =~ s/<OBJS>/$objs/;
    $content =~ s/<INCLUDES>/$incs/;
    $content =~ s/<OBJECT_RULES>/$rules/;

    $content =~ s/\n/\r\n/g;

    open(my $fh, ">", "Makefile") or die "Cannot open Makefile: $!";
    binmode($fh);
    print $fh $content;
    close($fh);
}

sub generate_kernel_lnk {
    my ($sources_ref) = @_;

    my @lines = (
        "system dos com",
        "name build/kernel.com",
        "option nodefaultlibs",
        "option quiet",
    );

    foreach my $src (@$sources_ref) {
        my ($base, $ext) = splitext($src);
        my $obj_path = "build/" . $base . ".obj";
        push @lines, "file $obj_path";
    }

    push @lines, "";

    open(my $fh, ">", "misc/kernel.lnk") or die "Cannot open misc/kernel.lnk: $!";
    binmode($fh);
    print $fh join("\r\n", @lines);
    close($fh);
}

sub main {
    my @sources = collect_sources();
    my @includes = collect_includes();
    generate_makefile(\@sources, \@includes);
    generate_kernel_lnk(\@sources);
}

main();
