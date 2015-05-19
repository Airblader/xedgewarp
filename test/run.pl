#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use strict;
use warnings;
use utf8;
use v5.10;
use IPC::System::Simple qw(capture $EXITVAL EXIT_ANY);

my $pid;

my @tests = @ARGV;
@tests = <t/*.t> if @tests == 0;

$ENV{XEWDISPLAY} //= ":99";

sub wait_for_x {
    my $display = substr $ENV{XEWDISPLAY}, 1;
    while (1) {
        last if -S "/tmp/.X11-unix/X$display";
    } 
}

sub start_x_server {
    return if $ENV{XEWDISPLAY} eq $ENV{DISPLAY};

    $pid = fork;
    if ($pid == 0) {
        exec "Xephyr", $ENV{XEWDISPLAY}, "-screen", "800x600", "-nolisten", "tcp", "+extension", "XINERAMA";
        exit 1;
    }

    wait_for_x;
}

sub stop_x_server {
    if (defined $pid) {
        kill(15, $pid);
        waitpid($pid, 0);
    }
}

BEGIN {
#    start_x_server;
}

END {
#    stop_x_server;
}

my $failures = 0;
for my $test (@tests) {
    start_x_server;

    print "\nRunning $test...\n";
    my @lines = capture(EXIT_ANY, "/bin/sh -c $test");

    stop_x_server;

    if ($EXITVAL != 0) {
        $failures++;
        next;
    }

    for (@lines) {
        print "$_";

        next unless /^not ok/;
        $failures++;
    }
}

if ($failures == 0) {
    print "\n\nAll tests successful.\n";
    exit 0;
} else {
    print "\n\nFailed tests: $failures\n";
    exit 1;
}
