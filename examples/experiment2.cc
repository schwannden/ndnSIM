/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 University of California, Los Angeles
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */
// ndn-simple.cc
#include "ns3/core-module.h"
#include "ns3/names.h"
#include "ns3/ptr.h"
#include "ns3/random-variable.h"
#include "ns3/simulator.h"
#include "ns3/ndn-global-routing-helper.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/ndnSIM/apps/ndn-consumer.h"
#include "ns3/ndnSIM/examples/parameters.h"
#include "ns3/ndnSIM/utils/tracers/ndn-l3-rate-tracer.h"
#include "ns3/ndnSIM/plugins/topology/rocketfuel-weights-reader.h"

#ifndef FW_STRATEGY
#define FW_STRATEGY 2
#endif

using namespace std;
using namespace ns3;
using namespace boost;

//topology reader, make it global because it is just a small program
AnnotatedTopologyReader* reader = new AnnotatedTopologyReader ();

void
ConfigureTopology (const std::string &topology)
{
  Names::Clear ();
  if (reader != 0) delete reader;
  reader = new AnnotatedTopologyReader ();

  string input (topology);

  reader->SetFileName (input);
  reader->Read ();
}

void
InstallNdnStack (bool installFIBs/* = true*/)
{
  // Install NDN stack
  ndn::StackHelper ndnHelper;
  ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::SmartFlooding::PerOutFaceLimits",
                                   "EnableNACKs", "true",
                                   "Limit", "ns3::ndn::Limits::Rate");
  ndnHelper.EnableLimits ();
  ndnHelper.SetDefaultRoutes (false);
  ndnHelper.InstallAll ();

  ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
  ndnGlobalRoutingHelper.InstallAll ();

  string nodeName( "servern" );
  for( int i = 0 ; i < NUMOFSERVER ; i++ )
    {
      nodeName[ nodeName.size() - 1 ] = '1'+i;
      Ptr<Node> server = Names::Find<Node> ( nodeName );
      ndnGlobalRoutingHelper.AddOrigins ( CONTENTNAME, server );
    }

  ndn::GlobalRoutingHelper::CalculateRoutes ();
}

void
AddNdnApplications ()
{
  ApplicationContainer app;

  ndn::AppHelper consumerHelper ("ns3::ndn::ConsumerWindow");
  consumerHelper.SetPrefix (CONTENTNAME);
  consumerHelper.SetStrategy (FW_STRATEGY);
  consumerHelper.SetAttribute ("Size", StringValue (CONTENTSIZE));

  string nodeName( "clientn" );
  if( strcmp (TOPOLOGY, "src/ndnSIM/examples/topologies/topology1.txt") == 0 )
    {
      for (int i = 0 ; i < NUMOFCLIENT ; i++)
        {
          nodeName[ nodeName.size() - 1 ] = '1'+i;
          Ptr<Node> client = Names::Find<Node> (nodeName);
          app = consumerHelper.Install (client);
          app.Start (Seconds (i*INTERMISSION));
        }
    }
  else
    {
      for (int i = 0 ; i < NUMOFCLIENT/2 ; i++)
        {
          nodeName[ nodeName.size() - 1 ] = '1'+i;
          Ptr<Node> client = Names::Find<Node> (nodeName);
          app = consumerHelper.Install (client);
          app.Start (Seconds (0.0));
          Simulator::Schedule (Seconds (INTERMISSION), ndn::LinkControlHelper::FailLink,
                               Names::Find<Node> (nodeName), Names::Find<Node> ("router1"));
          Simulator::Schedule (Seconds (2*INTERMISSION), ndn::LinkControlHelper::UpLink,
                               Names::Find<Node> (nodeName), Names::Find<Node> ("router1"));
        }
      for (int i = NUMOFCLIENT/2 ; i < NUMOFCLIENT ; i++)
        {
          nodeName[ nodeName.size() - 1 ] = '1'+i;
          Ptr<Node> client = Names::Find<Node> (nodeName);
          app = consumerHelper.Install (client);
          app.Start (Seconds (INTERMISSION));
        }
    }

  nodeName = "servern";
  ndn::AppHelper producerHelper ("ns3::ndn::Producer");
  for (int i = 0 ; i < NUMOFSERVER ; i++)
    {
      nodeName[ nodeName.size() - 1 ] = '1'+i;
      Ptr<Node> server = Names::Find<Node> (nodeName);
      producerHelper.SetPrefix (CONTENTNAME);
      producerHelper.Install (server);
    }
}

int
main (int argc, char *argv[])
{
  Config::SetDefault ("ns3::PointToPointNetDevice::DataRate", StringValue ("1Mbps"));
  Config::SetDefault ("ns3::ndn::RttEstimator::MaxRTO", StringValue ("1s"));
  Config::SetDefault ("ns3::ndn::ConsumerWindow::Window", StringValue ("1"));

  CommandLine cmd;
  cmd.Parse (argc, argv);

  string prefix = "results/congestion-zoom-ndn-";

  ConfigureTopology (TOPOLOGY);
  InstallNdnStack ( true );
  AddNdnApplications ();

  Simulator::Stop (Seconds (ENDTIME));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

