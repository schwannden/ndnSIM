#ifndef NUMOFCLIENT
#define NUMOFCLIENT (4)
#endif

#ifndef NUMOFSERVER
#define NUMOFSERVER (3)
#endif

#ifndef CONTENTNAME
#define CONTENTNAME ("/content")
#endif

#ifndef CONTENTSIZE
#define CONTENTSIZE ("10.0")  /*MB*/
#endif

#ifndef INTERMISSION
#define INTERMISSION (4)
#endif

#ifndef ENDTIME
#define ENDTIME (20.0)
#endif

#ifndef TOPOLOGY
#define TOPOLOGY ("src/ndnSIM/examples/topologies/scenario1.txt")
#endif

#define CONFIG_T1  (0.0)
#define CONFIG_E1  (ndn::LinkControlHelper::FailLink)
#define CONFIG_N11 ("router1")
#define CONFIG_N12 ("server1")

#define CONFIG_T2  (0.0)
#define CONFIG_E2  (ndn::LinkControlHelper::FailLink)
#define CONFIG_N21 ("router1")
#define CONFIG_N22 ("server2")

#define CONFIG_T3  (16.0)
#define CONFIG_E3  (ndn::LinkControlHelper::UpLink)
#define CONFIG_N31 ("router1")
#define CONFIG_N32 ("server1")

#define CONFIG_T4  (16.0)
#define CONFIG_E4  (ndn::LinkControlHelper::UpLink)
#define CONFIG_N41 ("router1")
#define CONFIG_N42 ("server2")

#define CONFIG_T5  (1000.0)
#define CONFIG_E5  (ndn::LinkControlHelper::UpLink)
#define CONFIG_N51 ("router1")
#define CONFIG_N52 ("server1") 
#define CONFIG_T6  (1000.0)
#define CONFIG_E6  (ndn::LinkControlHelper::UpLink)
#define CONFIG_N61 ("router1")
#define CONFIG_N62 ("server1")

