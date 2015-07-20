# http://www.thousandtyone.com/blog/EasierThanFizzBuzzWhyCantProgrammersPrint100To1.aspx
#
# Print 100 to 1.
# That's it.
# That was the question.
# The Catch?
# You need to start with "for(int i=0;" and continue from there -
#  you cannot write anything before "for(int i=0;" and
#  you can't use two loops.

for(my $i=0;

# The "shit_lord solution" was proposed by AEvar Bjarmason,
#  (https://github.com/avar) which truly made my day.
# I thought I would adapt the idea to perl, just for fun.

    shit_lord( \$i ) > 0 ; $i-- ) {

    print $i, "\n";
}

{
    my $init;

    sub shit_lord {
        my ($i_ref) = @_;

        if ( !$init ) {
            $$i_ref = 100;
            $init   = 1;
        }
        return $$i_ref;
    }
}
