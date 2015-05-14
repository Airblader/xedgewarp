#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

my $pointer;

###################################################################################################
# Test warping left / right with two outputs.
###################################################################################################

# Layout:
# #######
#
# +---+
# | 1 +---+
# +---+ 2 |
#     +---+
#
run_xedgewarp(outputs => [
    '200x200+0+0',
    '200x200+200+100'
]);

# A: Left edge
warp_pointer(200, 250);
$pointer = get_pointer;
is($pointer->{x}, 199, 'pointer is warped to the neighboring output (left)');
is($pointer->{y}, 199, 'pointer is warped to the bottom of the new output');

# reset pointer to become eligible for warping again
warp_pointer(50, 50);

# B: Right edge
warp_pointer(199, 50);
$pointer = get_pointer;
is($pointer->{x}, 200, 'pointer is warped to the neighboring output (right)');
is($pointer->{y}, 100, 'pointer is warped to the top of the new output');

exit_xedgewarp;

###################################################################################################
# Test warping up / down with two outputs.
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
run_xedgewarp(outputs => [
    '200x200+0+0',
    '200x200+100+200'
]);

# A: Top edge
warp_pointer(250, 200);
$pointer = get_pointer;
is($pointer->{x}, 199, 'pointer is warped to the neighboring output (top)');
is($pointer->{y}, 199, 'pointer is warped to the right of the new output');

# reset pointer to become eligible for warping again
warp_pointer(50, 50);

# B: Bottom edge
warp_pointer(50, 199);
$pointer = get_pointer;
is($pointer->{x}, 100, 'pointer is warped to the neighboring output (bottom)');
is($pointer->{y}, 200, 'pointer is warped to the left of the new output');

exit_xedgewarp;

###################################################################################################

done_testing;
