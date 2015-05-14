#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

my $pointer;

###################################################################################################
# Checks that warping is disabled while touching a border after a successful warp.
# Also check that moving away from the border resets this and we become eligible for warping
# again.
###################################################################################################

# Layout:
# #######
#
#       +---+
# +---+ | 3 |
# | 1 +-+-+-+
# +---+ 2 |
#     +---+
#
run_xedgewarp(outputs => [
    '200x200+0+100',
    '200x200+200+200',
    '200x200+300+0'
]);

# A: Pointer is initially warped.
warp_pointer(199, 150);
$pointer = get_pointer;
is($pointer->{x}, 200, 'pointer is warped (x)');
is($pointer->{y}, 200, 'pointer is warped (y)');

# B: We are now not eligible for warping and moving along the border does nothing
warp_pointer(205, 200);
$pointer = get_pointer;
is($pointer->{x}, 205, 'pointer is not warped (x)');
is($pointer->{y}, 200, 'pointer is not warped (y)');

# C: We move away from the border, become eligible again and move to the border,
#    which will now trigger a warp.
warp_pointer(205, 205);
warp_pointer(205, 200);
$pointer = get_pointer;
is($pointer->{x}, 300, 'pointer is warped (x)');
is($pointer->{y}, 199, 'pointer is warped (y)');

exit_xedgewarp;

###################################################################################################

done_testing;
