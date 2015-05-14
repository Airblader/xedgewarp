#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

my $pointer;

###################################################################################################
# 7: When multiple outputs are available, make sure to select the nearest one.
###################################################################################################

# Layout:
# #######
#
#     +---+
# +---+ 2 |
# |   +---+
# | 1 |
# |   +---+
# +---+ 3 |
#     +---+
#
run_xedgewarp(outputs => [
    '200x400+0+100',
    '200x200+200+0',
    '200x200+200+400'
]);

# A: Above half, warp to output 2
warp_pointer(199, 299);
$pointer = get_pointer;
is($pointer->{x}, 200, 'pointer is warped to next output (x)');
is($pointer->{y}, 199, 'pointer is warped to output 2 (y)');

# B: Below half, warp to output 3
warp_pointer(199, 301);
$pointer = get_pointer;
is($pointer->{x}, 200, 'pointer is warped to next output (x)');
is($pointer->{y}, 400, 'pointer is warped to output 3 (y)');

exit_xedgewarp;

###################################################################################################

done_testing;
