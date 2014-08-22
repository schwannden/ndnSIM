/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
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

#include "smart-flooding.h"

#include "ns3/ndn-interest.h"
#include "ns3/ndn-pit.h"
#include "ns3/ndn-pit-entry.h"

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/boolean.h"

#include <iostream>
#include <boost/ref.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
namespace ll = boost::lambda;

namespace ns3 {
namespace ndn {
namespace fw {

NS_OBJECT_ENSURE_REGISTERED (SmartFlooding);

LogComponent SmartFlooding::g_log = LogComponent (SmartFlooding::GetLogName ().c_str ());

std::string
SmartFlooding::GetLogName ()
{
  return super::GetLogName ()+".SmartFlooding";
}

TypeId
SmartFlooding::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ndn::fw::SmartFlooding")
    .SetGroupName ("Ndn")
    .SetParent <GreenYellowRed> ()
    .AddConstructor <SmartFlooding> ()
    ;
  return tid;
}

SmartFlooding::SmartFlooding ()
{
}

bool
SmartFlooding::DoPropagateInterest_BestRoute (Ptr<Face> inFace,
                                              Ptr<const Interest> interest,
                                              Ptr<pit::Entry> pitEntry)
{
  int propagatedCount = 0;

  BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
    {
      if (metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_RED) // all non-read faces are in front
        break;

      if (!TrySendOutInterest (inFace, metricFace.GetFace (), interest, pitEntry))
        {
          continue;
        }

      propagatedCount++;
      break; // do only once
    }

  return propagatedCount > 0;
}

bool
SmartFlooding::DoPropagateInterest_Flooding (Ptr<Face> inFace,
                                             Ptr<const Interest> interest,
                                             Ptr<pit::Entry> pitEntry)
{
  int propagatedCount = 0;

  BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
    {
      if (metricFace.GetStatus () == fib::FaceMetric::NDN_FIB_RED) // all non-read faces are in the front of the list
        break;

      if (!TrySendOutInterest (inFace, metricFace.GetFace (), interest, pitEntry))
        {
          continue;
        }

      propagatedCount++;
    }

  return propagatedCount > 0;
}

bool
SmartFlooding::DoPropagateInterest_SmartFlooding (Ptr<Face> inFace,
                                                  Ptr<const Interest> interest,
                                                  Ptr<pit::Entry> pitEntry)
{
  // Try to work out with just the best green face
  if (super::DoPropagateInterest (inFace, interest, pitEntry))
    return true;

  // Otherwise propagate interest in all other yellow/green faces
  int propagatedCount = 0;

  BOOST_FOREACH (const fib::FaceMetric &metricFace, pitEntry->GetFibEntry ()->m_faces.get<fib::i_metric> ())
    {
      if (metricFace.GetStatus () != fib::FaceMetric::NDN_FIB_RED)
        {
          if (TrySendOutInterest (inFace, metricFace.GetFace (), interest, pitEntry))
            {
              propagatedCount++;
            }
          else
            {
            }
        }
    }

  return propagatedCount > 0;
}

bool
SmartFlooding::DoPropagateInterest (Ptr<Face> inFace,
                                    Ptr<const Interest> interest,
                                    Ptr<pit::Entry> pitEntry)
{
  NS_LOG_FUNCTION (this);
  bool propagated = false;
  int strategy = interest->GetStrategy ();
  switch( strategy )
    {
      case 1: propagated = DoPropagateInterest_Flooding (inFace, interest, pitEntry);
                 break;
      case 2: propagated = DoPropagateInterest_SmartFlooding (inFace, interest, pitEntry);
                 break;
      case 3: propagated = DoPropagateInterest_BestRoute (inFace, interest, pitEntry);
                 break;
    }
  return propagated;
}

} // namespace fw
} // namespace ndn
} // namespace ns3
