#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use xewtest;

run_xedgewarp(outputs => [ '400x200+0+0', '400x200+400+100' ]);

sleep 5;
warp_pointer(399, 50);
sleep 2;

done_testing;
