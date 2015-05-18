#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab
package xewtest;

use strict;
use warnings;
use utf8;
use v5.10;

use X11::XCB::Connection;

use Exporter 'import';
our @EXPORT = qw(
    run_xedgewarp
    exit_xedgewarp
    warp_pointer
    get_pointer
    $x
);

my $pid;
our $x = X11::XCB::Connection->new(display => $ENV{XEWDISPLAY});
if ($x->has_error) {
    die "Cannot connect to X on display $ENV{XEWDISPLAY}!\n";
}

sub run_xedgewarp {
    my %args = @_;
    $args{outputs} //= [ '400x200+0+0', '400x200+400+0' ];
    $args{mode} //= 'closest';

    # warp the pointer so we have a deterministic start scenario
    $x->root->warp_pointer(0, 0);

    $pid = open XEWOUT, '-|';
    if ($pid == 0) {
        $ENV{DISPLAY} = $ENV{XEWDISPLAY};

        open STDERR, '>&STDOUT';
        # we use stdbuf (coreutils) to disable buffering
        exec 'stdbuf', '-o', '0', '-e', '0', '../xedgewarp', '-l', 'trace', '-m', $args{mode}, '-o', join(',', @{$args{outputs}});
        exit 1;
    }

    while (<XEWOUT>) {
        return if /Entering event loop/;
    }
}

sub exit_xedgewarp {
    if (defined $pid && $pid != 0) {
        kill(15, $pid);
    }
}

sub warp_pointer {
    my ($pos_x, $pos_y) = (@_);
    $x->root->warp_pointer($pos_x, $pos_y);
}

sub get_pointer {
    my $cookie = $x->root->_conn->query_pointer($x->root->id);
    my $reply = $x->root->_conn->query_pointer_reply($cookie->{sequence});
    return {
        'x' => $reply->{root_x},
        'y' => $reply->{root_y}
    };
}

END {
    exit_xedgewarp;
}

1;
