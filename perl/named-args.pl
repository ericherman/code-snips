#!/usr/bin/perl

# this demonstrates two basic ways of doing named arguments in perl
# it's just a matter of style, but I prefer a hash-ref to a simple hash

use strict;
use warnings;

use Data::Dumper;

# this function takes a hash reference of named aguments
sub href_funk {
	my ($args) = @_;
	print 'print Dumper({ args => $args } );', "\n";
	print Dumper( { args => $args } );
}

# this function takes a hash (not a refernce) of named aguments
sub hash_funk {
	my %args = @_;
	print 'print Dumper({ args => \%args } );', "\n";
	print Dumper( { args => \%args } );
}

print "Calling: href_funk( { a => 'foo', b => 2 } )\n";
href_funk( { a => 'foo', b => 2 } );

print "hash_funk( a => 'foo', b => 2 )\n";
hash_funk( a => 'foo', b => 2 );

if (0) {    # nothing prevents programmer error
	print "\n";

	print "Calling href_funk( { a => 'bar', b => [0..1] } )\n";
	href_funk( { a => 'bar', b => [ 0 .. 1 ] } );

	print "calling href_funk( { a => 'bar', b => 0..1 } )\n";
	href_funk( { a => 'bar', b => 0 .. 1 } );

	print "hash_funk( a => 'bar', b => [0..1] );\n";
	hash_funk( a => 'bar', b => [ 0 .. 1 ] );

	print "hash_funk( a => 'bar', b => 0..1 );\n";
	hash_funk( a => 'bar', b => 0 .. 1 );
}
