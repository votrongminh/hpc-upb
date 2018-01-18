#!perl

# Program by Rodos rodos at haywood dot org

use Storable qw(dclone);
use Data::Dumper;

print "Lets play Rush Hour! \n";


# Lets define our current game state as a grid where each car is a different letter.
# Our special car is a marked with the specific letter T
# The boarder is a * and the gloal point on the edge is an @.
# The grid must be the same witdh and height 
# You can use a . to mark an empty space

# Grand Master
@startingGrid = (
 ['*','*','*','*','*','*','*','*'],
 ['*','.','.','A','O','O','O','*'],
 ['*','.','.','A','.','B','.','*'],
 ['*','.','T','T','C','B','.','@'],
 ['*','D','D','E','C','.','P','*'],
 ['*','.','F','E','G','G','P','*'],
 ['*','.','F','Q','Q','Q','P','*'],
 ['*','*','*','*','*','*','*','*']
);

# Now lets print out our grid board so we can see what it looks like.
# We will go through each row and then each column.
# As we do this we will record the list of cars (letters) we see into a hash

print "Here is your board.\n";

&printGrid(\@startingGrid);

# Lets find the cars on the board and the direction they are sitting

for $row (0 .. $#startingGrid) {
    for $col (0 .. $#{$startingGrid[$row]} ) {

        # Make spot the value of the bit on the grid we are looking at
        $spot = $startingGrid[$row][$col];

        # Lets record any cars we see into a "hash" of valid cars.
        # If the splot is a non-character we will ignore it cars are only characters
        unless ($spot =~ /\W/) {

            # We will record the direction of the car as the value of the hash key.
            # If the location above or below our spot is the same then the car must be vertical.
            # If its not vertical we mark as it as horizonal as it can't be anything else!

            if ($startingGrid[$row-1][$col] eq $spot || $startingGrid[$row+1] eq $spot) {
                $cars{$spot} = '|';
            } else {
                $cars{$spot} = '-';
            }
        }
    }
}

# Okay we should have printed our grid and worked out the unique cars
# Lets print out our list of cars in order

print "\nI have determined that you have used the following cars on your grid board.\n";
foreach $car (sort keys %cars) {
    print " $car$cars{$car}";
}
print "\n\n";

end;

&tryMoves();

end;

# Here are our subroutines for things that we want to do over and over again or things we might do once but for 
# clatiry we want to keep the main line of logic clear

sub tryMoves {

    # Okay, this is the hard work. Take the grid we have been given. For each car see what moves are possible
    # and try each in turn on a new grid. We will do a shallow breadth first search (BFS) rather than depth first. 
    # The BFS is achieved by throwing new sequences onto the end of a stack. You then keep pulling sequnces
    # from the front of the stack. Each time you get a new item of the stack you have to rebuild the grid to what
    # it looks like at that point based on the previous moves, this takes more CPU but does not consume as much
    # memory as saving all of the grid representations.

    my (@moveQueue);
    my (@thisMove);
    push @moveQueue, \@thisMove;

    # Whlst there are moves on the queue process them                
    while ($sequence = shift @moveQueue) { 

        # We have to make a current view of the grid based on the moves that got us here

        $currentGrid = dclone(\@startingGrid);
        foreach $step (@{ $sequence }) {
            $step =~ /(\w)-(\w)(\d)/;
            $car = $1; $dir = $2; $repeat = $3;

            foreach (1 .. $repeat) {
                &moveCarRight($car, $currentGrid) if $dir eq 'R';
                &moveCarLeft($car,  $currentGrid) if $dir eq 'L';
                &moveCarUp($car,    $currentGrid) if $dir eq 'U';
                &moveCarDown($car,  $currentGrid) if $dir eq 'D';
            }
        }

        # Lets see what are the moves that we can do from here.

        my (@moves);

        foreach $car (sort keys %cars) {
            if ($cars{$car} eq "-") {
                $l = &canGoLeft($car,$currentGrid);
                push @moves, "$car-L$l" if ($l);
                $l = &canGoRight($car,$currentGrid);
                push @moves, "$car-R$l" if ($l);
            } else {
                $l = &canGoUp($car,$currentGrid);
                push @moves, "$car-U$l" if ($l);
                $l = &canGoDown($car,$currentGrid);
                push @moves, "$car-D$l" if ($l);
            }
        }

        # Try each of the moves, if it solves the puzzle we are done. Otherwise take the new 
        # list of moves and throw it on the stack

        foreach $step (@moves) {

            $step =~ /(\w)-(\w)(\d)/;
            $car = $1; $dir = $2; $repeat = $3;

            my $newGrid = dclone($currentGrid);

            foreach (1 .. $repeat) {
                &moveCarRight($car, $newGrid) if $dir eq 'R';
                &moveCarLeft($car, $newGrid) if $dir eq 'L';
                &moveCarUp($car, $newGrid) if $dir eq 'U';
                &moveCarDown($car, $newGrid) if $dir eq 'D';
            }

            if (&isItSolved($newGrid)) {
                print sprintf("Solution in %d moves :\n", (scalar @{ $sequence }) + 1);
                print join ",", @{ $sequence };
                print ",$car-$dir$repeat\n";
                return;
            } else {

                # That did not create a solution, before we push this for further sequencing we want to see if this
                # pattern has been encountered before. If it has there is no point trying more variations as we already
                # have a sequence that gets here and it might have been shorter, thanks to our BFS

                if (!&seen($newGrid)) {
                    # Um, looks like it was not solved, lets throw this grid on the queue for another attempt
                    my (@thisSteps) = @{ $sequence };
                    push @thisSteps, "$car-$dir$repeat";
                    push @moveQueue, \@thisSteps;
                }
            }            
        }
    }
}    

