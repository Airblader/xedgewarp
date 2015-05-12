#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

my $pointer;

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
