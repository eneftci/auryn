/*
* Copyright 2014-2015 Friedemann Zenke
* Contributed by Ankur Sinha
*
* This file is part of Auryn, a simulation package for plastic
* spiking neural networks.
*
* Auryn is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Auryn is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Auryn.  If not, see <http://www.gnu.org/licenses/>.
*
* If you are using Auryn or parts of it for your work please cite:
* Zenke, F. and Gerstner, W., 2014. Limits to high-speed simulations
* of spiking neural networks using general-purpose computers.
* Front Neuroinform 8, 76. doi: 10.3389/fninf.2014.00076
*/

#include "BinarySpikeMonitor.h"


BinarySpikeMonitor::BinarySpikeMonitor(SpikingGroup * source, string filename, NeuronID from, NeuronID to)
	: Monitor(filename)
{
	init(source,filename,from,to);
}

BinarySpikeMonitor::BinarySpikeMonitor(SpikingGroup * source, string filename, NeuronID to)
	: Monitor(filename)
{
	init(source,filename,0,to);
}

BinarySpikeMonitor::BinarySpikeMonitor(SpikingGroup * source, string filename)
	: Monitor(filename )
{
	init(source,filename,0,source->get_size());
}

BinarySpikeMonitor::~BinarySpikeMonitor()
{
	free();
}

void BinarySpikeMonitor::open_output_file(string filename)
{
	if ( filename.empty() ) return; // stimulators do not necessary need an outputfile

	outfile.open( filename.c_str(), ios::binary );
	if (!outfile) {
	  stringstream oss;
	  oss << "Can't open binary output file " << filename;
	  logger->msg(oss.str(),ERROR);
	  exit(1);
	}
}

void BinarySpikeMonitor::init(SpikingGroup * source, string filename, NeuronID from, NeuronID to)
{
	sys->register_monitor(this);

	// sys = system;
	n_from = from;
	n_to = to;
	n_every = 1;
	src = source;
	offset = 0;
}

void BinarySpikeMonitor::free()
{
}

void BinarySpikeMonitor::set_offset(NeuronID of)
{
	offset = of;
}

void BinarySpikeMonitor::set_every(NeuronID every)
{
	n_every = every;
}

void BinarySpikeMonitor::propagate()
{
	struct spikeEvent_type spikeData;
	for (it = src->get_spikes_immediate()->begin() ; it < src->get_spikes_immediate()->end() ; ++it ) {
		if (*it >= n_from ) {
			if ( *it < n_to && (*it%n_every==0) )
            {
                spikeData.time = dt*(sys->get_clock());
                spikeData.neuronID = (*it + offset);
                outfile.write((char*)&spikeData, sizeof(spikeEvent_type));
            }
		}
	}
}