sub isItSolved {

    my ($grid) = shift;

    my ($row, $col);
    my $stringVersion;

    foreach $row (@$grid) {
        $stringVersion .= join "",@$row;
    }

    # We know we have solve the grid lock when the T is next to the @, because that means the taxi is at the door
    if ($stringVersion =~ /\T\@/) {
        return 1;
    }
    return 0;
}    

sub seen {

    my ($grid) = shift;

    my ($row, $col);
    my $stringVersion;

    foreach $row (@$grid) {
        $stringVersion .= join "",@$row;
    }

    # Have we seen this before?
    if ($seen{$stringVersion}) {
        return 1;
    }
    $seen{$stringVersion} = 1;
    return 0;
}    

sub canGoDown {

    my ($car) = shift;

    return 0 if $cars{$car} eq "-";

    my ($grid) = shift;

    my ($row, $col);


    for ($row = $#{$grid}; $row >= 0; --$row) {
        for $col (0 .. $#{$grid->[$row]} ) {
            if ($grid->[$row][$col] eq $car) {
                # See how many we can move
                $l = 0;
                while ($grid->[++$row][$col] eq ".") {
                    ++$l;
                }
                return $l;
            }
        }
    }
    return 0;
}

sub canGoUp {

    my ($car) = shift;

    return 0 if $cars{$car} eq "-";

    my ($grid) = shift;

    my ($row, $col);

    for $row (0 .. $#{$grid}) {
        for $col (0 .. $#{$grid->[$row]} ) {
            if ($grid->[$row][$col] eq $car) {
                # See how many we can move
                $l = 0;
                while ($grid->[--$row][$col] eq ".") {
                    ++$l;
                } 
                return $l;
            }
        }
    }
    return 0;
}

sub canGoRight {

    my ($car) = shift;

    return 0 if $cars{$car} eq "|";

    my ($grid) = shift;

    my ($row, $col);

    for $row (0 .. $#{$grid}) {
        for ($col = $#{$grid->[$row]}; $col >= 0; --$col ) {
            if ($grid->[$row][$col] eq $car) {
                # See how many we can move
                $l = 0;
                while ($grid->[$row][++$col] eq ".") {
                    ++$l;
                } 
                return $l;
            }
        }
    }
    return 0;
}

