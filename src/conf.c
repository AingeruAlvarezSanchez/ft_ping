/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   conf.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aalvarez <aalvarez@student.42urduliz.com>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/23 09:12:23 by aalvarez          #+#    #+#             */
/*   Updated: 2024/05/02 16:56:34 by aalvarez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include "ft_ping.h"

/**
 * Configures a set of addrinfo linked lists resultant from the calls to
 * getaddrinfo() (man getaddrinfo(3)) based on the number of destination
 * addresses specified by the user on program call.
 * @param addrs the addrinfo structs to be filled.
 * @param max_hosts the number of hosts.
 * @param hosts_addrs the destination hosts specified by the user.
 * @param domain the type of protocol domain, default is AF_INET.
 * @return EXIT_SUCCESS if all the calls to getaddrinfo() (man getaddrinfo(3))
 * were successful, EXIT_FAILURE if any call to the getaddrinfo()
 * (man socket(3)) function failed.
 */
static int configure_addr_info(struct addrinfo **addrs, int max_hosts, char **hosts_addrs, int domain) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));

  hints.ai_socktype = SOCK_RAW;
  hints.ai_flags |= AI_CANONNAME;
  if (domain == AF_INET) {
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_ICMP;
  } else {
    hints.ai_family = AF_INET6;
    hints.ai_protocol = IPPROTO_ICMPV6;
  }

  int status;
  for (int i = optind; i < max_hosts; i++) {
    if ((status = getaddrinfo(hosts_addrs[i], NULL, &hints, &addrs[i - optind])) != 0) {
      fatal_error(hosts_addrs[i], gai_strerror(status));
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

//TODO docu
static int configure_packet_info(struct addrinfo *last_addr) {
  params.payload_size = DEFAULT_PAYLOAD_SIZE;
  void *last_host_addr;
  char last_host_str[last_addr->ai_family == AF_INET ?
      INET_ADDRSTRLEN :
      INET6_ADDRSTRLEN];

  if (last_addr->ai_family == AF_INET) {
    last_host_addr = &(((struct sockaddr_in *)last_addr->ai_addr)->sin_addr);
  } else {
    last_host_addr = &(((struct sockaddr_in6 *)last_addr->ai_addr)->sin6_addr);
  }

  if (inet_ntop(last_addr->ai_family, last_host_addr, last_host_str, sizeof(last_host_str)) == NULL) {
    fatal_error("inet_ntop", NULL);
    return EXIT_FAILURE;
  }
  params.ip_str = last_host_str;

  return EXIT_SUCCESS;
}

/**
 * Creates and configures a socket for each specified host.
 * @param max_hosts the number of hosts to create a socket from.
 * @param addrs the addrinfo structs to use as configuration for
 * the sockets.
 * @return EXIT_SUCCESS if every socket was successfully created,
 * EXIT_FAILURE if any call to the socket() (man socket(2)) function failed.
 */
static int configure_sockets(int max_hosts, struct addrinfo **addrs) {
  while (--max_hosts >= 0) {
    if ((params.sockets[max_hosts] = socket(addrs[max_hosts]->ai_family,
                                      addrs[max_hosts]->ai_socktype,
                                      addrs[max_hosts]->ai_protocol)) == -1) {
      fatal_error("socket", NULL);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * Checks if the protocol will be configured to IPV4 or IPV6 and tries
 * to create all sockets configured with the corresponding configuration.
 * @param addrs the addrinfo structs to be configured and used as socket
 * configuration.
 * @param max_hosts the number of destination hosts.
 * @param hosts_addrs the destination hosts specified by the user.
 * @return EXIT_SUCCESS if all the addrinfo structs were successfully created and all
 * sockets were equally successfully created, EXIT_FAILURE if any of the configuration
 * functions failed, OTHER_ERROR if both IPV6 and IPV4 flags were set to true.
 */
int configure_icmp(struct addrinfo **addrs, int max_hosts, char **hosts_addrs) {
  // Tests if the flags in the bitset are set to true, it moves the IPv6 flag
  // one bit to the left, then uses a logical "or" comparison to check which bits were
  // set, the possible combinations would be ("0000", "0001", "0010", "0011"), corresponding
  // to the binary representation of the switch cases, being "0011" the default case.
  switch (IS_SET_FLAG(IPV6_FLAG) << 1 | IS_SET_FLAG(IPV4_FLAG)) {
    case 0:
    case 1:
      if (configure_addr_info(addrs, max_hosts + optind, hosts_addrs, AF_INET) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
      }
      break;
    case 2:
      if (configure_addr_info(addrs, max_hosts + optind, hosts_addrs, AF_INET6)) {
        return EXIT_FAILURE;
      }
      break;
    default:
      print_err_msg(ONLY_ONE_IP_TYPE);
      return OTHER_ERROR;
  }

  return configure_sockets(max_hosts, addrs) && configure_packet_info(addrs[max_hosts - 1]);
}

//TODO documentation
uint16_t calculate_checksum(uint16_t *data, int length) {
  uint32_t sum = 0;

  while (length > 1) {
    sum += *data++;
    length -= 2;
  }
  if (length > 0) {
    sum += *((uint8_t *)data);
  }
  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }
  return (uint16_t)(~sum);
}

/**
 * Configures the fields of the ICMP request header for version 4 (IPv4).
 * @param last_addr the address used as reference to create the configuration.
 * @param ip_header the IPv4 header struct.
 * @return EXIT_SUCCESS if the configuration was successful, EXIT_FAILURE if the
 * internal call to getifaddrs (man getifaddrs(3)) failed.
 */
static int configure_ipv4_header(struct addrinfo *last_addr, ip_hdr *ip_header) {
  char packet[IPV4_HEADER_SIZE];
  struct ifaddrs *if_addr = {}, *cpy;

  if (getifaddrs(&if_addr) != 0) {
    fatal_error("getifaddrs", NULL);
    return EXIT_FAILURE;
  }

  struct sockaddr_in source_addr = {};
  for (cpy = if_addr; cpy != NULL; cpy = cpy->ifa_next) {
    if (cpy->ifa_addr != NULL && cpy->ifa_addr->sa_family == AF_INET) {
      struct sockaddr_in *addr = (struct sockaddr_in *)cpy->ifa_addr;
      source_addr.sin_addr = addr->sin_addr;
      break;
    }
  }
  freeifaddrs(if_addr);

  ip_header->ip4 = (struct iphdr *)packet;
  ip_header->ip4->ihl = 5;
  ip_header->ip4->version = 4;
  ip_header->ip4->protocol = IPPROTO_ICMP;
  ip_header->ip4->tos = 0;
  ip_header->ip4->frag_off = 0;
  ip_header->ip4->tot_len = IPV4_HEADER_SIZE + params.payload_size;

  // The default Time To Live (TTL) varies depending on the system,
  // this implementation uses a TTL of 64 hops by default.
  ip_header->ip4->ttl = 64;
  ip_header->ip4->saddr = source_addr.sin_addr.s_addr;
  ip_header->ip4->daddr = ((struct sockaddr_in *)(last_addr->ai_addr))->sin_addr.s_addr;
  ip_header->ip4->id = htons(getpid());
  ip_header->ip4->check = calculate_checksum((uint16_t *)ip_header->ip4, sizeof(struct iphdr));

  return EXIT_SUCCESS;
}

//TODO documentation
static int configure_ipv6_header(__attribute__((unused)) struct addrinfo *last_addr, __attribute__((unused)) ip_hdr *ip_header) {
  return EXIT_SUCCESS;
}

/**
 * Configures a struct representing a header of IPv4 or IPv6 type, defined by the user,
 * IPv4 by default.
 * @param last_addr the address used as reference to create the configuration.
 * @param ip_header a union containing a structure for each IP version that will be
 * dynamically configured.
 * @return EXIT_SUCCESS if the configuration was successful, EXIT_FAILURE otherwise.
 */
int configure_headers(struct addrinfo *last_addr, ip_hdr *ip_header) {
  if ((last_addr->ai_family == AF_INET && configure_ipv4_header(last_addr, ip_header) != EXIT_SUCCESS)
        || (last_addr->ai_family == AF_INET6 && configure_ipv6_header(last_addr, ip_header) != EXIT_SUCCESS)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
