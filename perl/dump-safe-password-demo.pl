#!/usr/bin/env perl
use 5.034;

# It's frustrating when a log accidentally contains a password
# because of a debug dump of an argument list or similar.
#
# We can avoid that by using "inside out objects" to hide the
# contents from the log, like this:

package SecretText;
use Scalar::Util qw/refaddr/;
{
	my $secrets = {};    # only accessble inside this block

	sub new {
		my ( $class, $secret ) = @_;

		my $this = bless \do { my $anon_scalar }, $class;

		$secrets->{ refaddr $this} = $secret;

		return $this;
	}

	sub reveal {
		my ($this) = @_;
		return $secrets->{ refaddr $this};
	}
}
1;

package main;
use Data::Dumper;

print "we create a SecretText object:\n",
	"\tmy \$pw = new SecretText('our little secret');\n";
my $pw = new SecretText('our little secret');

print "we can't see it in a string, \$pw: '$pw'\n";
print "we can't see it in a Dumpper( { pw => \$pw } ): \n\t",
	Dumper( { pw => $pw } );
print "yet we can retrieve it using \$pw->reveal(): '", $pw->reveal(), "'\n";

