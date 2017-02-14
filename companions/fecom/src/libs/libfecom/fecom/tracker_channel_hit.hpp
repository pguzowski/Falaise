//! \file fecom/tracker_hit.hpp
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2016 by Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>

#ifndef FECOM_TRACKER_CHANNEL_HIT_HPP
#define FECOM_TRACKER_CHANNEL_HIT_HPP

// Standard library:
#include <string>
#include <iostream>

// - Bayeux:
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.h>

// This project:
#include <fecom/base_hit.hpp>
#include <fecom/tracker_constants.hpp>

namespace fecom {

  struct tracker_channel_hit
    : public base_hit
  {

		struct find_by_channel {
			find_by_channel(const uint16_t & feast_id_,
											const uint16_t & channel_id_) : feast_id(feast_id_),
																											channel_id(channel_id_) {}

			bool operator()(const tracker_channel_hit & tracker_channel_hit_) {
				uint16_t input_feast_id = tracker_channel_hit_.feast_id;
				uint16_t input_channel_id = tracker_channel_hit_.channel;
				// std::clog << "DEBUG : TRACKER_CHANNEL_HIT::FIND_BY_CHANNEL : input_feast = " << input_feast_id
				//				<< "input_channel = " << input_channel_id << std::endl;
				bool finded = false;
				if (feast_id == input_feast_id && channel_id == input_channel_id) finded = true;
				else finded = false;
				return finded;
			}

			uint16_t feast_id;
			uint16_t channel_id;
		};

		struct find_by_timestamp {
			find_by_timestamp(const uint16_t & feast_id_,
												const uint16_t & channel_id_,
												const std::string & timestamp_type_) : feast_id(feast_id_),
																															 channel_id(channel_id_),
																															 timestamp_type(timestamp_type_) {}

			bool operator()(const tracker_channel_hit & tracker_channel_hit_) {
				uint16_t input_feast_id = tracker_channel_hit_.feast_id;
				uint16_t input_channel_id = tracker_channel_hit_.channel;
				std::string input_timestamp_type = tracker_channel_hit_.timestamp_type;
				// std::clog << "DEBUG : TRACKER_CHANNEL_HIT::FIND_BY_TIMESTAMP : input_feast = " << input_feast_id
				//				<< " input_channel = " << input_channel_id
				//				<< " input timestamp type = " << input_timestamp_type << std::endl;
				bool finded = false;
				if (feast_id == input_feast_id
						&& channel_id == input_channel_id
						&& timestamp_type == input_timestamp_type) finded = true;
				else finded = false;
				return finded;
			}

			uint16_t feast_id;
			uint16_t channel_id;
			std::string timestamp_type;
		};

    enum channelmode_type {
      INVALID_CHANNEL  = 0,
      ANODIC_CHANNEL   = 1,
      CATHODIC_CHANNEL = 2
    };

    tracker_channel_hit();

    virtual ~tracker_channel_hit();

    bool is_valid() const;

    virtual void reset();

    virtual void tree_dump(std::ostream & out_,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;


    /// Reset all times (anodic and cathodic)
    void reset_timestamp();

  private:

    void _reset_();

  public:

		// Management :
		mutable bool associated = false;

    // Config :
    uint16_t feast_id; ///< FEAST (0..1)
    uint16_t channel;  ///< Channel (0..15)
    uint16_t event_id; ///< Debug counter (0..255)
    channelmode_type channel_type; ///< Channel type (anodic || cathodic)
    std::string timestamp_type;    ///< Type of the timestamp (t0..t6)

    // Data :
    uint64_t timestamp_value;      ///< Value of the timestamp

		DATATOOLS_SERIALIZATION_DECLARATION()

    friend class tracker_channel_hit_parser;
  };

} // namespace fecom

#endif // FECOM_TRACKER_CHANNEL_HIT_HPP

// Local Variables: --
// Mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
