#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

my $pointer;

###################################################################################################
# 7: Test warping left / right with two outputs with relative warp mode.
###################################################################################################

# Layout:
# #######
#
# +---+
# | 1 +---+
# +---+ 2 |
#     +---+
#
run_xedgewarp(mode => 'relative', outputs => [
    '200x200+0+0',
    '200x200+200+100'
]);

# A: Left edge
warp_pointer(200, 250);
$pointer = get_pointer;
is($pointer->{x}, 199, 'pointer is warped to the neighboring output (left)');
is($pointer->{y}, 150, 'pointer is warped to the correct relative position (y)');

# reset pointer to become eligible for warping again
warp_pointer(50, 50);

# B: Right edge
warp_pointer(199, 50);
$pointer = get_pointer;
is($pointer->{x}, 200, 'pointer is warped to the neighboring output (right)');
is($pointer->{y}, 150, 'pointer is warped to the correct relative position (y)');

exit_xedgewarp;

###################################################################################################
# 7: Test warping up / down with two outputs with relative warp mode.
###################################################################################################

# Layout:
# #######
#
# +---+
# | 1 |
# +-+-+-+
#   | 2 |
#   +---+
#
run_xedgewarp(mode => 'relative', outputs => [
    '200x200+0+0',
    '200x200+100+200'
]);

# A: Top edge
warp_pointer(250, 200);
$pointer = get_pointer;
is($pointer->{x}, 150, 'pointer is warped to the correct relative position (x)');
is($pointer->{y}, 199, 'pointer is warped to the neighboring output (top)');

# reset pointer to become eligible for warping again
warp_pointer(50, 50);

# B: Bottom edge
warp_pointer(50, 199);
$pointer = get_pointer;
is($pointer->{x}, 150, 'pointer is warped to the correct relative position (x)');
is($pointer->{y}, 200, 'pointer is warped to the neighboring output (bottom)');

exit_xedgewarp;

###################################################################################################
# 7: Test relative warping is actually relative on different-sized outputs.
###################################################################################################

# Layout:
# #######
#
# +---+---+
# | 1 |   |
# +---+ 2 |
#     |   |
#     +---+
#
run_xedgewarp(mode => 'relative', outputs => [
    '200x200+0+0',
    '200x400+200+0'
]);

warp_pointer(200, 300);
$pointer = get_pointer;
is($pointer->{x}, 199, 'pointer is warped to the neighboring output');
is($pointer->{y}, 150, 'pointer is warped to the correct relative position (y)');

###################################################################################################

done_testing;
