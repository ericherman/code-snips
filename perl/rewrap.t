#!/usr/bin/env perl

use 5.034;

# the module is in the same directory as the test
# thus strip the extension to get the module path:
(my $module = $0) =~ s/\.[^.]+$//;

do($module) or die $@ || $!;

use Test::More tests => 4;

sub check_rewrap {
	my ( $in, $expect, $wrap, $msg ) = @_;

	$msg ||= "rewrap col $wrap";

	open my $fhin, "<", \$in;
	my $out = "";
	open my $fhout, ">", \$out;

	rewrap( $fhin, $fhout, $wrap );
	is( $out, $expect, $msg );
}

my $in = <<EOF;
   abcdef
   ghijkl
EOF

my $expected_6 = <<EOF;
   abc
   def
   ghi
   jkl
EOF
check_rewrap( $in, $expected_6, 6 );

my $expected_8 = <<EOF;
   abcde
   fghij
   kl
EOF
check_rewrap( $in, $expected_8, 8 );

my $expected_10 = <<EOF;
   abcdefg
   hijkl
EOF
check_rewrap( $in, $expected_10, 10 );

my $in_break = <<EOF;
   abcdef
   ghijkl

   mnopqr
   stuvwx
EOF
my $expected_8_break = <<EOF;
   abcde
   fghij
   kl

   mnopq
   rstuv
   wx
EOF
check_rewrap( $in_break, $expected_8_break, 8, "with linebreak" );

done_testing();
