/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalvarez <aalvarez@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/11 10:32:28 by aalvarez          #+#    #+#             */
/*   Updated: 2024/04/23 13:39:57 by aalvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "ft_ping.h"

char HELP_MSG_BUFF[] = {
    "\nUsage\n"
    "  ft_ping [options] <destination>\n"
    "\nOptions:\n"
    "  <destination>      dns name or ip address\n"
    "  -h                 print help and exit\n"
    "  -?                 behaves like \"-h\"\n"
    "\nFor more details see README."
};

const error_params errors[] = {
    {DEST_REQUIRED, "ft_ping: usage error: Destination address required"},
    {HELP_MSG, HELP_MSG_BUFF},
};

void print_err_msg(int errnum) {
  fprintf(stderr, "%s\n", errors[errnum].err_msg);
}

void fatal_error(const char *failed, const char *msg) {
  free(params.dst_addrs);
  params.dst_addrs = NULL;
  fprintf(stderr, "ft_ping: ");
  if (msg == NULL) {
    perror(failed);
  } else {
    fprintf(stderr, "%s: %s\n", failed, msg);
  }
}
