#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

run_xedgewarp(outputs => [ '400x200+0+0', '400x200+400+100' ]);

warp_pointer(399, 50);

my $pointer = get_pointer;
ok($pointer->{x} == 401, 'pointer has been moved to the output to the right');
ok($pointer->{y} == 100, 'pointer has been moved down to the top of the new output');

done_testing;
