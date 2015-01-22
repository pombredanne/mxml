//  Created by Alejandro Isaza on 2014-05-05.
//  Copyright (c) 2014 Venture Media Labs. All rights reserved.

#pragma once
#include "Geometry.h"
#include "MeasureGeometry.h"
#include "TieGeometry.h"

#include <mxml/ScoreProperties.h>
#include <mxml/ScrollMetrics.h>
#include <mxml/dom/Part.h>
#include <mxml/dom/Direction.h>
#include <mxml/dom/Ornaments.h>

namespace mxml {

class BarlineGeometry;
class ChordGeometry;

class PartGeometry : public Geometry {
public:
    explicit PartGeometry(const dom::Part& part, const ScoreProperties& scoreProperties, const ScrollMetrics& metrics, const SpanCollection& spans);
    
    const dom::Part& part() const {
        return _part;
    }
    
    const std::vector<MeasureGeometry*>& measureGeometries() const {
        return _measureGeometries;
    }
    const std::vector<TieGeometry*>& tieGeometries() const {
        return _tieGeometries;
    }
    const std::vector<PlacementGeometry*>& directionGeometries() const {
        return _directionGeometries;
    }
    std::vector<PlacementGeometry*>& directionGeometries() {
        return _directionGeometries;
    }

    std::size_t staves() const {
        return _staves;
    }
    dom::tenths_t staffDistance() const {
        return _staffDistance;
    }
    dom::tenths_t stavesHeight() const {
        return Metrics::stavesHeight(_staves, _staffDistance);
    }
    dom::tenths_t staffOrigin(int staffNumber) const {
        return (staffNumber - 1) * (Metrics::staffHeight() + _staffDistance);
    }

    /**
     Return the y position of a note in this part geometry.
     */
    dom::tenths_t noteY(const dom::Note& note) const;

private:
    const dom::Part& _part;
    const ScoreProperties& _scoreProperties;
    const ScrollMetrics& _metrics;
    
    std::size_t _staves;
    dom::tenths_t _staffDistance;

    std::vector<MeasureGeometry*> _measureGeometries;
    std::vector<TieGeometry*> _tieGeometries;
    std::vector<PlacementGeometry*> _directionGeometries;
    
    friend class PartGeometryFactory;
};

} // namespace mxml
