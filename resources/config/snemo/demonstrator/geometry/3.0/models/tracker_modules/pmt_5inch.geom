# -*- mode: conf-unix; -*-
# pmt_5inch.geom

#################################################
### PMT HAMAMATSU R6594 (5")                  ###
#################################################


##########################################################################
[name="PMT_HAMAMATSU_R6594.dynodes" type="geomtools::simple_shaped_model"]
#@config Simplified PMT dynodes

shape_type : string = "tube"
outer_r    : real as length = 15.0 mm
inner_r    : real as length = 14.5 mm
z          : real as length = 80 mm

material.ref      : string = "std::copper"

visibility.hidden : boolean = 0
visibility.color  : string  = "orange"


#######################################################################
[name="PMT_HAMAMATSU_R6594.base" type="geomtools::simple_shaped_model"]
#@config Simplified PMT base

shape_type : string = "cylinder"
r          : real as length = 25.5 mm
z          : real as length = 30.0 mm

material.ref      : string = "std::delrin"

visibility.hidden : boolean = 0
visibility.color  : string  = "orange"


##################################################################
[name="PMT_HAMAMATSU_R6594" type="geomtools::simple_shaped_model"]

#@config The configuration parameters for the PMT glass bulb and its contents

#@description The default implicit length unit
length_unit : string = "mm"

#@description The shape of the PMT's bulb
shape_type  : string = "polycone"

#@description The polycone build mode
build_mode  : string = "datafile"

#@description The polycone coordinates filename
datafile    : string = "@falaise:config/common/geometry/pmt/1.0/hamamatsu_R6594/hamamatsu_R6594_shape.data"

#@description The 'filled' mode to build the model
filled_mode : string = "by_envelope"

#@description The label of the PMT's bulb volume as daughter volume of the model's envelope
filled_label : string = "bulb"

#######################
# Material parameters #
#######################

#@description The material name
material.ref        : string = "glass"

#@description The inner material name
material.filled.ref : string = "vacuum"

#########################
# Visibility parameters #
#########################

#@description The visibility hidden flag for the display
visibility.hidden           : boolean = 1

#@description The visibility hidden flag for the envelope
visibility.hidden_envelop   : boolean = 0

#@description The recommended color for the display
visibility.color            : string  = "cyan"

#@description The visibility hidden flag for the daughters volumes
visibility.daughters.hidden : boolean = 1

###########################
# Internal/daughter items #
###########################

#@description The list of daughter volumes by labels
internal_item.filled.labels            : string[1] = "dynodes"

#@description The placement of the "dynodes" daughter volume
internal_item.filled.placement.dynodes : string  = "0 0 -30 (mm)"

#@description The model of the "dynodes" daughter volume
internal_item.filled.model.dynodes     : string  = "PMT_HAMAMATSU_R6594.dynodes"


# end of pmt_5inch.geom
