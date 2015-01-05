//  Created by Alejandro Isaza on 2/20/2014.
//  Copyright (c) 2014 Venture Media Labs Inc. All rights reserved.

#include "PitchHandler.h"
#include <mxml/dom/InvalidDataError.h>
#include <cstring>

namespace mxml {

using dom::Pitch;
using lxml::QName;

static const char* kStepTag = "step";
static const char* kAlterTag = "alter";
static const char* kOctaveTag = "octave";

void PitchHandler::startElement(const lxml::QName& qname, const AttributeMap& attributes) {
    _result.reset(new Pitch());
}

lxml::RecursiveHandler* PitchHandler::startSubElement(const QName& qname) {
    if (strcmp(qname.localName(), kStepTag) == 0)
        return &_stringHandler;
    else if (strcmp(qname.localName(), kAlterTag) == 0)
        return &_doubleHandler;
    else if (strcmp(qname.localName(), kOctaveTag) == 0)
        return &_integerHandler;
    return 0;
}

void PitchHandler::endSubElement(const QName& qname, RecursiveHandler* parser) {
    if (strcmp(qname.localName(), kStepTag) == 0)
        _result->setStep(stepFromString(_stringHandler.result()));
    else if (strcmp(qname.localName(), kAlterTag) == 0)
        _result->setAlter(_doubleHandler.result());
    else if (strcmp(qname.localName(), kOctaveTag) == 0)
        _result->setOctave(_integerHandler.result());
    
}

Pitch::Step PitchHandler::stepFromString(const std::string& string) {
    char c = string[0];
    switch (c) {
        case 'C':
        case 'c':
            return Pitch::STEP_C;
        case 'D':
        case 'd':
            return Pitch::STEP_D;
        case 'E':
        case 'e':
            return Pitch::STEP_E;
        case 'F':
        case 'f':
            return Pitch::STEP_F;
        case 'G':
        case 'g':
            return Pitch::STEP_G;
        case 'A':
        case 'a':
            return Pitch::STEP_A;
        case 'B':
        case 'b':
            return Pitch::STEP_B;
            
        default:
            throw dom::InvalidDataError("Invalid step " + string);
    }
}

} // namespace mxml
