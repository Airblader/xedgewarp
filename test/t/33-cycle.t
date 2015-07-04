#!/usr/bin/env perl
# vim:ts=4:sw=4:expandtab

use Test::More;
use xewtest;

my $pointer;

###################################################################################################
# Test warping on the far left / right / top / bottom output cycles.
###################################################################################################

# Layout:
# #######
#
#     +---+
#     | 4 |
# +---+---+---+
# | 2 | 1 | 3 |
# +---+---+---+
#     | 5 |
#     +---+
#
foreach my $cycle_mode (('none', 'vertical', 'horizontal', 'both')) {
    run_xedgewarp(cycle => $cycle_mode, outputs => [
        '200x200+200+200',
        '200x200+0+200',
        '200x200+400+200',
        '200x200+200+0',
        '200x200+200+400'
    ]);

    warp_pointer(300, 300);

    # A: Far left
    warp_pointer(0, 300);
    $pointer = get_pointer;
    if ($cycle_mode eq 'horizontal' || $cycle_mode eq 'both') {
        is($pointer->{x}, 599, 'pointer is warped to the right');
        is($pointer->{y}, 300, 'non-warp direction position does not mutate');
    } else {
        is($pointer->{x}, 0, 'nothing happened');
        is($pointer->{y}, 300, 'nothing happened');
    }

    warp_pointer(300, 300);

    # B: Far right
    warp_pointer(599, 300);
    $pointer = get_pointer;
    if ($cycle_mode eq 'horizontal' || $cycle_mode eq 'both') {
        is($pointer->{x}, 0, 'pointer is warped to the left');
        is($pointer->{y}, 300, 'non-warp direction position does not mutate');
    } else {
        is($pointer->{x}, 599, 'nothing happened');
        is($pointer->{y}, 300, 'nothing happened');
    }

    warp_pointer(300, 300);

    # C: Far top
    warp_pointer(300, 0);
    $pointer = get_pointer;
    if ($cycle_mode eq 'vertical' || $cycle_mode eq 'both') {
        is($pointer->{x}, 300, 'non-warp direction position does not mutate');
        is($pointer->{y}, 599, 'pointer is warped to the bottom');
    } else {
        is($pointer->{x}, 300, 'nothing happened');
        is($pointer->{y}, 0, 'nothing happened');
    }

    warp_pointer(300, 300);

    # D: Far bottom
    warp_pointer(300, 599);
    $pointer = get_pointer;
    if ($cycle_mode eq 'vertical' || $cycle_mode eq 'both') {
        is($pointer->{x}, 300, 'non-warp direction position does not mutate');
        is($pointer->{y}, 0, 'pointer is warped to the top');
    } else {
        is($pointer->{x}, 300, 'nothing happened');
        is($pointer->{y}, 599, 'nothing happened');
    }

    exit_xedgewarp;
}

###################################################################################################

# TODO Test more abstruse scenarios

###################################################################################################

done_testing;
