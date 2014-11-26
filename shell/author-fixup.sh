git filter-branch -f --commit-filter '
if [ "$GIT_COMMITTER_EMAIL" = "eric.herman@booking.com" ];
then
GIT_COMMITTER_NAME="Eric Herman";
GIT_AUTHOR_NAME="Eric Herman";
GIT_COMMITTER_EMAIL="eric@freesa.org";
GIT_AUTHOR_EMAIL="eric@freesa.org";
git commit-tree "$@";
else
git commit-tree "$@";
fi' HEAD
