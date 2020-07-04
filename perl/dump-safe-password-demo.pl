#!/usr/bin/env perl
package SecretText;
use strict;
use warnings;
use Scalar::Util qw/refaddr/;

# It's frustrating when a log accidentally contains a password
# because of a debug dump of an argument list or similr.
#
# We can avoid that by using "inside out objects" to hide the
# contents from the log, like this:

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

use Data::Dumper;

my $pw = new SecretText("our little secret");

print "we can't see it in a string: '$pw'\n";
print "we can't see it in a Dump: \n\t", Dumper( { pw => $pw } );
print "yet we can reveal it: '", $pw->reveal, "'\n";
