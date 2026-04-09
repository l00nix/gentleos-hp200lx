#
# This file is auto-generated. ***ANY*** changes here will be lost
#

package Errno;
use vars qw(@EXPORT_OK %EXPORT_TAGS @ISA $VERSION %errno $AUTOLOAD);
use Exporter ();
use Config;
use strict;

"$Config{'archname'}-$Config{'osvers'}" eq
"dos-djgpp-djgpp" or
	die "Errno architecture (dos-djgpp-djgpp) does not match executable architecture ($Config{'archname'}-$Config{'osvers'})";

$VERSION = "1.111";
@ISA = qw(Exporter);

@EXPORT_OK = qw(ENFILE EACCES EPERM EPIPE ERANGE ENOSPC ESPIPE ENOTTY
	ENOMEM ENOSYS EAGAIN EXDEV ECHILD ENOTDIR ESRCH EISDIR ENMFILE EROFS
	ENODEV EEXIST EBADF EMLINK EINVAL ENOLCK ENXIO ENAMETOOLONG E2BIG
	EBUSY EINTR ENOENT ENOTEMPTY EDOM EIO EDEADLK EFAULT EFBIG ENOEXEC
	EMFILE);

%EXPORT_TAGS = (
    POSIX => [qw(
	E2BIG EACCES EAGAIN EBADF EBUSY ECHILD EDEADLK EDOM EEXIST EFAULT
	EFBIG EINTR EINVAL EIO EISDIR EMFILE EMLINK ENAMETOOLONG ENFILE ENODEV
	ENOENT ENOEXEC ENOLCK ENOMEM ENOSPC ENOSYS ENOTDIR ENOTEMPTY ENOTTY
	ENXIO EPERM EPIPE ERANGE EROFS ESPIPE ESRCH EXDEV
    )]
);

sub EDOM () { 1 }
sub ERANGE () { 2 }
sub E2BIG () { 3 }
sub EACCES () { 4 }
sub EAGAIN () { 5 }
sub EBADF () { 6 }
sub EBUSY () { 7 }
sub ECHILD () { 8 }
sub EDEADLK () { 9 }
sub EEXIST () { 10 }
sub EFAULT () { 11 }
sub EFBIG () { 12 }
sub EINTR () { 13 }
sub EINVAL () { 14 }
sub EIO () { 15 }
sub EISDIR () { 16 }
sub EMFILE () { 17 }
sub EMLINK () { 18 }
sub ENAMETOOLONG () { 19 }
sub ENFILE () { 20 }
sub ENODEV () { 21 }
sub ENOENT () { 22 }
sub ENOEXEC () { 23 }
sub ENOLCK () { 24 }
sub ENOMEM () { 25 }
sub ENOSPC () { 26 }
sub ENOSYS () { 27 }
sub ENOTDIR () { 28 }
sub ENOTEMPTY () { 29 }
sub ENOTTY () { 30 }
sub ENXIO () { 31 }
sub EPERM () { 32 }
sub EPIPE () { 33 }
sub EROFS () { 34 }
sub ESPIPE () { 35 }
sub ESRCH () { 36 }
sub EXDEV () { 37 }
sub ENMFILE () { 38 }

sub TIEHASH { bless [] }

sub FETCH {
    my ($self, $errname) = @_;
    my $proto = prototype("Errno::$errname");
    my $errno = "";
    if (defined($proto) && $proto eq "") {
	no strict 'refs';
	$errno = &$errname;
        $errno = 0 unless $! == $errno;
    }
    return $errno;
}

sub STORE {
    require Carp;
    Carp::confess("ERRNO hash is read only!");
}

*CLEAR = \&STORE;
*DELETE = \&STORE;

sub NEXTKEY {
    my($k,$v);
    while(($k,$v) = each %Errno::) {
	my $proto = prototype("Errno::$k");
	last if (defined($proto) && $proto eq "");
    }
    $k
}

sub FIRSTKEY {
    my $s = scalar keys %Errno::;	# initialize iterator
    goto &NEXTKEY;
}

sub EXISTS {
    my ($self, $errname) = @_;
    my $proto = prototype($errname);
    defined($proto) && $proto eq "";
}

tie %!, __PACKAGE__;

1;
__END__

=head1 NAME

Errno - System errno constants

=head1 SYNOPSIS

    use Errno qw(EINTR EIO :POSIX);

=head1 DESCRIPTION

C<Errno> defines and conditionally exports all the error constants
defined in your system C<errno.h> include file. It has a single export
tag, C<:POSIX>, which will export all POSIX defined error numbers.

C<Errno> also makes C<%!> magic such that each element of C<%!> has a
non-zero value only if C<$!> is set to that value. For example:

    use Errno;

    unless (open(FH, "/fangorn/spouse")) {
        if ($!{ENOENT}) {
            warn "Get a wife!\n";
        } else {
            warn "This path is barred: $!";
        } 
    } 

If a specified constant C<EFOO> does not exist on the system, C<$!{EFOO}>
returns C<"">.  You may use C<exists $!{EFOO}> to check whether the
constant is available on the system.

=head1 CAVEATS

Importing a particular constant may not be very portable, because the
import will fail on platforms that do not have that constant.  A more
portable way to set C<$!> to a valid value is to use:

    if (exists &Errno::EFOO) {
        $! = &Errno::EFOO;
    }

=head1 AUTHOR

Graham Barr <gbarr@pobox.com>

=head1 COPYRIGHT

Copyright (c) 1997-8 Graham Barr. All rights reserved.
This program is free software; you can redistribute it and/or modify it
under the same terms as Perl itself.

=cut

