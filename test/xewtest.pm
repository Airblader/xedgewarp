#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab
package xewtest;

use strict;
use warnings;
use utf8;
use v5.10;

use Test::More;
use X11::XCB::Connection;

use Exporter 'import';
our @EXPORT = qw(
    run_xedgewarp
    $x
);

my $pid;
our $x = X11::XCB::Connection->new(display => ":99");
if ($x->has_error) {
    die "Cannot connect to X on display 99!\n";
}

sub run_xedgewarp {
    my %args = @_;
    $args{outputs} //= [ '800x600+0+0', '800x600+0+0' ];

    $pid = fork;
    if ($pid == 0) {
        $ENV{DISPLAY} = ":99";
        exec "/home/ingo/repos/xedgewarp/xedgewarp -o 800x600+0+0";
        exit 1;
    }
}

END {
    if (defined $pid && $pid != 0) {
        kill(15, $pid);
    }
}

1;