/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cleanup.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalvarez <aalvarez@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 09:25:13 by aalvarez          #+#    #+#             */
/*   Updated: 2024/04/24 09:36:28 by aalvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "ft_ping.h"

/**
 * Closes all the sockets mapped to the different destination addresses.
 * @param max_hosts the number of sockets to be closed.
 * @return EXIT_SUCCESS if all the sockets were correctly closed,
 * EXIT_FAILURE if any call to close() (man close()) failed.
 */
static int close_sockets(int max_hosts) {
  while (--max_hosts >= 0) {
    if (close(params.sockets[max_hosts]) != 0) {
      fatal_error("close", NULL);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * Frees all the memory allocated by the getaddrinfo() (man getaddrinfo(3))
 * function.
 * @param addrs the addrinfo structs to be freed.
 * @param max_hosts the number of addrinfo structs to be freed.
 */
static void  clean_addrs(struct addrinfo **addrs, int max_hosts) {
  while (--max_hosts >= 0) {
    freeaddrinfo(addrs[max_hosts]);
  }
}

/**
 * Wrapper that makes calls to close all the on-use sockets and frees
 * all the memory allocated by getaddrinfo() (man getaddrinfo(3)).
 * @param addrs the addrinfo structs to be freed.
 * @param max_hosts the number of sockets and addrinfo structs to be
 * handled.
 * @return EXIT_SUCCESS if all the cleanup process was successful,
 * EXIT_FAILURE if any socket failed to close.
 */
int  clean_program(struct addrinfo **addrs, int max_hosts) {
  clean_addrs(addrs, max_hosts);
  return close_sockets(max_hosts);
}
