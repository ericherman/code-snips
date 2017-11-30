# guess if we have color
case "$TERM" in
    linux)       have_color=yes;;
    screen)      have_color=yes;;
    xterm)       have_color=yes;;
    xterm-color) have_color=yes;;
    xterm-256color) have_color=yes;;
    ansi)        have_color=yes;;
esac

if [ "$have_color" = yes ]; then
    color_prompt=yes
fi
unset have_color

COLOR_NONE="\[\e[0m\]"
GREEN="\[\e[32;1m\]"
DARK_GREEN="\[\e[0;32m\]"
RED="\[\e[31;1m\]"
DARK_RED="\[\e[0;31m\]"
BROWN="\[\e[0;33m\]"

if [ "_${SUBNET}_" == "__" ]; then
    # laptop7.foo.example.com
    # first shift off the machine name (laptop7), then pop off the ".com", then the "example"
    SUBNET=$( perl -le 'my @parts = split "\\.", `uname -n`; shift @parts; pop @parts; pop @parts; print join(".", @parts)' )
    if [ "_${SUBNET}_" == "__" ]; then
        SUBNET="NO_SUBNET"
    fi
    export SUBNET

    if grep -qi "test" <<< "_${SUBNET}_"; then
        SUBNET_COLOR="${DARK_GREEN}"
    elif grep -qi production <<< "_${SUBNET}_"; then
        SUBNET_COLOR="${DARK_RED}"
    elif grep -qi example <<< "_${SUBNET}_"; then
        SUBNET_COLOR="${BROWN}"
    else
        SUBNET_COLOR="${DARK_GREEN}"
    fi
    export SUBNET_COLOR;
fi

function get_server_role()
{
    if [ -f /etc/SERVER_ROLE ]; then
        SERV_ROLE=" $(cat /etc/SERVER_ROLE)"
    else
        SERV_ROLE=""
    fi;


    if [ "_${SUBNET}_" != "_NO_SUBNET_" ]; then
        SERV_ROLE=" ${SUBNET}${SERV_ROLE}";
    fi

    echo "${SERV_ROLE}"

    unset SERV_ROLE

    exit $1;
}

function get_prompt_color_prefix()
{
    if [ $1 != 0 ]; then
        echo ${RED}$1;
    else
        echo ${GREEN};
    fi

    exit $1;
}

if [ "$color_prompt" == yes ]; then


    if [ $(id -u) -eq 0 ]; then
        psymb="#"
        USER_COLOR=$DARK_RED
    else
        USER_COLOR=$DARK_GREEN
        psymb="\$"
    fi
    export PROMPT_COMMAND='PS1="${USER_COLOR}\u${COLOR_NONE}@\h${SUBNET_COLOR}$(get_server_role $?)${COLOR_NONE}:\w$(get_prompt_color_prefix $?)${psymb}${COLOR_NONE} "'
else
    if [ $(id -u) -eq 0 ]; then
        PS1="[\u@\h \w]# "
    else
        PS1="[\u@\h \w]\$ "
    fi
fi
unset color_prompt
