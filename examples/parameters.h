#define NUMOFCLIENT (8)
#define NUMOFSERVER (3)
#define CONTENTNAME ("/content")
#define CONTENTSIZE ("20.0")
#define INTERMISSION (10.0)
#define ENDTIME (30.0)
#define TOPOLOGY ("src/ndnSIM/examples/topologies/topology2.txt")

#define CONFIG_T1 (6.0)
#define CONFIG_E1 (ndn::LinkControlHelper::FailLink)
#define CONFIG_N11 ("router1")
#define CONFIG_N12 ("server1")

#define CONFIG_T2 (6.0)
#define CONFIG_E2 (ndn::LinkControlHelper::FailLink)
#define CONFIG_N21 ("router1")
#define CONFIG_N22 ("server2")

#define CONFIG_T3 (16.0)
#define CONFIG_E3 (ndn::LinkControlHelper::FailLink)
#define CONFIG_N31 ("router3")
#define CONFIG_N32 ("server2")

#define CONFIG_T4 (16.0)
#define CONFIG_E4 (ndn::LinkControlHelper::FailLink)
#define CONFIG_N41 ("router3")
#define CONFIG_N42 ("server3")

#define CONFIG_T5 (22.0)
#define CONFIG_E5 (ndn::LinkControlHelper::UpLink)
#define CONFIG_N51 ("router1")
#define CONFIG_N52 ("server2")

#define CONFIG_T6 (22.0)
#define CONFIG_E6 (ndn::LinkControlHelper::UpLink)
#define CONFIG_N61 ("router3")
#define CONFIG_N62 ("server2")

#define CONFIG_T7 (24.0)
#define CONFIG_E7 (ndn::LinkControlHelper::UpLink)
#define CONFIG_N71 ("router1")
#define CONFIG_N72 ("server1")

#define CONFIG_T8 (24.0)
#define CONFIG_E8 (ndn::LinkControlHelper::UpLink)
#define CONFIG_N81 ("router3")
#define CONFIG_N82 ("server3")

