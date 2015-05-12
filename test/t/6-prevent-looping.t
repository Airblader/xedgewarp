#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

my $pointer;

###################################################################################################
# ISSUE #6
# Check that we don't "loop", i.e., if we warped the pointer, that warp should not trigger another
# warp.
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

warp_pointer(199, 150);
$pointer = get_pointer;
is($pointer->{x}, 200, 'pointer is warped to the output on the right and no further');
is($pointer->{y}, 200, 'pointer is warped to the top of the new output');

warp_pointer(201, 200);
$pointer = get_pointer;
is($pointer->{x}, 300, 'pointer is warped to the output above');
is($pointer->{y}, 199, 'pointer is warped to the bottom of the new output');

exit_xedgewarp;

###################################################################################################

done_testing;
