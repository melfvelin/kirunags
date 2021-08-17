//
// Copyright 2010-2011,2014 Ettus Research LLC
// Copyright 2018,2019 Ettus Research, a National Instruments Company
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#include <uhd/usrp/multi_usrp.hpp>
#include <uhd/utils/safe_main.hpp>
#include <uhd/utils/thread.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>
#include <complex>
#include <csignal>
#include <iostream>

#include "ccommon.h"
#include "constellations.h"
#include "prbs.h"
#include "linecode.h"
#include "transmitter.h"

namespace po = boost::program_options;

static bool stop_signal_called = false;
void sig_int_handler(int)
{
    stop_signal_called = true;
}

int test_exec(uhd::usrp::multi_usrp::sptr usrp, std::vector<std::complex<float> > modulatedSamps, size_t samps_to_send)
{
    // create a transmit streamer
    uhd::stream_args_t stream_args("fc32"); // complex floats
    stream_args.channels             = channel_nums;
    uhd::tx_streamer::sptr tx_stream = usrp->get_tx_stream(stream_args);

    
    std::signal(SIGINT, &sig_int_handler);
    if (repeat)
        std::cout << "Press Ctrl + C to quit..." << std::endl;

    double time_to_send = seconds_in_future;

    // this should be the set-up for sending
    
    TransmitterObj.Execute(OriginalBits, fcxUpSRRC);
    
    // setup metadata for the first packet 
    uhd::tx_metadata_t md;
    md.start_of_burst = false;
    md.end_of_burst   = false;
    md.has_time_spec  = false;
    md.time_spec      = uhd::time_spec_t(time_to_send);

    // the first call to send() will block this many seconds before sending:
    double timeout = std::max(rep_rate, seconds_in_future)
                         + 0.1; // timeout (delay before transmit + padding)

    // sending samples here spb = samps per buffer
    size_t num_tx_samps;

    do 
    {
        num_tx_samps += tx_stream->send(&fcxUpSRRC[0], fcxUpSRRC.size(), md, timeout); 
    }
    while(not stop_signal_called and repeat);
        
    if (stop_signal_called) 
    {
        std::cout << std::endl << "Done!" << std::endl << std::endl;
        std::cout << std::endl << "Transmitted " << num_tx_samps << " samples" << std::endl;
        exit(EXIT_FAILURE);
    }
        
    

    // finished
    std::cout << std::endl << "Done!" << std::endl << std::endl;

    return EXIT_SUCCESS;
}
