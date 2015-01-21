//  Created by Alejandro Isaza on 2014-07-09.
//  Copyright (c) 2014 Venture Media Labs. All rights reserved.

#include "SyllabicHandler.h"
#include <mxml/dom/InvalidDataError.h>

namespace mxml {

using dom::Syllabic;

void SyllabicHandler::startElement(const lxml::QName& qname, const AttributeMap& attributes) {
    _result.reset(new Syllabic());
}

void SyllabicHandler::endElement(const lxml::QName& qname, const std::string& contents) {
    _result->setType(typeFromString(contents));
}

dom::Syllabic::Type SyllabicHandler::typeFromString(const std::string& string) {
    if (string == "single")
        return dom::Syllabic::kSingle;
    else if (string == "begin")
        return dom::Syllabic::kBegin;
    else if (string == "end")
        return dom::Syllabic::kEnd;
    else if (string == "middle")
        return dom::Syllabic::kMiddle;
    throw dom::InvalidDataError("Invalid syllabic type " + string);
}

} // namespace
