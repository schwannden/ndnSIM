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
#include "ns3/ndnSIM/utils/tracers/ndn-l3-rate-tracer.h"
#include "ns3/ndnSIM/plugins/topology/rocketfuel-weights-reader.h"


#ifndef NUMOFCLIENT
#define NUMOFCLIENT 4
#endif

#ifndef NUMOFSERVER
#define NUMOFSERVER 3
#endif

#ifndef CONTENTNAME
#define CONTENTNAME "/content"
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
  ndnHelper.SetForwardingStrategy ("ns3::ndn::fw::SmartFlooding", "EnableNACKs", "true");
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
  consumerHelper.SetStrategy (1);
  consumerHelper.SetAttribute ("Size", StringValue ("8.0"));

  string nodeName( "clientn" );
  for (int i = 0 ; i < NUMOFCLIENT ; i++)
    {
      nodeName[ nodeName.size() - 1 ] = '1'+i;
      Ptr<Node> client = Names::Find<Node> (nodeName);
      app = consumerHelper.Install (client);
      app.Start (Seconds (10*i));
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

  ConfigureTopology ("src/ndnSIM/examples/topologies/congestion-zoom.txt");
  InstallNdnStack ( true );
  AddNdnApplications ();

  Simulator::Stop (Seconds (40.0));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

