//  Created by Alejandro Isaza on 2014-04-28.
//  Copyright (c) 2014 Venture Media Labs. All rights reserved.

#include "MeasureGeometry.h"

#include <iostream>

#include "BarlineGeometry.h"
#include "BeamGeometry.h"
#include "ChordGeometry.h"
#include "ClefGeometry.h"
#include "KeyGeometry.h"
#include "NoteGeometry.h"
#include "RestGeometry.h"
#include "TimeSignatureGeometry.h"

#include <mxml/dom/Backup.h>
#include <mxml/dom/Forward.h>
#include <mxml/Metrics.h>

namespace mxml {

using namespace dom;

const coord_t MeasureGeometry::kGraceNoteScale = 0.85;
const coord_t MeasureGeometry::kVerticalPadding = 40;

MeasureGeometry::MeasureGeometry(const Measure& measure,
                                 const SpanCollection& spans,
                                 const ScoreProperties& scoreProperties,
                                 const Metrics& metrics)
: _measure(measure),
  _spans(spans),
  _scoreProperties(scoreProperties),
  _metrics(metrics),
  _partIndex(metrics.partIndex())
{
    build();
}

void MeasureGeometry::build() {
    _currentTime = 0;
    
    for (auto& node : _measure.nodes()) {
        if (const Attributes* attributes = dynamic_cast<const Attributes*>(node.get())) {
            buildAttributes(attributes);
        } else if (const Barline* barline = dynamic_cast<const Barline*>(node.get())) {
            buildBarline(barline);
        } else if (const TimedNode* timedNode = dynamic_cast<const TimedNode*>(node.get())) {
            buildTimedNode(timedNode);
        }
    }

    std::vector<ChordGeometry*> chords;
    for (std::size_t i = 0; i < _geometries.size(); i += 1) {
        auto& geom = _geometries[i];
        ChordGeometry* chordGeom = dynamic_cast<ChordGeometry*>(geom.get());
        if (!chordGeom)
            continue;
        
        const Note* note = chordGeom->chord().firstNote();
        if (note->beams().empty())
            continue;
        
        chords.push_back(chordGeom);
        
        const auto& beam = note->beams().front();
        if (beam->type() == Beam::kTypeBegin) {
        } else if (beam->type() == Beam::kContinue) {
        } else if (beam->type() == Beam::kTypeEnd) {
            std::unique_ptr<BeamGeometry> beamGeom(new BeamGeometry(chords));
            ChordGeometry* firstChordGeom = chords.front();
            beamGeom->setLocation(firstChordGeom->location());
            chords.clear();
            
            addGeometry(std::move(beamGeom));
        }
    }

    setSize({_spans.width(_measure.index()), _metrics.stavesHeight() + 2*kVerticalPadding});
    setContentOffset({0, -kVerticalPadding});

    centerLoneRest();
}

void MeasureGeometry::buildAttributes(const Attributes* attributes) {
    const auto staves = _metrics.staves();
    for (int staff = 1; staff <= staves; staff += 1) {
        if (!attributes->clef(staff))
            continue;

        const auto& clef = attributes->clef(staff);
        auto it = _spans.with(clef);
        if  (it == _spans.end())
            continue;

        std::unique_ptr<ClefGeometry> geo(new ClefGeometry(*clef));
        geo->setStaff(staff);

        const Span& span = *it;
        Point location;
        location.x = span.start() + span.width()/2 - _spans.origin(_measure.index());
        location.y = _metrics.staffOrigin(staff) + Metrics::staffHeight()/2;
        geo->setLocation(location);
        
        addGeometry(std::move(geo));
    }
    
    for (int staff = 1; staff <= staves; staff += 1) {
        if (!attributes->time())
            continue;
        
        const auto& time = attributes->time();
        auto it = _spans.with(time);
        if  (it == _spans.end())
            continue;
        
        const Span& span = *it;

        std::unique_ptr<TimeSignatureGeometry> geo(new TimeSignatureGeometry(*time));
        geo->setStaff(staff);

        Point location;
        location.x = span.start() + span.width()/2 - _spans.origin(_measure.index());
        location.y = _metrics.staffOrigin(staff) + Metrics::staffHeight()/2;
        geo->setLocation(location);
        
        addGeometry(std::move(geo));
    }
    
    for (int staff = 1; staff <= staves; staff += 1) {
        const auto& key = _scoreProperties.key(_partIndex, _measure.index(), staff, attributes->start());
        if (!key)
            continue;
        
        auto it = _spans.with(key);
        if  (it == _spans.end())
            continue;
        
        const Span& span = *it;
        const auto& clef = _scoreProperties.clef(_partIndex, _measure.index(), staff, attributes->start());
        std::unique_ptr<KeyGeometry> geo;
        
        if (key->fifths() == 0) {
            const auto& activeKey = _scoreProperties.key(_partIndex, _measure.index(), staff, attributes->start()-1);
            if (!activeKey)
                continue;
            
            geo.reset(new KeyGeometry(*activeKey, *clef));
            geo->setNatural(true);
        } else {
            geo.reset(new KeyGeometry(*key, *clef));
        }
    
        Point location;
        location.x = span.start() + span.width()/2 - _spans.origin(_measure.index());
        location.y = _metrics.staffOrigin(staff) + Metrics::staffHeight()/2;
        geo->setLocation(location);
        geo->setStaff(staff);
        
        addGeometry(std::move(geo));
    }
}

void MeasureGeometry::buildBarline(const Barline* barline) {
    std::unique_ptr<BarlineGeometry> geo(new BarlineGeometry(*barline, _metrics));
    geo->setVerticalAnchorPointValues(0, 0);
    geo->setHorizontalAnchorPointValues(0, 0);

    auto it = _spans.with(barline);
    assert(it != _spans.end());

    const Span& span = *it;
    geo->setLocation({span.start() - _spans.origin(_measure.index()), 0});

    addGeometry(std::move(geo));
}

void MeasureGeometry::buildTimedNode(const TimedNode* node)  {
    if (auto forward = dynamic_cast<const Forward*>(node)) {
        _currentTime += forward->duration();
        return;
    }
    
    if (auto backup = dynamic_cast<const Backup*>(node)) {
        _currentTime -= backup->duration();
        if (_currentTime < 0)
            _currentTime = 0;
            return;
    }
    
    if (_currentTime != node->start()) {
        _currentTime = node->start();
    }
    
    if (auto chord = dynamic_cast<const Chord*>(node)) {
        buildChord(chord);
    } else if (auto note = dynamic_cast<const Note*>(node)) {
        buildRest(note);
    }
}

void MeasureGeometry::buildChord(const Chord* chord) {
    if (!chord->firstNote()->printObject)
        return;
    
    std::unique_ptr<ChordGeometry> geo(new ChordGeometry(*chord, _scoreProperties, _metrics));

    Point location = geo->refNoteLocation();
    if (chord->stem() == kStemUp) {
        geo->setHorizontalAnchorPointValues(0, location.x - geo->contentOffset().x);
        geo->setVerticalAnchorPointValues(1, -(geo->size().height - (location.y - geo->contentOffset().y)));
    } else {
        geo->setHorizontalAnchorPointValues(1, -(geo->size().width - (location.x - geo->contentOffset().x)));
        geo->setVerticalAnchorPointValues(0, location.y - geo->contentOffset().y);
    }

    auto it = _spans.with(chord);
    assert(it != _spans.end());

    const Span& span = *it;
    if (chord->firstNote()->grace()) {
        location.x = span.start() - _spans.origin(_measure.index()) + geo->anchorPoint().x;
    } else {
        location.x = span.start() + span.eventOffset() - _spans.origin(_measure.index());
    }
    geo->setLocation(location);

    addGeometry(std::move(geo));
}

void MeasureGeometry::buildRest(const Note* note) {
    assert (note->rest());
    if (!note->printObject)
        return;
    
    std::unique_ptr<RestGeometry> geo(new RestGeometry(*note));

    auto it = _spans.with(note);
    assert(it != _spans.end());

    const Span& span = *it;
    Point location;
    location.x = span.start() + span.eventOffset() - _spans.origin(_measure.index());
    location.y = _metrics.noteY(*note);
    geo->setLocation(location);
    addGeometry(std::move(geo));
}

void MeasureGeometry::centerLoneRest() {
    std::map<int, RestGeometry*> staffRests;
    
    for (auto& geometry : geometries()) {
        NoteGeometry* note = dynamic_cast<NoteGeometry*>(geometry.get());
        if (note) {
            staffRests[note->note().staff()-1] = nullptr;
            continue;
        }
        
        ChordGeometry* chord = dynamic_cast<ChordGeometry*>(geometry.get());
        if (chord) {
            staffRests[chord->chord().firstNote()->staff() - 1] = nullptr;
            continue;
        }
        
        RestGeometry* rest = dynamic_cast<RestGeometry*>(geometry.get());
        if (rest) {
            const int staff = rest->note().staff() - 1;
            if (staffRests.find(staff) != staffRests.end()) {
                staffRests[staff] = nullptr;
            } else {
                staffRests[staff] = rest;
            }
        }
    }
    
    // If there is only a Rest on this Measures Staff, center it
    for (auto& staffRest : staffRests) {
        RestGeometry* rest = staffRest.second;
        if (rest) {
            auto location = rest->location();
            const Span& span = *_spans.with(&rest->note());
            auto start = -_spans.origin(_measure.index()) + span.start();
            location.x = start + (size().width - start) / 2 - rest->size().width/2;
            rest->setLocation(location);
        }
    }
}

} // namespace mxml
