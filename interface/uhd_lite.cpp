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

int UHD_SAFE_MAIN(int argc, char* argv[])
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


    CCommon Common;
    enum Test
    {
        BPSK_NRZ_L = 0,
        BPSK_NRZ_S,
        BPSK_NRZ_M,
        BPSK_BP_L,
        BPSK_BP_S,
        BPSK_BP_M,
        QPSK_NRZ_L,
        QPSK_NRZ_S,
        QPSK_NRZ_M,
        QPSK_BP_L,
        QPSK_BP_S,
        QPSK_BP_M
    };

    // Filtering parameters
    Firdes Fir;
    Filter CFilter;
    float fRs = 4e6;
    float fRb = 1e6;
    float fSPS = 4;
    // SatTrack inits
    std::time_t timeArg = time(0);
    double *doubPtr = nullptr;
    doubPtr = new double[5];
    float doppler_factor = 435/100;
    // Filter objects and taps
    std::vector<float> SRRCTaps,SquareTaps, Taps;
    Firdes CFirdes;
    CFirdes.SRRC(1.0,fRs,fRb,0.35,SRRCTaps);
    CFirdes.Square(int(fSPS),SquareTaps);
    Taps = SRRCTaps;// You can choose SRRCTaps or SquareTaps for Upfilter here.

    //************************************TEST******************************************//
    Test Test = BPSK_BP_M;
    std::vector<uint8_t> OriginalBits;  //TRANSMITTER INPUT
    
    // CADU generator
    int nFrameLen = 255*500*8;
    const char* syncword = "0x1ACFFC1D";
    PRBS PRBSObject;
    PRBSObject.Initialize(CCommon::PRBS20,nFrameLen);
    PRBSObject.GetSeq(OriginalBits);
    PRBSObject.AppendSyncWord(syncword,32,OriginalBits);

    std::vector<std::complex<float> > fcxUpSRRC;//TRANSMITTER OUTPUT
    Transmitter TransmitterObj;

    if(Test == BPSK_BP_M)
    {

        // Constellation modulation
        CCommon::Arity nBERArity = CCommon::Binary;
        CCommon::LineCode nLineCode = CCommon::BP_M;

        //transmitter initialize
        //Transmitter TransmitterObj;
        TransmitterObj.Initialize(nLineCode, nBERArity, Taps, fSPS);

        //Transmitter execute
        TransmitterObj.Execute(OriginalBits, fcxUpSRRC);
        std::cout<<fcxUpSRRC.size()<<std::endl;
    }


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

    // set the analog frontend filter bandwidth
    /*
    if (vm.count("bw")) {
        std::cout << boost::format("Setting TX Bandwidth: %f MHz...") % (bw / 1e6)
                  << std::endl;
        usrp->set_tx_bandwidth(bw);
        std::cout << boost::format("Actual TX Bandwidth: %f MHz...")
                         % (usrp->get_tx_bandwidth() / 1e6)
                  << std::endl
                  << std::endl;
    }
    */

    std::cout << boost::format("Setting device timestamp to 0...") << std::endl;
    usrp->set_time_now(uhd::time_spec_t(0.0));

    // create a transmit streamer
    uhd::stream_args_t stream_args("fc32"); // complex floats
    stream_args.channels             = channel_nums;
    uhd::tx_streamer::sptr tx_stream = usrp->get_tx_stream(stream_args);

    
    std::signal(SIGINT, &sig_int_handler);
    if (repeat)
        std::cout << "Press Ctrl + C to quit..." << std::endl;

    double time_to_send = seconds_in_future;

    // this should be the set-up for sending
    
    // modulate OriginalBits
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
