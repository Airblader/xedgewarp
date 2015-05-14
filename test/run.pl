#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use strict;
use warnings;
use utf8;
use v5.10;
use IPC::System::Simple qw(system);

my $pid;

my @tests = @ARGV;
@tests = <t/*.t> if @tests == 0;

sub wait_for_x {
    my $display = substr $ENV{XEWDISPLAY}, 1;
    while (1) {
        last if -S "/tmp/.X11-unix/X$display";
    } 
}

sub start_x_server {
    $ENV{XEWDISPLAY} //= ":99";
    return if $ENV{XEWDISPLAY} eq $ENV{DISPLAY};

    $pid = fork;
    if ($pid == 0) {
        exec "Xephyr", $ENV{XEWDISPLAY}, "-screen", "800x600", "-nolisten", "tcp";
        exit 1;
    }

    wait_for_x;
}

sub stop_x_server {
    if (defined $pid) {
        kill(15, $pid);
    }
}

start_x_server();
for my $test (@tests) {
    print "\nRunning $test...\n";
    system([0..255], "/bin/sh -c $test");
}
stop_x_server();
