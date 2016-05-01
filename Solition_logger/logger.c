#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <errno.h>
#include  <string.h>
#include  <sys/types.h>

#ifdef __linux__
#include  <sys/socket.h>
#include  <netinet/in.h>
#include  <arpa/inet.h>
#include  <netdb.h>
#else
#define _WIN32_WINNT 0x501
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#define MYPORT "48879"
#define MAXBUFLEN 100

enum
  {
    LOG_MSTICS = 1,
    LOG_MINTICS,
    LOG_AUXV,
    LOG_PACKV,
    LOG_CURRENT,
    LOG_TEMP,
    LOG_INPUT3,
    LOG_INPUT2,
    LOG_INPUT1,
    LOG_THROTTLE,
    LOG_CPULOAD,
    LOG_PWM,
    LOG_RPM,
    LOG_RPMERROR,
    LOG_MODE,
    LOG_NUMFIELDS
  };

#define LIMIT_HIMOTORV	0x8000
#define LIMIT_HIMOTORP	0x4000
#define LIMIT_THROTTLE	0x2000
#define LIMIT_OVERREV	0x1000

#define LIMIT_LOWPACKV	0x0800
#define LIMIT_HIPACKC	0x0400
#define LIMIT_HVC	0x0200
#define LIMIT_CTEMP	0x0100

#define LIMIT_SLEWRATE	0x0080
#define LIMIT_BLOCKED	0x0040
#define LIMIT_BRAKES   	0x0020

char *mode[32] =
  {
    "Starting up",
    "Precharge phase",
    "Engaging contactors",
    "Waiting for startsignal",
    "Throttle not in zero pos",
    "Running",
    "Error 6","Error 7","Error 8","Error 9","Error 10","Error 11","Error 12",
    "Error 13", "Error 14","Error 15","Error 16","Error 17",
    "Error: Motor overcurrent",
    "Error: ADC out of range",
    "Error: Current sensor failure",
    "Error: Zero voltage after precharge",
    "Error: Pack voltage too low after precharge",
    "Error: Faulty throttle signal",
    "Error: 12 Volt too high",
    "Error: 12 Volt too low",
    "Error: Pack voltage too high",
    "Error: Pack voltage too low",
    "Error: IGBT desaturation",
    "Error: Out of memory",
    "Error: Software error",
    "Controller shut down by user"
  };

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int numbytes;
  char buf[MAXBUFLEN];
  uint16_t *debug = (uint16_t *)buf;
  FILE *log = fopen ("logger.txt", "w");
  if (!log)
    {
      perror ("logger.txt");
      return EXIT_FAILURE;
    }
  
  fprintf (log, "Time\tCPU\tThrtl\tMotorC\tPWM\tPackV\tTemp\tRPM\t"
	   "In1\tIn2\tIn3\t12V\tMode\r\n");
  
#ifndef __linux__
  WSADATA wsaData;
  if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0) {
    fprintf(stderr, "WSAStartup failed.\n");
    exit(1);
  }
#endif
  
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;
  if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }
  
  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
			 p->ai_protocol)) == -1) {
      perror("listener: socket");
      continue;
    }
    
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("listener: bind");
      continue;
    }
    break;
  }
  
  if (p == NULL) {
    fprintf(stderr, "listener: failed to bind socket\n");
    return 2;
  }
  
  freeaddrinfo(servinfo);
  uint16_t oldmstic = 0xffff;
  while (1)
    {
      numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0, NULL, NULL);
      if (numbytes == -1) {
	perror("recvfrom");
	exit(1);
      }
      
      if (numbytes && !buf[0])
	{
	  if (oldmstic != debug[LOG_MSTICS] / 200)
	    {
	      oldmstic = debug[LOG_MSTICS] / 200;
	      printf ("\r%2.1d:%2.2d:%2.2d - CPU:%3.1d%% Thr:%3.1dA "
		      "I:%3.1dA D:%3.1d.%1.1d Pack:%3.1dV "
		      "T:%3.1d.%1.1d RPM:%4.1d ",
		      debug[LOG_MINTICS] / 60,
		      debug[LOG_MINTICS] % 60,
		      debug[LOG_MSTICS] / 1000,
		      debug[LOG_CPULOAD] >> 7,
		      debug[LOG_THROTTLE],
		      debug[LOG_CURRENT],
		      debug[LOG_PWM] / 10,
		      debug[LOG_PWM] % 10,
		      debug[LOG_PACKV],
		      ((int16_t) debug[LOG_TEMP])/10,
		      abs((int16_t) debug[LOG_TEMP])%10,
		      debug[LOG_RPM]);
	      
	      fflush (stdout);
	    }
	  
	  fprintf (log, "%d\t%.2f\t%d\t%d\t%.2f\t%d\t%.2f\t%d\t"
		   "%.2f\t%.2f\t%.2f\t%.2f\t%d\t"
		   "%s%s%s%s%s%s%s%s%s%s%s%s\r\n",
		   debug[LOG_MSTICS] +
		   debug[LOG_MINTICS] * 60000,
		   debug[LOG_CPULOAD] / 128.0,
		   debug[LOG_THROTTLE],

		   debug[LOG_CURRENT],

		   debug[LOG_PWM] / 10.0,

		   debug[LOG_PACKV],
		   ((int16_t) debug[LOG_TEMP])/10.0,
		   debug[LOG_RPM],
		   debug[LOG_INPUT1] * 0.000080645,
		   debug[LOG_INPUT2] * 0.000080645,
		   debug[LOG_INPUT3] * 0.000080645,
		   debug[LOG_AUXV] * 0.00025939941 + 1.0,
		   debug[LOG_MODE], mode[debug[LOG_MODE] & 0x1f],
		   
		   debug[LOG_MODE] & LIMIT_HIMOTORV ? ", High motor volt" : "",
		   debug[LOG_MODE] & LIMIT_HIMOTORP ? ", High motor power" : "",
		   debug[LOG_MODE] & LIMIT_THROTTLE ? ", Throttle limit" : "",
		   debug[LOG_MODE] & LIMIT_OVERREV ? ", RPM high" : "",

		   debug[LOG_MODE] & LIMIT_LOWPACKV ? ", Low pack volt" : "",
		   debug[LOG_MODE] & LIMIT_HIPACKC ? ", High pack current" : "",
		   debug[LOG_MODE] & LIMIT_HVC ?
		   ", High pack volt throttle limit" : "",
		   debug[LOG_MODE] & LIMIT_CTEMP ?
		   ", Controller temp high" : "",
		   
		   debug[LOG_MODE] & LIMIT_SLEWRATE ? ", Slewrate active" : "",
		   debug[LOG_MODE] & LIMIT_BLOCKED ? ", Throttle blocked" : "",
		   debug[LOG_MODE] & LIMIT_BRAKES ? ", Brakes active" : "");
	}
      else
	{
	  printf ("\r                                                                             \r");
	  if (numbytes)
	    {
	      buf[numbytes] = '\0';
	      while (strlen (buf) && buf[strlen (buf) - 1] < ' ')
		buf[strlen (buf) - 1] = '\0';
	      printf ("%s\n", buf);
	    }
	  else
	    {
	      printf ("Empty message!\n");
	    }
	}
    }
}
