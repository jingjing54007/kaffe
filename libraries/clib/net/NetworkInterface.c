/*
 * NetworkInterface.c
 * Native implementation of java.net.NetworkInterface functions.
 *
 * Copyright (c) 2002 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "config-net.h"
#include "config-io.h"
#include "config-hacks.h"
#include <native.h>
#include "java_net_NetworkInterface.h"
#include "java_util_Vector.h"
#include "nets.h"
#include <arpa/inet.h>
#include <jsyscall.h>
#include "../../../kaffe/kaffevm/debug.h"

#include <ifaddrs.h>

static struct ifaddrs*
detectInterfaces(void)
{
  struct Hkaffe_util_Ptr *retval = NULL;
  struct ifaddrs *ifa;
  errorInfo einfo;
  
  if( getifaddrs(&ifa) == 0 )
    {
      retval = (struct Hkaffe_util_Ptr *)ifa;
    }
  else
    {
      switch( errno )
	{
	case ENOMEM:
	  postOutOfMemory(&einfo);
	  break;
	case ENOSYS:
	  postExceptionMessage(
			       &einfo,
			       "kaffe.util.NotImplemented",
			       "OS doesn't support getifaddrs()");
	  break;
	default:
	  postExceptionMessage(
			       &einfo,
			       "java.net.SocketException",
			       "%s",
			       SYS_ERROR(errno));
	  break;
	}
      throwError(&einfo);
    }
  return( ifa );
}

static void
freeInterfaces(struct ifaddrs* jifa)
{
  if( jifa )
    {
      freeifaddrs(jifa);
    }
}

static struct Hjava_net_InetAddress *
getInetAddress(struct ifaddrs *ifa)
{
  struct Hjava_lang_String *address_string = NULL;
  struct Hjava_net_InetAddress *retval = NULL;

  if( ifa )
    {
      struct sockaddr *sa;
      
      if( (sa = ifa->ifa_addr) )
	{
#define NII_MAX_ADDRESS_SIZE 128
	  char addr[NII_MAX_ADDRESS_SIZE];
	  
	  switch( sa->sa_family )
	    {
	    case AF_INET:
	      inet_ntop(sa->sa_family,
			&((struct sockaddr_in *)sa)->sin_addr,
			addr,
			NII_MAX_ADDRESS_SIZE);
	      address_string = stringC2Java(addr);
	      break;
#if defined(AF_INET6)
	    case AF_INET6:
	      inet_ntop(sa->sa_family,
			&((struct sockaddr_in6 *)sa)->sin6_addr,
			addr,
			NII_MAX_ADDRESS_SIZE);
	      address_string = stringC2Java(addr);
#endif
	    default:
	      /* XXX What to do? */
	      break;
	    }
	}
    }
  retval = (struct Hjava_net_InetAddress *)do_execute_java_class_method
    ("java/net/InetAddress", 0, "getByName", "(Ljava/lang/String;)Ljava/net/InetAddress;",
     address_string).l;
  return( retval );
}

struct Hjava_util_Vector*
java_net_NetworkInterface_getRealNetworkInterfaces(void)
{
  struct Hjava_util_Vector* vector;
  struct ifaddrs* addrs;
  struct ifaddrs* ifa; 

  vector =
    (struct Hjava_util_Vector*)execute_java_constructor("java/util/Vector", 0, 0, "()V");
  
  ifa = addrs = detectInterfaces();
  while (ifa != NULL)
    {
      struct Hjava_lang_String* iface_name =
	stringC2Java(ifa->ifa_name);
      struct Hjava_net_InetAddress* addr;
      
      addr = getInetAddress(ifa);

      if (addr != NULL)
	do_execute_java_method
	  (vector, "add", "(Ljava/lang/Object;)Z", 0, 0,
	   execute_java_constructor("java/net/NetworkInterface", 0, 0,
		   "(Ljava/lang/String;Ljava/net/InetAddress;)V",
		    iface_name, addr));
      
      ifa = ifa->ifa_next;
    }

  freeInterfaces(addrs);
  return vector;
}
