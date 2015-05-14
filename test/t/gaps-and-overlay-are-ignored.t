#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

my $pointer;

###################################################################################################
# Checks that outputs which have a gap in between or which are not disjoint are not considered
# for warping.
###################################################################################################

# Layout:
# #######
#
# +-----+ +-----+
# |     | |     |
# |  1  | |  2  |
# +- - -+ |     |
# + - - + +-----+
# |  3  |
# |     |
# +-----+
#
run_xedgewarp(outputs => [
    '200x200+0+0',
    '200x200+300+0',
    '200x200+0+100'
]);

# A: Output with a gap is not considered
warp_pointer(199, 50);
$pointer = get_pointer;
is($pointer->{x}, 199, 'pointer is not warped to an output with a gap (x)');
is($pointer->{y}, 50, 'pointer is not warped to an output with a gap (y)');

# B: Output which is not disjoint is not considered
warp_pointer(50, 199);
$pointer = get_pointer;
is($pointer->{x}, 50, 'pointer is not warped to an overlaying output (x)');
is($pointer->{y}, 199, 'pointer is not warped to an overlaying output (y)');

exit_xedgewarp;

###################################################################################################

done_testing;
