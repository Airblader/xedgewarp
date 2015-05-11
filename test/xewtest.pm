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
our $x = X11::XCB::Connection->new(display => ":99");
if ($x->has_error) {
    die "Cannot connect to X on display 99!\n";
}

sub wait_for_startup {
    # TODO
    sleep 2;
}

sub run_xedgewarp {
    my %args = @_;
    $args{outputs} //= [ '400x200+0+0', '400x200+400+0' ];

    # warp the pointer so we have a deterministic start scenario
    $x->root->warp_pointer(0, 0);

    $pid = fork;
    if ($pid == 0) {
        $ENV{DISPLAY} = ":99";
        exec '../xedgewarp -o "' . join(',', @{$args{outputs}}) . '"';
        exit 1;
    }

    wait_for_startup;
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
