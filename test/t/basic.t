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
is($pointer->{x}, 198, 'pointer is warped to the neighboring output (left)');
is($pointer->{y}, 199, 'pointer is warped to the bottom of the new output');

# B: Right edge
warp_pointer(199, 50);
$pointer = get_pointer;
is($pointer->{x}, 201, 'pointer is warped to the neighboring output (right)');
is($pointer->{y}, 100, 'pointer is warped to the top of the new output');

exit_xedgewarp;

###################################################################################################
# Test warping each of the four edges in a setup with three outputs.
###################################################################################################

# Layout:
# #######
#
#   +---+
#   | 1 +---+
# +-+-+-+ 2 |
# | 3 | +---+
# +---+
#
run_xedgewarp(outputs => [
    '200x200+100+0',
    '200x200+300+100',
    '200x200+0+200'
]);

# A: Top edge
#warp_pointer(50, 200);
#$pointer = get_pointer;
#ok($pointer->{x} == 101, '');
#ok($pointer->{y} == 199, '');

# B: Left edge
#warp_pointer(300, 250);
#$pointer = get_pointer;
#ok($pointer->{x} == 299, '');
#ok($pointer->{y} == 199, '');

# C: Bottom edge
#warp_pointer();
#$pointer = get_pointer;
#ok($pointer->{x} == , '');
#ok($pointer->{y} == , '');

# A: Right edge
#warp_pointer();
#$pointer = get_pointer;
#ok($pointer->{x} == , '');
#ok($pointer->{y} == , '');

exit_xedgewarp;

###################################################################################################

done_testing;
