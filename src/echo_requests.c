/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo_requests.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalvarez <aalvarez@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 12:46:48 by aalvarez          #+#    #+#             */
/*   Updated: 2024/05/02 16:53:30 by aalvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "ft_ping.h"

//TODO docu
static void display_ping_request(struct addrinfo *last_addr) {
  printf("FT_PING %s (%s) %%d(%%d) bytes of data.\n", last_addr->ai_canonname, params.ip_str);
}

//TODO docu
int icmp_echo_handler(struct addrinfo **addrs, int max_hosts) {
  ip_hdr  ip_header;

  if (configure_headers(addrs[max_hosts], &ip_header) != EXIT_SUCCESS) {
    return EXIT_FAILURE;
  }
  display_ping_request(addrs[max_hosts]);

  return EXIT_SUCCESS;
}