sub canGoLeft {

    my ($car) = shift;

    return 0 if $cars{$car} eq "|";

    my ($grid) = shift;

    my ($row, $col);

    for $row (0 .. $#{$grid}) {
        for $col (0 .. $#{$grid->[$row]} ) {
            if ($grid->[$row][$col] eq $car) {
                # See how many we can move
                $l = 0;
                while ($grid->[$row][--$col] eq ".") {
                    ++$l;
                } 
                return $l;
            }
        }
    }
    return 0;
}

sub moveCarLeft {

    # Move the named car to the left of the passed grid. Care must be taken with the algoritm
    # to not move part of the car and then come across it again on the same pass and move it again 
    # so moving left requires sweeping left to right.

    # We need to know which car you want to move and the reference to the grid you want to move it on
    my ($car) = shift;
    my ($grid) = shift;

    # Only horizontal cards can move left
    die "Opps, tried to move a vertical car $car left" if $cars{$car} eq "|";

    my ($row, $col);

    for $row (0 .. $#{$grid}) {
        for $col (0 .. $#{$grid->[$row]} ) {
            if ($grid->[$row][$col] eq $car) {
                die "Tried to move car $car left into an occupied spot\n" if $grid->[$row][$col-1] ne ".";
                $grid->[$row][$col-1] = $car;
                $grid->[$row][$col] = ".";
            }
        }
    }
}

sub moveCarRight {

    # Move the named car to the right of the passed grid. Care must be taken with the algoritm
    # to not move part of the car and then come across it again on the same pass and move it again 
    # so moving right requires sweeping right to left (backwards).

    # We need to know which car you want to move and the reference to the grid you want to move it on
    my ($car) = shift;
    my ($grid) = shift;

    # Only horizontal cards can move right
    die "Opps, tried to move a vertical car $car right" if $cars{$car} eq "|";

    my ($row, $col);

    for $row (0 .. $#{$grid}) {
        for ($col = $#{$grid->[$row]}; $col >= 0; --$col ) {
            if ($grid->[$row][$col] eq $car) {
                die "Tried to move car $car right into an occupied spot\n" if $grid->[$row][$col+1] ne ".";
                $grid->[$row][$col+1] = $car;
                $grid->[$row][$col] = ".";
            }
        }
    }
}


sub moveCarUp {

    # Move the named car up in the passed grid. Care must be taken with the algoritm
    # to not move part of the car and then come across it again on the same pass and move it again 
    # so moving right requires sweeping top down.

    # We need to know which car you want to move and the reference to the grid you want to move it on
    my ($car) = shift;
    my ($grid) = shift;

    # Only vertical cards can move up
    die "Opps, tried to move a horizontal car $car up" if $cars{$car} eq "-";

    my ($row, $col);

    for $row (0 .. $#{$grid}) {
        for $col (0 .. $#{$grid->[$row]} ) {
            if ($grid->[$row][$col] eq $car) {
                die "Tried to move car $car up into an occupied spot\n" if $grid->[$row-1][$col] ne ".";
                $grid->[$row-1][$col] = $car;
                $grid->[$row][$col] = ".";
            }
        }
    }
}

sub moveCarDown {

    # Move the named car down in the passed grid. Care must be taken with the algoritm
    # to not move part of the car and then come across it again on the same pass and move it again 
    # so moving right requires sweeping upwards from the bottom.

    # We need to know which car you want to move and the reference to the grid you want to move it on
    my ($car) = shift;
    my ($grid) = shift;

    # Only vertical cards can move up
    die "Opps, tried to move a horizontal car $car down" if $cars{$car} eq "-";

    my ($row, $col);    

    for ($row = $#{$grid}; $row >=0; --$row) {
        for $col (0 .. $#{$grid->[$row]} ) {
            if ($grid->[$row][$col] eq $car) {
                die "Tried to move car $car down into an occupied spot\n" if $grid->[$row+1][$col] ne ".";
                $grid->[$row+1][$col] = $car;
                $grid->[$row][$col] = ".";
            }
        }
    }
}

sub printGrid {

    # Print out a representation of a grid

    my ($grid) = shift; # This is a reference to an array of arrays whch is passed as the argument

    my ($row, $col);

    for $row (0 .. $#{$grid}) {
        for $col (0 .. $#{$grid->[$row]} ) {
                print $grid->[$row][$col], " ";
        }
        print "\n";
    }
}