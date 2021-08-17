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

uhd::usrp::multi_usrp::sptr test_init()
{
    // variables to be set by po
    std::string args, channel_list, subdev, ref;
    double seconds_in_future, rate, freq, rep_rate, gain, lo_offset, bw;

    seconds_in_future = 1.5;    // start of first packet
    rate = 3000000;               // 1 MSamps
    freq = 500000000;           // 500 MHz
    gain = 40;                  // 40 dB RF gain
    lo_offset = 0;              // 0 Hz
    rep_rate = 0;               // not used

    args = "";
    channel_list = "0";
    subdev = "A:B";
    ref = "internal";

    bool repeat  = true;
    size_t total_num_samps;     

    // create a usrp device
    std::cout << std::endl;
    std::cout << boost::format("Creating the usrp device with: %s...") % args
              << std::endl;
    uhd::usrp::multi_usrp::sptr usrp = uhd::usrp::multi_usrp::make(args);

    // Lock mboard clocks
    usrp->set_clock_source(ref);
    
    // always select the subdevice first, the channel mapping affects the other settings
    usrp->set_tx_subdev_spec(subdev);

    std::cout << boost::format("Using Device: %s") % usrp->get_pp_string() << std::endl;

    // detect which channels to use
    std::vector<std::string> channel_strings;
    std::vector<size_t> channel_nums;
    boost::split(channel_strings, channel_list, boost::is_any_of("\"',"));
    for (size_t ch = 0; ch < channel_strings.size(); ch++) {
        size_t chan = std::stoi(channel_strings[ch]);
        if (chan >= usrp->get_tx_num_channels()) {
            throw std::runtime_error("Invalid channel(s) specified.");
        } else
            channel_nums.push_back(std::stoi(channel_strings[ch]));
    }

    // set the tx sample rate
    std::cout << boost::format("Setting TX Rate: %f Msps...") % (rate / 1e6) << std::endl;
    usrp->set_tx_rate(rate);
    std::cout << boost::format("Actual TX Rate: %f Msps...") % (usrp->get_tx_rate() / 1e6)
              << std::endl
              << std::endl;

    // set the center frequency
    std::cout << boost::format("Setting TX Freq: %f MHz...") % (freq / 1e6) << std::endl;
    std::cout << boost::format("Setting TX LO Offset: %f MHz...") % (lo_offset / 1e6)
              << std::endl;

    for (size_t i = 0; i < channel_nums.size(); i++) {
        uhd::tune_request_t tune_request;
        tune_request = uhd::tune_request_t(freq, lo_offset);
        usrp->set_tx_freq(tune_request, channel_nums[i]);
    }

    std::cout << boost::format("Actual TX Freq: %f MHz...") % (usrp->get_tx_freq() / 1e6)
              << std::endl
              << std::endl;

    std::cout << boost::format("Setting TX Gain: %f...") % (gain) << std::endl;
    for (size_t i = 0; i < channel_nums.size(); i++)
        usrp->set_tx_gain(gain, channel_nums[i]);
    std::cout << boost::format("Actual TX Gain: %f...") % (usrp->get_tx_gain())
              << std::endl
              << std::endl;

    std::cout << boost::format("Setting device timestamp to 0...") << std::endl;
    usrp->set_time_now(uhd::time_spec_t(0.0));
    return usrp;
}
