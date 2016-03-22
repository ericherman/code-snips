
/*
eric@titan:~/src/code-snips/c$ echo -e "#include <errno.h>\n#include <stdio.h>\n#include <string.h>\nint main()\n{" > print-errnos.c
eric@titan:~/src/code-snips/c$ cpp -dM /usr/include/errno.h | grep 'define E' | sort -n -k 3 | cut -d' ' -f2 | sed -e's/^\(E[A-Z0-9_]*\)/\tprintf("%3d: %-16s\\\"%s\\"\\n", \1, "\1", strerror(\1));/' >> print-errnos.c
eric@titan:~/src/code-snips/c$ echo -e "\n\treturn 0;\n}" >> print-errnos.c
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
int main()
{
	printf("%3d: %-16s\"%s\"\n", EDEADLOCK, "EDEADLOCK", strerror(EDEADLOCK));
	printf("%3d: %-16s\"%s\"\n", ENOTSUP, "ENOTSUP", strerror(ENOTSUP));
	printf("%3d: %-16s\"%s\"\n", EWOULDBLOCK, "EWOULDBLOCK", strerror(EWOULDBLOCK));
	printf("%3d: %-16s\"%s\"\n", EPERM, "EPERM", strerror(EPERM));
	printf("%3d: %-16s\"%s\"\n", ENOENT, "ENOENT", strerror(ENOENT));
	printf("%3d: %-16s\"%s\"\n", ESRCH, "ESRCH", strerror(ESRCH));
	printf("%3d: %-16s\"%s\"\n", EINTR, "EINTR", strerror(EINTR));
	printf("%3d: %-16s\"%s\"\n", EIO, "EIO", strerror(EIO));
	printf("%3d: %-16s\"%s\"\n", ENXIO, "ENXIO", strerror(ENXIO));
	printf("%3d: %-16s\"%s\"\n", E2BIG, "E2BIG", strerror(E2BIG));
	printf("%3d: %-16s\"%s\"\n", ENOEXEC, "ENOEXEC", strerror(ENOEXEC));
	printf("%3d: %-16s\"%s\"\n", EBADF, "EBADF", strerror(EBADF));
	printf("%3d: %-16s\"%s\"\n", ECHILD, "ECHILD", strerror(ECHILD));
	printf("%3d: %-16s\"%s\"\n", EAGAIN, "EAGAIN", strerror(EAGAIN));
	printf("%3d: %-16s\"%s\"\n", ENOMEM, "ENOMEM", strerror(ENOMEM));
	printf("%3d: %-16s\"%s\"\n", EACCES, "EACCES", strerror(EACCES));
	printf("%3d: %-16s\"%s\"\n", EFAULT, "EFAULT", strerror(EFAULT));
	printf("%3d: %-16s\"%s\"\n", ENOTBLK, "ENOTBLK", strerror(ENOTBLK));
	printf("%3d: %-16s\"%s\"\n", EBUSY, "EBUSY", strerror(EBUSY));
	printf("%3d: %-16s\"%s\"\n", EEXIST, "EEXIST", strerror(EEXIST));
	printf("%3d: %-16s\"%s\"\n", EXDEV, "EXDEV", strerror(EXDEV));
	printf("%3d: %-16s\"%s\"\n", ENODEV, "ENODEV", strerror(ENODEV));
	printf("%3d: %-16s\"%s\"\n", ENOTDIR, "ENOTDIR", strerror(ENOTDIR));
	printf("%3d: %-16s\"%s\"\n", EISDIR, "EISDIR", strerror(EISDIR));
	printf("%3d: %-16s\"%s\"\n", EINVAL, "EINVAL", strerror(EINVAL));
	printf("%3d: %-16s\"%s\"\n", ENFILE, "ENFILE", strerror(ENFILE));
	printf("%3d: %-16s\"%s\"\n", EMFILE, "EMFILE", strerror(EMFILE));
	printf("%3d: %-16s\"%s\"\n", ENOTTY, "ENOTTY", strerror(ENOTTY));
	printf("%3d: %-16s\"%s\"\n", ETXTBSY, "ETXTBSY", strerror(ETXTBSY));
	printf("%3d: %-16s\"%s\"\n", EFBIG, "EFBIG", strerror(EFBIG));
	printf("%3d: %-16s\"%s\"\n", ENOSPC, "ENOSPC", strerror(ENOSPC));
	printf("%3d: %-16s\"%s\"\n", ESPIPE, "ESPIPE", strerror(ESPIPE));
	printf("%3d: %-16s\"%s\"\n", EROFS, "EROFS", strerror(EROFS));
	printf("%3d: %-16s\"%s\"\n", EMLINK, "EMLINK", strerror(EMLINK));
	printf("%3d: %-16s\"%s\"\n", EPIPE, "EPIPE", strerror(EPIPE));
	printf("%3d: %-16s\"%s\"\n", EDOM, "EDOM", strerror(EDOM));
	printf("%3d: %-16s\"%s\"\n", ERANGE, "ERANGE", strerror(ERANGE));
	printf("%3d: %-16s\"%s\"\n", EDEADLK, "EDEADLK", strerror(EDEADLK));
	printf("%3d: %-16s\"%s\"\n", ENAMETOOLONG, "ENAMETOOLONG", strerror(ENAMETOOLONG));
	printf("%3d: %-16s\"%s\"\n", ENOLCK, "ENOLCK", strerror(ENOLCK));
	printf("%3d: %-16s\"%s\"\n", ENOSYS, "ENOSYS", strerror(ENOSYS));
	printf("%3d: %-16s\"%s\"\n", ENOTEMPTY, "ENOTEMPTY", strerror(ENOTEMPTY));
	printf("%3d: %-16s\"%s\"\n", ELOOP, "ELOOP", strerror(ELOOP));
	printf("%3d: %-16s\"%s\"\n", ENOMSG, "ENOMSG", strerror(ENOMSG));
	printf("%3d: %-16s\"%s\"\n", EIDRM, "EIDRM", strerror(EIDRM));
	printf("%3d: %-16s\"%s\"\n", ECHRNG, "ECHRNG", strerror(ECHRNG));
	printf("%3d: %-16s\"%s\"\n", EL2NSYNC, "EL2NSYNC", strerror(EL2NSYNC));
	printf("%3d: %-16s\"%s\"\n", EL3HLT, "EL3HLT", strerror(EL3HLT));
	printf("%3d: %-16s\"%s\"\n", EL3RST, "EL3RST", strerror(EL3RST));
	printf("%3d: %-16s\"%s\"\n", ELNRNG, "ELNRNG", strerror(ELNRNG));
	printf("%3d: %-16s\"%s\"\n", EUNATCH, "EUNATCH", strerror(EUNATCH));
	printf("%3d: %-16s\"%s\"\n", ENOCSI, "ENOCSI", strerror(ENOCSI));
	printf("%3d: %-16s\"%s\"\n", EL2HLT, "EL2HLT", strerror(EL2HLT));
	printf("%3d: %-16s\"%s\"\n", EBADE, "EBADE", strerror(EBADE));
	printf("%3d: %-16s\"%s\"\n", EBADR, "EBADR", strerror(EBADR));
	printf("%3d: %-16s\"%s\"\n", EXFULL, "EXFULL", strerror(EXFULL));
	printf("%3d: %-16s\"%s\"\n", ENOANO, "ENOANO", strerror(ENOANO));
	printf("%3d: %-16s\"%s\"\n", EBADRQC, "EBADRQC", strerror(EBADRQC));
	printf("%3d: %-16s\"%s\"\n", EBADSLT, "EBADSLT", strerror(EBADSLT));
	printf("%3d: %-16s\"%s\"\n", EBFONT, "EBFONT", strerror(EBFONT));
	printf("%3d: %-16s\"%s\"\n", ENOSTR, "ENOSTR", strerror(ENOSTR));
	printf("%3d: %-16s\"%s\"\n", ENODATA, "ENODATA", strerror(ENODATA));
	printf("%3d: %-16s\"%s\"\n", ETIME, "ETIME", strerror(ETIME));
	printf("%3d: %-16s\"%s\"\n", ENOSR, "ENOSR", strerror(ENOSR));
	printf("%3d: %-16s\"%s\"\n", ENONET, "ENONET", strerror(ENONET));
	printf("%3d: %-16s\"%s\"\n", ENOPKG, "ENOPKG", strerror(ENOPKG));
	printf("%3d: %-16s\"%s\"\n", EREMOTE, "EREMOTE", strerror(EREMOTE));
	printf("%3d: %-16s\"%s\"\n", ENOLINK, "ENOLINK", strerror(ENOLINK));
	printf("%3d: %-16s\"%s\"\n", EADV, "EADV", strerror(EADV));
	printf("%3d: %-16s\"%s\"\n", ESRMNT, "ESRMNT", strerror(ESRMNT));
	printf("%3d: %-16s\"%s\"\n", ECOMM, "ECOMM", strerror(ECOMM));
	printf("%3d: %-16s\"%s\"\n", EPROTO, "EPROTO", strerror(EPROTO));
	printf("%3d: %-16s\"%s\"\n", EMULTIHOP, "EMULTIHOP", strerror(EMULTIHOP));
	printf("%3d: %-16s\"%s\"\n", EDOTDOT, "EDOTDOT", strerror(EDOTDOT));
	printf("%3d: %-16s\"%s\"\n", EBADMSG, "EBADMSG", strerror(EBADMSG));
	printf("%3d: %-16s\"%s\"\n", EOVERFLOW, "EOVERFLOW", strerror(EOVERFLOW));
	printf("%3d: %-16s\"%s\"\n", ENOTUNIQ, "ENOTUNIQ", strerror(ENOTUNIQ));
	printf("%3d: %-16s\"%s\"\n", EBADFD, "EBADFD", strerror(EBADFD));
	printf("%3d: %-16s\"%s\"\n", EREMCHG, "EREMCHG", strerror(EREMCHG));
	printf("%3d: %-16s\"%s\"\n", ELIBACC, "ELIBACC", strerror(ELIBACC));
	printf("%3d: %-16s\"%s\"\n", ELIBBAD, "ELIBBAD", strerror(ELIBBAD));
	printf("%3d: %-16s\"%s\"\n", ELIBSCN, "ELIBSCN", strerror(ELIBSCN));
	printf("%3d: %-16s\"%s\"\n", ELIBMAX, "ELIBMAX", strerror(ELIBMAX));
	printf("%3d: %-16s\"%s\"\n", ELIBEXEC, "ELIBEXEC", strerror(ELIBEXEC));
	printf("%3d: %-16s\"%s\"\n", EILSEQ, "EILSEQ", strerror(EILSEQ));
	printf("%3d: %-16s\"%s\"\n", ERESTART, "ERESTART", strerror(ERESTART));
	printf("%3d: %-16s\"%s\"\n", ESTRPIPE, "ESTRPIPE", strerror(ESTRPIPE));
	printf("%3d: %-16s\"%s\"\n", EUSERS, "EUSERS", strerror(EUSERS));
	printf("%3d: %-16s\"%s\"\n", ENOTSOCK, "ENOTSOCK", strerror(ENOTSOCK));
	printf("%3d: %-16s\"%s\"\n", EDESTADDRREQ, "EDESTADDRREQ", strerror(EDESTADDRREQ));
	printf("%3d: %-16s\"%s\"\n", EMSGSIZE, "EMSGSIZE", strerror(EMSGSIZE));
	printf("%3d: %-16s\"%s\"\n", EPROTOTYPE, "EPROTOTYPE", strerror(EPROTOTYPE));
	printf("%3d: %-16s\"%s\"\n", ENOPROTOOPT, "ENOPROTOOPT", strerror(ENOPROTOOPT));
	printf("%3d: %-16s\"%s\"\n", EPROTONOSUPPORT, "EPROTONOSUPPORT", strerror(EPROTONOSUPPORT));
	printf("%3d: %-16s\"%s\"\n", ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT", strerror(ESOCKTNOSUPPORT));
	printf("%3d: %-16s\"%s\"\n", EOPNOTSUPP, "EOPNOTSUPP", strerror(EOPNOTSUPP));
	printf("%3d: %-16s\"%s\"\n", EPFNOSUPPORT, "EPFNOSUPPORT", strerror(EPFNOSUPPORT));
	printf("%3d: %-16s\"%s\"\n", EAFNOSUPPORT, "EAFNOSUPPORT", strerror(EAFNOSUPPORT));
	printf("%3d: %-16s\"%s\"\n", EADDRINUSE, "EADDRINUSE", strerror(EADDRINUSE));
	printf("%3d: %-16s\"%s\"\n", EADDRNOTAVAIL, "EADDRNOTAVAIL", strerror(EADDRNOTAVAIL));
	printf("%3d: %-16s\"%s\"\n", ENETDOWN, "ENETDOWN", strerror(ENETDOWN));
	printf("%3d: %-16s\"%s\"\n", ENETUNREACH, "ENETUNREACH", strerror(ENETUNREACH));
	printf("%3d: %-16s\"%s\"\n", ENETRESET, "ENETRESET", strerror(ENETRESET));
	printf("%3d: %-16s\"%s\"\n", ECONNABORTED, "ECONNABORTED", strerror(ECONNABORTED));
	printf("%3d: %-16s\"%s\"\n", ECONNRESET, "ECONNRESET", strerror(ECONNRESET));
	printf("%3d: %-16s\"%s\"\n", ENOBUFS, "ENOBUFS", strerror(ENOBUFS));
	printf("%3d: %-16s\"%s\"\n", EISCONN, "EISCONN", strerror(EISCONN));
	printf("%3d: %-16s\"%s\"\n", ENOTCONN, "ENOTCONN", strerror(ENOTCONN));
	printf("%3d: %-16s\"%s\"\n", ESHUTDOWN, "ESHUTDOWN", strerror(ESHUTDOWN));
	printf("%3d: %-16s\"%s\"\n", ETOOMANYREFS, "ETOOMANYREFS", strerror(ETOOMANYREFS));
	printf("%3d: %-16s\"%s\"\n", ETIMEDOUT, "ETIMEDOUT", strerror(ETIMEDOUT));
	printf("%3d: %-16s\"%s\"\n", ECONNREFUSED, "ECONNREFUSED", strerror(ECONNREFUSED));
	printf("%3d: %-16s\"%s\"\n", EHOSTDOWN, "EHOSTDOWN", strerror(EHOSTDOWN));
	printf("%3d: %-16s\"%s\"\n", EHOSTUNREACH, "EHOSTUNREACH", strerror(EHOSTUNREACH));
	printf("%3d: %-16s\"%s\"\n", EALREADY, "EALREADY", strerror(EALREADY));
	printf("%3d: %-16s\"%s\"\n", EINPROGRESS, "EINPROGRESS", strerror(EINPROGRESS));
	printf("%3d: %-16s\"%s\"\n", ESTALE, "ESTALE", strerror(ESTALE));
	printf("%3d: %-16s\"%s\"\n", EUCLEAN, "EUCLEAN", strerror(EUCLEAN));
	printf("%3d: %-16s\"%s\"\n", ENOTNAM, "ENOTNAM", strerror(ENOTNAM));
	printf("%3d: %-16s\"%s\"\n", ENAVAIL, "ENAVAIL", strerror(ENAVAIL));
	printf("%3d: %-16s\"%s\"\n", EISNAM, "EISNAM", strerror(EISNAM));
	printf("%3d: %-16s\"%s\"\n", EREMOTEIO, "EREMOTEIO", strerror(EREMOTEIO));
	printf("%3d: %-16s\"%s\"\n", EDQUOT, "EDQUOT", strerror(EDQUOT));
	printf("%3d: %-16s\"%s\"\n", ENOMEDIUM, "ENOMEDIUM", strerror(ENOMEDIUM));
	printf("%3d: %-16s\"%s\"\n", EMEDIUMTYPE, "EMEDIUMTYPE", strerror(EMEDIUMTYPE));
	printf("%3d: %-16s\"%s\"\n", ECANCELED, "ECANCELED", strerror(ECANCELED));
	printf("%3d: %-16s\"%s\"\n", ENOKEY, "ENOKEY", strerror(ENOKEY));
	printf("%3d: %-16s\"%s\"\n", EKEYEXPIRED, "EKEYEXPIRED", strerror(EKEYEXPIRED));
	printf("%3d: %-16s\"%s\"\n", EKEYREVOKED, "EKEYREVOKED", strerror(EKEYREVOKED));
	printf("%3d: %-16s\"%s\"\n", EKEYREJECTED, "EKEYREJECTED", strerror(EKEYREJECTED));
	printf("%3d: %-16s\"%s\"\n", EOWNERDEAD, "EOWNERDEAD", strerror(EOWNERDEAD));
	printf("%3d: %-16s\"%s\"\n", ENOTRECOVERABLE, "ENOTRECOVERABLE", strerror(ENOTRECOVERABLE));
	printf("%3d: %-16s\"%s\"\n", ERFKILL, "ERFKILL", strerror(ERFKILL));
	printf("%3d: %-16s\"%s\"\n", EHWPOISON, "EHWPOISON", strerror(EHWPOISON));

	return 0;
}
